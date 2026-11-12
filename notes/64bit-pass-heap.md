# 64-bit pass: heap, printf walker, arena, object offsets

NDS lines stay under `#ifndef SM64DS_HOST_64`, character for character.
Host lines use `uintptr_t` or the named member. `include/ds_width.h` and
`include/launder.h` were not edited. No commit.

Match is `python tools/match.py --version 2004/b56` from this worktree.
Overlay matches add `--bin extracted/overlays/overlay_XXXX.bin --base <yaml> --module ovNNN`.
Every result below is `MATCHING VERSIONS: 2004/b56`. Nothing was reverted.

## Heap

`NNSiGetUIntPtr` is the `(u32)` truncation. There is no `MemoryNode` type.
The user pointer just past the block header is `AddU32ToPtr(block, sizeof(NNSiFndExpHeapMBlockHead))`
(`0x10` on the DS). Widening the helper to `uintptr_t` on the host makes that
add and the pointer comparisons pointer-wide. `GetOffsetFromPtr` still returns
`u32`. Alignment arguments are `int`, so `~(alignment - 1)` sign-extends.
The arena mask is the unsigned case and is handled below.

- `src/engine/heap/ExpandingHeapAllocator.cpp` `NNSiGetUIntPtr`: host returns `(uintptr_t)ptr`.
  Proof: `NNS_FndGetSizeForMBlockExpHeap` `0x0204e084` size `0x8`.
- `src/engine/heap/SolidHeapAllocator.cpp` same helper.
  Proof: `NNS_FndResizeForMBlockFrmHeap` `0x0204e964` size `0xa4` (ROM `add` of `0x24`, `sizeof(NNSiFndHeapHead)` on the DS).
- `src/engine/heap/Heap.cpp` object size `0x18` is `sizeof(ExpandingHeap)` / `sizeof(SolidHeap)` on the host.
  - `Heap_t::SetupRootHeap` `0x0203cb04` size `0xbc`. Host uses `uintptr_t` lo/hi and `~(uintptr_t)0x1f`. Size passed to `OS_AllocFromArenaLo` stays `unsigned int`.
  - `Heap_t::CreateRootHeap` `0x0203c8e8` size `0x88`. Both `size - 0x18` sites. ROM still has `sub #0x18`.
  - `Heap_t::CreateExpandingHeap` `0x0203c844` size `0xa4`. ROM `add r1, r6, #0x18`.
  - `Heap_t::CreateSolidHeap` `0x0203c7a0` size `0xa4`. ROM `add #0x18`.
  - `SolidHeap_t::VResizeToFit` `0x0203c33c` size `0x4c`. ROM `add #0x18`.

## Arena

`src/engine/os/OS_Arena.cpp`. `0x027ffda0` and `0x027ffdc4` stay integer bus addresses.
Host loads them through `(uintptr_t *)(uintptr_t)0x027ffda0` / `...dc4` so the slot pointer is zero-extended.
`OS_GetInitArenaLo` / `OS_GetInitArenaHi` stay `unsigned int` and were not edited.
Host `OS_AllocFromArenaLo` aligns with `~(uintptr_t)(align - 1)`. The size and align parameters stay `unsigned int`.

- `OS_AllocFromArenaLo` `0x02058cd0` size `0x88`
- `OS_SetArenaLo` `0x02058d58` size `0x14` (store at `0xda0`)
- `OS_SetArenaHi` `0x02058d6c` size `0x14` (store at `0xdc4`)
- `OS_GetArenaLo` `0x02058ea0` size `0x14`
- `OS_GetArenaHi` `0x02058eb4` size `0x14`

## printf walker

- `src/nds_printf.c` `nds_printf` `0x0201470c` size `0x30`.
  NDS: `char *args = (char *)(((u32)&fmt & ~3u) + 4);` and `nds_vprintf(dst, fmt, args);`.
  Host: `va_start`, then `nds_vprintf(dst, DS_UADDR(fmt), (void *)ap)`.
- `src/nds_vprintf.c` `nds_vprintf` `0x0201473c` size `0x34`.
  NDS signature `(void *dst, const char *fmt, char *args)` unchanged.
  Host: `(void *dst, uintptr_t fmt, void *args)`, then casts back for `vsnprintf`.
- `src/cstd4.cpp` `__pformatter` already advances pointer arguments by `sizeof(void *)`.
  `vsnprintf` already takes pointers. No walker edit.
  Host leftover, not changed: MSVC x64 `va_list` is not the packed ARM image MSL walks, and integer slots still step by `sizeof(int)` (4 on both targets).

## Appendix B `(long long)(int)`

Host is `(uintptr_t)` with no `(int)`. A parallel `DS_IADDR` sweep rewrote three of these after the first match; they were put back and rematched.

- `src/Message2.cpp` `func_0201b388` `0x0201b388` size `0x220`.
  NDS `sb = (u32)(((long long)(int)start));`. `start` is an `int` tile index.
- `src/runtime/actor/d_cap_enemy_arm9.cpp` `dCapEnemy_c::GetCapState` `0x02005fa0` size `0xb4`.
  NDS `mFlags = *(int *)((long long)(int)&mDormantFlags);`.
- `src/minigames/d_mg3d_heyho_obj_adapter.cpp` `Heyho_SeparateAll` `0x020c42bc` size `0x1f8`, ov006 base `0x020bfec0`.
  NDS `oj = (Heyho *)((long long)(int)&sHeyhos[j]);`.
