//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int data_ov002_0210bae4[];
extern int _ZN7Vector3D1Ev[];
void _ZN5ModelD2Ev(void*);
void __destroy_arr(void*, int, int, void*);
void* _ZN8daDemo_c13simpleModel_cD1Ev(char* c){
  void* p;
  *(int*)c = (int)data_ov002_0210bae4;
  p = *(void**)(c+0x5c);
  if(p!=0){
    ((SharedFilePtr *)(p))->Release();
  }
  _ZN5ModelD2Ev(c);
  __destroy_arr(c+0x50, 1, 0xc, (void*)_ZN7Vector3D1Ev);
  return c;
}
}
