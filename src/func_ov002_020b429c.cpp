//cpp
// @symbol func_ov002_020b429c
/* recovered: shared common types */
#include "common.h"

extern "C" {
void func_ov002_020b41b8(struct Vector3*, char*);
int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, struct Vector3*, void*, int, int);
void func_ov002_020b429c(char* c){
  struct Vector3 v;
  func_ov002_020b41b8(&v, c);
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x114, 0, &v, 0, *(signed char*)(c+0xcc), -1);
}
}
