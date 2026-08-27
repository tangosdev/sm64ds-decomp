//cpp
// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// Model::LoadCompressedTextureToVram @ 0x02045b58 (arm9, size 0xb8). 5 words diverge:
// the allocator build delta from PR #957 -- ROM gives sumA a fresh register and colors
// sumB onto the just-freed one; every owned build reuses the dying operand register.
// 17-probe pass, flags, versions, value/address launders all closed (notes 6ay, DB row).
// Register-choice-only delta: functionally identical. Supersedes the older 13-div draft.
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
