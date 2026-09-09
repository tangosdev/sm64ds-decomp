//cpp
// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// Model::LoadCompressedTextureToVram @ 0x02045b58 (arm9, size 0xb8). 5 words diverge:
// the allocator build delta from PR #957 -- ROM gives sumA a fresh register and colors
// sumB onto the just-freed one; every owned build reuses the dying operand register.
// 17-probe pass, flags, versions, value/address launders all closed (notes 6ay, DB row).
// Register-choice-only delta: functionally identical. Supersedes the older 13-div draft.
//
// RE-MEASURED 2026-09-09 (run link100 lane MATCH4): the `volatile` on the last two
// statements is buying the low number by pinning the WRONG SHAPE. volatile forces
// load-add-store then load-add-store; the ROM emits load, load, add, str, add, str.
// Drop it and 2004/b56 emits the ROM's shape exactly, and the residue becomes a pure
// 8-word register rotation: ROM pool addresses r2,r1 with values r0,r3 and the first
// sum in a fresh r4, versus pool r1,r0 with values r3,r2 reused in place and both adds
// before both stores. So div=5 is a launder artefact and div=8 is the honest number for
// the right shape; the 6bs fresh-register choice is what is actually out of reach.
// Measured inert on the un-volatiled form, all 8: plain `+=` on both globals; both old
// values read into locals first; those locals plus a named `size >> 1`; the explicit
// `x = x + n` form. Keeping the volatile only because 5 is the lower banked number.
extern "C" {
extern unsigned int data_020a4be8;
extern unsigned int data_020a4bc8;
extern unsigned int data_020a4be4;
extern unsigned int data_020a4be0;
void Crash();
void _ZN2GX12BeginLoadTexEv();
void _ZN2GX7LoadTexEPKvjj(const void*, unsigned int, unsigned int);
void _ZN2GX10EndLoadTexEv();

#pragma opt_common_subs off
void _ZN5Model27LoadCompressedTextureToVramEPcjS0_(char* src, unsigned int size, char* dst){
  if ((data_020a4be8 - data_020a4bc8) < size) Crash();
  _ZN2GX12BeginLoadTexEv();
  _ZN2GX7LoadTexEPKvjj(src, data_020a4bc8, size);
  data_020a4be4 += size;
  _ZN2GX7LoadTexEPKvjj(dst, data_020a4be0, size >> 1);
  data_020a4be4 += size >> 1;
  _ZN2GX10EndLoadTexEv();
  *(volatile unsigned int*)&data_020a4bc8 += size;
  *(volatile unsigned int*)&data_020a4be0 += size >> 1;
}
}
