//cpp
// @symbol func_ov063_02118ddc
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, const struct Vector3* v, const void* w, int s0, int s1);
extern int func_02012694(int a, char* p);
void func_ov063_02118ddc(char* c){
  struct Vector3 v;
  v.x = *(int*)(c+0x5c);
  int y = *(int*)(c+0x60);
  v.y = y;
  v.z = *(int*)(c+0x64);
  v.y = y + 0x64000;
  int s = *(int*)(c+0x5c);
  if(s < 0) s = -s;
  v.x = s;
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, 3, &v, 0, *(signed char*)(c+0x5d0), -1);
  func_02012694(0xbb, c+0x74);
}
}
