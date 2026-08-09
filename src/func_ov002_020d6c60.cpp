//cpp
// @symbol func_ov002_020d6c60
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* pos, void* rot, int e, int f);
extern "C" void func_ov002_020d7030(void* c, void* d);
extern "C" void _ZN6Player18SetNewHatCharacterEjjb(void* self, unsigned int j1, unsigned int j2, int b);
extern "C" void Player_DisableInteraction(void* c);
extern "C" void func_ov002_020d71ec(void* c, int x);



extern "C" int func_ov002_020d6c60(char* p0, char* p1){
  Vector3 v;
  int b;
  int y;
  int* q;
  void* sp;
  if(func_ov002_020d82f0(p1) == 0) goto ret0a;
  b = (*(int*)(p1+0x360) != 0);
  if(b == 0) goto l48;
ret0a:
  return 0;
l48:
  b = (*(unsigned char*)(p1+0x6d9) != *(int*)(p1+8));
  if(b == 0) goto leq;
  v.x = *(int*)(p0+0x5c);
  y = *(int*)(p0+0x60);
  v.y = y;
  v.z = *(int*)(p0+0x64);
  v.y = y + 0x64000;
  sp = (void*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (*(int*)(p1+8)<<8)|5, &v, p0+0x8c, *(signed char*)(p0+0xcc), -1);
  if(sp == 0) goto ret0b;
  func_ov002_020d7030(p0, sp);
  _ZN6Player18SetNewHatCharacterEjjb(p1, *(unsigned char*)(p0+0x6d9), 0, 0);
  Player_DisableInteraction(p0);
  return 1;
leq:
  if(func_ov002_020d5f34(p1, p0) == 0) goto ret0b;
  *(char**)(p0+0x360) = p1;
  q = (int*)(((int)p1 + 0xb0) & 0xFFFFFFFFFFFFFFFFull);
  *q = *q | 0x20000;
  func_ov002_020d71ec(p0, 1);
  Player_DisableInteraction(p0);
  return 1;
ret0b:
  return 0;
}
