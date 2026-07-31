//cpp
// @symbol func_ov002_020b41f8
/* recovered: shared common types */
#include "common.h"

extern "C" {
void func_ov002_020b41b8(struct Vector3*, char*);
int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, struct Vector3*, void*, int, int);
void func_ov002_020b41f8(char* c){
  struct Vector3 v;
  func_ov002_020b41b8(&v, c);
  char* r = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11d, 0, &v, 0, *(signed char*)(c+0xcc), -1);
  if(r) *(unsigned char*)(r+0x3c6)=0xb4;
}
}
