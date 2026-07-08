//cpp
extern "C" {
extern int VRAM_Tex_End;
extern int VRAM_Tex4x4_IndexData_Start;
extern int VRAM_Tex4x4_MaxSize;
extern int VRAM_Tex4x4_TexelData_Start;
void Crash();

int _ZN5Model13GetVramOffsetEj(unsigned int j){
  unsigned int* p;
  unsigned int b = VRAM_Tex4x4_MaxSize - VRAM_Tex4x4_TexelData_Start;
  unsigned int a = VRAM_Tex_End - VRAM_Tex4x4_IndexData_Start;
  if ((a << 1) > b && a > j) {
    p = (unsigned int*)&VRAM_Tex_End;
  } else if (b > j) {
    p = (unsigned int*)&VRAM_Tex4x4_MaxSize;
  } else {
    Crash();
  }
  *p -= j;
  return *p;
}
}
