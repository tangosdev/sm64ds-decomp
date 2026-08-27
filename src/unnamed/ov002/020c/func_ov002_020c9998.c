// @symbol func_ov002_020c9998
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;


extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, const struct Vector3*, const struct Vector3_16*, int, int);
void func_ov002_020c9998(void* c) {
  *(unsigned char*)((char*)c+0x6e3) = 0x12;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x92, 0x40000000, 0x1000, 0);
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
      0xb2, 0xffff,
      (struct Vector3*)((char*)c+0x5c),
      (struct Vector3_16*)((char*)c+0x8c),
      *(signed char*)((char*)c+0xcc), -1);
  *(int*)((char*)c+0x98) = 0;
  *(int*)((char*)c+0xa8) = 0;
}
