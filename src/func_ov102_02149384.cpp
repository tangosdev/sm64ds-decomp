//cpp
// @symbol func_ov102_02149384
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void func_ov102_02149684(void*,void*);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int,unsigned int,Vector3*,void*,int,int);
void* func_ov102_02149384(void* c){
  Vector3 v;
  func_ov102_02149684(&v, c);
  void* a=_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11d,0,&v,0,*(signed char*)((char*)c+0xcc),-1);
  if(a) *(unsigned char*)((char*)a+0x3c6)=0xb4;
  return a;
}
}
