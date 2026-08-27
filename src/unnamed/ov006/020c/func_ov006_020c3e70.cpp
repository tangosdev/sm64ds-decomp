//cpp
#include "SharedFilePtr.h"
extern "C" {
int SharedFilePtr_Destruct_Anim(int x);
int func_02017ab4(int x);
void _ZN9ModelAnimD1Ev(void* p);
void* __destroy_arr(void* p, int a, int b, void* f);
}
extern void* func_ov006_020c3e54;
extern "C" void* func_ov006_020c3e70(char* c){
  ((SharedFilePtr *)(c + 0xd7c))->Release();
  ((SharedFilePtr *)(c + 0xd84))->Release();
  ((SharedFilePtr *)(c + 0xd8c))->Release();
  ((SharedFilePtr *)(c + 0xd94))->Release();
  ((SharedFilePtr *)(c + 0xd9c))->Release();
  ((SharedFilePtr *)(c + 0xda4))->Release();
  SharedFilePtr_Destruct_Anim((int)(c + 0xda4));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd9c));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd94));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd8c));
  func_02017ab4((int)(c + 0xd84));
  func_02017ab4((int)(c + 0xd7c));
  _ZN9ModelAnimD1Ev(c + 0xd18);
  __destroy_arr(c + 8, 0x16, 0x98, &func_ov006_020c3e54);
  return c;
}
