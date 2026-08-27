// @symbol func_ov002_020ed6cc
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

extern Fix12i Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
int func_ov002_020ed6cc(void* c) {
  Fix12i d = Vec3_HorzDist((struct Vector3*)((char*)c+0x3cc), (struct Vector3*)((char*)c+0x5c));
  *(int*)((char*)c+0x3cc) = *(int*)((char*)c+0x5c);
  *(int*)((char*)c+0x3d0) = *(int*)((char*)c+0x60);
  *(int*)((char*)c+0x3d4) = *(int*)((char*)c+0x64);
  if (d >= 0x32000) goto fail;
  if (DecIfAbove0_Byte((unsigned char*)((char*)c+0x41f))) goto ret0;
  return 1;
fail:
  *(unsigned char*)((char*)c+0x41f) = 0xf;
ret0:
  return 0;
}
