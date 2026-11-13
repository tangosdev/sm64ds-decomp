# Pointer width

An address kept in an integer is `intptr_t` or `uintptr_t`. On the DS
compiler those are `signed int` and `unsigned int` (`include/types.h`),
so a cast through them is the same bytes as the old `(int)` / `(u32)`
cast. On a 64-bit host they are the host pointer width, from `<stdint.h>`.

There is one spelling. Nothing in `src/` or `include/` grew a second
body for the host.

`(long long)(int)` address launders stay in that spelling. The `(int)`
is the truncation the ROM's addressing fold is built on.

## Still a 32-bit layout

These are the retail layout, not a cast that can be widened under the
same instructions.

- The C++ vptr is 4 bytes. `fBase_c::uniqueID` is at `0x04`, and
  `dActor_c::mPos` is at `0x05c` on top of that. Named members after the
  vptr move together when the vptr grows. A `(char *)this + 0xNNN` does
  not.
- BMD, BCA, BTP, KCL, level object tables, and sound wave-arc links are
  the bytes of the file. The loader adds the file address and stores it
  back into the same slot (`UpdateFileOffsets`). The slot is still a
  native pointer. A `u32` slot is the 4-byte file word, and every reader
  of that field would have to change with it.
- `SharedFilePtr::filePtr` is the loaded buffer, a real pointer.
- A pointer-to-member in the ROM is two 32-bit words, function then
  adjustment.
- `nds_printf` and `OS_SPrintf` still walk a 4-byte ARM argument list
  (`& ~3`, then `+ 4`). The address of the format parameter is
  `uintptr_t`.
- Size locks (`sizeof(T) == N`) and the arena words at `0x027ffda0` /
  `0x027ffdc4` describe the 32-bit object and the 32-bit bus.
- `asm` functions are ARM instructions. They stay ARM.
