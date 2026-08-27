//cpp
extern "C" {
char* func_02054efc(void);
void MultiCopyHalf(void*, void*, int);
}
namespace G2S { char* GetBG0CharPtr(void); }
extern "C" void func_ov006_020d3668(void){
  char* a = func_02054efc()+0xc000;
  MultiCopyHalf(func_02054efc(), a, 0x6000);
  char* b = G2S::GetBG0CharPtr()+0x6000;
  MultiCopyHalf(G2S::GetBG0CharPtr(), b, 0x6000);
}
