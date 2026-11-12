# 64-bit host seam

What still assumes a pointer is 4 bytes, after the `DS_P32` / `DS_IADDR` pass.
`port/` is not a source of these bugs. The matching build is unchanged: every
macro below is empty or is the original cast, and the functions that were
edited were matched on mwccarm 2004/b56.

The seam is `include/ds_width.h`.

- `DS_P32` is nothing on the NDS build and MSVC `__ptr32` when
  `SM64DS_HOST_64` is set (`SM64DS_PLATFORM_PC` and `_WIN64` / `__LP64__`).
  `T * DS_P32 name` is a 4-byte slot on both.
- `DS_IADDR` / `DS_UADDR` are `(int)` / `(u32)` on the NDS build and
  `intptr_t` / `uintptr_t` on a 64-bit host.
- `include/launder.h` keeps the 64-bit mask on the NDS build. On a 64-bit
  host the mask was dropping the top half of the address, so the host
  spelling reloads through `uintptr_t`.

An address stored through `DS_P32` has to sit in the low 4GB. That covers
file images (the rebase writes the sum back into the slot) and
`SharedFilePtr::filePtr` (the loaded buffer is stored in 4 bytes). The game
heap on a 64-bit host has to return those addresses. A native pointer field
is not that: list links, `OSThread`, a C++ member. Those widen. The vptr is
8 bytes because the compiler emits it.

## Fixed

File and bus records whose slots are the bytes on disk or the bytes the ARM7
reads. `DS_P32` on the slot, size unchanged:

- BMD, BCA, BTP, KCL header (`positions`, `normals`, `tris`, `octree`)
- level object tables (`ObjTable::entries`, `ObjSubTable::entries`)
- `FSOverlayInfo` `ram_address`, `sinit_init`, `sinit_init_end`
- `SNDWaveArcLink` / `SNDWaveArc::topLink`
- `SharedFilePtr::filePtr` and the same shape on `KuriKingAnimFile` and
  `PathStuff`

`dBgW_Kc::kclFile` stays a native pointer. It is a member of a virtual class,
not a slot in the file.

Rebase functions (`UpdateFileOffsets` for BMD, BCA, BTP, KCL) keep the NDS
arithmetic and, on the host, add a `uintptr_t` base. The BMD skin block is
four `DS_P32` slots, not a `void **`. Matched.

194 casts of a real address to `int` / `u32` / `unsigned` go through
`DS_IADDR` / `DS_UADDR`. Matched, including the eight `daPly_c` methods that
changed. Two static initializers (`__sinit_KuppaScript.cpp`,
`__sinit_d_s_mg_memory2.cpp`) fail a strict reloc-destination check with no
instruction difference; the unmodified sources fail the same way.

53 size locks that contain a pointer, a pointer-to-member, or a virtual
function and were not already under `#ifndef SM64DS_PLATFORM_PC` are under
`#ifndef SM64DS_HOST_64`. Integer-only locks stay (a `Matrix4x3` is still
`0x30`). File-handle locks of 8 stay, because the slot is `DS_P32`.

79 `asm` functions are inside `#ifndef SM64DS_PLATFORM_PC`. The host side is
a declaration when `port/` already defines the symbol or the routine is
CP15, BIOS, startup, IRQ, or a context switch. Eight plain algorithms that
`port/` does not define have a C body: the `MTX_Identity*` / `MTX_Rot*`
helpers, `OS_SpinWait`, and `func_02059d98`. `port/tools/host_frontier.py`
syntax-checks a wrapped file instead of skipping every file that contains
the word `asm`.

Heap and arena, host side only:

- `ExpandingHeapAllocator` address compares use `uintptr_t`. The user pointer
  is `sizeof(MemoryNode)` past the node. The NDS lines stay `0x10` and
  `(u32)`.
- The arena API stays `unsigned int`. Those values are bus addresses in a
  4-byte table at `0x027ffda0` / `0x027ffdc4`. On the host the constant is
  zero-extended to a pointer; the slot width does not change. A second
  signature next to the first fails the declaration check, which reads both
  sides of an `#if`.
- A handful of object offsets (`mPos`, `mPitchStep`, `mPosX/Y/Z`,
  `emitRadius`) use the named member on the host and the old byte offset on
  the NDS build.

`port/host_abi.h` no longer refuses a 64-bit host. A 32-bit PC build still
requires `sizeof(void *) == 4`.

Per-pass lists: `notes/64bit-pass-files.md`, `notes/64bit-pass-casts.md`,
`notes/64bit-pass-sizes.md`, `notes/64bit-pass-asm.md`,
`notes/64bit-pass-heap.md`.

## Still blocked

The vptr. 425 headers contain `virtual`. On a 64-bit host that word is 8
bytes, so `fBase_c::uniqueID` is not at `0x04` and `dActor_c` position is not
at `0x05c`. Named members move with it. A `(char *)this + 0xNNN` does not.
The passes above converted the ones that had a name and a small function.
What is left is the rest of that shape, mostly in
`src/game/player/d_a_ply_methods.cpp` (`data_0209ee90 + 0x238/0x244/...`,
`+ 0xd0` held-actor slots) and the actor bytes the heap pass could not name
(`pad_0d0`, `BLOCKS_EATING_PLAYER`). Screen and character-base offsets
(`G2::GetBG*Ptr() + 0xNNN`) are VRAM, not this.

Pointer-to-member. The ROM record is 8 bytes, function word plus adjustment.
MSVC's member pointer is not that record. Copying a slot as two words, or
testing `*(void **)&slot`, follows the ROM layout. The declarations were
left as real member pointers.

`FSOverlayInfo::sinit_init` is a 4-byte slot that points at a table of
function pointers. A host `++` over that table strides by 8.

Soft-float and printf. `src/cstd4.cpp` still passes `(int)&out` into
`func_0206de14`, whose parameter is `int`. `src/nds_printf.c` and
`src/engine/os/OS_Printf.c` align `&fmt` to 4 and walk a packed ARM argument
list. `cstd4.cpp` already steps a pointer argument by `sizeof(void *)`. The
format pointer and the argument pointer still cross as `u32`, and a host
`va_list` is not that image. `src/engine/os/OS_Exception.cpp` passes
`(u32)&data_020a63ac` to the user handler.

`__cxa_vec_delete` reads the element count at `((int *)a)[-1]`. Callers pass
the cookie size. That is this game's array ABI, not the host compiler's
`operator new[]`. Element sizes of `0x0c` (`Vector3`) and `6` (`Vector3s`)
are not pointer widths. An element size of 8 is a pair of 4-byte slots
wherever the element is two pointers.

Absolute machine addresses (`0x04000000`, ITCM `0x01ff8000`, the arena words)
are bus addresses. `port/tools/hostgen.py` rewrites MMIO dereferences in the
build tree. They are not host pointers.

`asm` that stayed a declaration (CP15, BIOS decompressors, `_start`, IRQ,
context switch, the soft-float routines whose calling convention is not a C
signature) links to `port/` when the HAL has the symbol, and is unresolved
when it does not. That is the HAL backlog, not a 4-byte pointer.
