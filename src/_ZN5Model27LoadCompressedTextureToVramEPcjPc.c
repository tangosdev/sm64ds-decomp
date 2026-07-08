//cpp
// NONMATCHING: different op / idiom (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern unsigned int VRAM_Tex4x4_MaxSize;
extern unsigned int VRAM_Tex4x4_TexelData_Start;
extern unsigned int VRAM_Tex_Size;
extern unsigned int VRAM_Tex4x4_IndexData_Start;
void Crash();
void _ZN2GX12BeginLoadTexEv();
void _ZN2GX7LoadTexEPKvjj(const void*, unsigned int, unsigned int);
void _ZN2GX10EndLoadTexEv();

void _ZN5Model27LoadCompressedTextureToVramEPcjPc(char* src, unsigned int size, char* dst){
  if ((VRAM_Tex4x4_MaxSize - VRAM_Tex4x4_TexelData_Start) < size) Crash();
  _ZN2GX12BeginLoadTexEv();
  _ZN2GX7LoadTexEPKvjj(src, VRAM_Tex4x4_TexelData_Start, size);
  VRAM_Tex_Size += size;
  _ZN2GX7LoadTexEPKvjj(dst, VRAM_Tex4x4_IndexData_Start, size >> 1);
  VRAM_Tex_Size += size >> 1;
  _ZN2GX10EndLoadTexEv();
  VRAM_Tex4x4_TexelData_Start += size;
  VRAM_Tex4x4_IndexData_Start += size >> 1;
}
}
