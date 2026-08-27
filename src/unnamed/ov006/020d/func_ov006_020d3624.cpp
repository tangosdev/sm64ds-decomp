//cpp
extern "C" {
char* func_02054efc(void);
void MultiCopyHalf(void*, void*, int);
}
namespace G2S { char* GetBG0CharPtr(void); }
extern "C" void func_ov006_020d3624(void){
  char* a = func_02054efc();
  MultiCopyHalf(func_02054efc()+0xc000, a, 0x6000);
  char* b = G2S::GetBG0CharPtr();
  MultiCopyHalf(G2S::GetBG0CharPtr()+0x6000, b, 0x6000);
}