- `src/game/actors/d_a_obj_block_s.cpp` `daObjBlockS_c::ResetToHome` `0x02138ce0` size `0x128`, ov098 base `0x02137be0`.
  Three NDS lines `((struct Vector3 *)(((long long)(int)&zero)))->{x,y,z} = 0;`.
- `src/game/player/d_a_ply_methods.cpp` `func_ov002_020e6bb0` `0x020e6bb0` size `0x5c`, ov002 base `0x020ad660`.
  NDS `pp = (void **)((long long)(int)((char *)model + 8));`. The `+ 8` stays. Compile finished in well under a minute. Other `DS_IADDR` edits in this file are outside this function and were left.

## Appendix C named members

NDS keeps the raw add. Host uses the field.

- `src/game/actors/d_a_obj_guragura.cpp` `daObjGuragura_c::TiltToward` `0x020b62cc` size `0xa8`, ov002 base `0x020ad660`.
  NDS `MTX_MultVec43((Vector3 *)(arg + 0x5c), ...)`. Host `&((dActor_c *)arg)->mPos` (`dActor_c::mPos` at `0x5c`). ROM still `add r0, r6, #0x5c`.
- `src/game/actors/d_a_huwa.cpp` `daHuwa_c::Behavior` `0x02123c6c` size `0x264`, ov081 `extracted/overlays/overlay_0081.bin` base `0x02123740`.
  NDS `(int *)((int)player + 0x5c)`. Host `&player->mPos`. ROM `add #0x5c`.
- `src/game/actors/d_a_ibl.cpp` `daIbl_c::SpawnerBehavior` `0x02142b90` size `0x18c`, ov100 `overlay_0100.bin` base `0x02140d80`.
  NDS `reinterpret_cast<const unsigned char *>(pl) + 0x5c`. Host `pl->mPosX/mPosY/mPosZ`. ROM `add #0x5c`.
- `src/game/actors/d_a_obj_rotate_updown_lift.cpp` `daObjRotateUpdownLift_c::Behavior` `0x02131468` size `0x488`, ov091 `overlay_0091.bin` base `0x02130f00`.
  NDS `(char *)this + 0x300` then `+ 0xa4`. Host `mPitchStep` (`0x3a4`). ROM still `add #0x300` and `ldrsh [r2, #0xa4]`.
- `src/minigames/d_s_mg_snowball.cpp` `Particle::System::emitRadius` is `s32` at `0x44`.
  - `dScMgSnowball_c::UpdateSnowball` `0x02125f68` size `0x9e0`, ov006 base `0x020bfec0`. NDS `((int *)sys)[0x44 / 4]`.
  - `dScMgSnowball_c::Behavior` `0x021283a4` size `0xc14`, same overlay. NDS `((int *)p)[0x44 / 4] = 0xffff8222;`.

## Skips

- `src/OS_InitContext.c`: `asm void OS_InitContext`. Not edited by this pass. A parallel `SM64DS_PLATFORM_PC` declaration stub around the asm was left in place.
- `src/cstd4.cpp`: no signature change. Parallel `SM64DS_PLATFORM_PC` stubs on `func_0206dab4` and `__builtin_trap` were left.
- `src/parse_format.c`: `*arg += 4` for `*` width and precision. Not an owned file.
- `src/engine/os/OS_Alloc.cpp`: `(HeapDesc *)((u32)arenaStart + sizeof(OSHeapInfo))`. Same truncation class, not an owned file.
- `src/engine/heap/NestedHeapIterator.cpp`: `NNS_FndInitList(..., 4)` is the NDS `offsetof` of the heap-head link. Decimal, not a `(char *) + hex` member. Not edited.
- SolidHeap `this + 0x24`: no raw `(char *)this + 0x24`. The frame-heap head step is `AddU32ToPtr(..., sizeof(NNSiFndHeapHead))`, which is `0x24` on the DS and follows `sizeof` on the host once `NNSiGetUIntPtr` is wide.
- smartball `this + 0x32`: already `mAngle` in the class header. No raw add.
- `cyl + 0x3c` as a pointer-element stride: not found. Hits of `+ 0x3c` are sound ids and `0x3c000` heights.
- `src/game/actors/d_a_obj_block_s.cpp` `BLOCKS_EATING_PLAYER`: `(char *)(o) + 0xd0`. `daObjBlockS_c` is a `dBgActor_c`. The header names that word `u8 pad_0d0[4]`, not a pointer. `dEnemyBase_c::mEatingPlayer` is a different class. Left.
- `src/game/player/d_a_ply_methods.cpp` `func_ov002_020d7030`: `*(void **)((char *)b + 0xd0) = this`. `b` is only a `dActor_c` here (`dActor_c` ends at `0xd0`). The derived field is not named on that static type. Also `*(int *)(*(char **)((char *)&mObjInMouth) + 0xd0)`. Left.
- Player `mGroundSoundType`: already the named field. No `(char *)this` pun.
- Player `data_0209ee90 + 0x238 / 0x244 / 0x248 / 0x24c`: the only named local is `flag26c` at `0x26c`. No header member for those constants. Left.
- G2/G2S character-base and screen offsets, and `Vector3` / matrix strides of `0xc`, were not touched.
- Asm functions, `launder.h`, `ds_width.h`, and the file-format / overlay headers named in the task were not touched.
