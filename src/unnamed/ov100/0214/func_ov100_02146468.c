// @symbol func_ov100_02146468
/* recovered: shared common types */
#include "common.h"
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern int Vec3_HorzLen(struct Vector3* v);

void func_ov100_02146468(char* r4){
  struct Vector3 v;
  char* other = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(r4 + 0x13c));
  Vec3_Sub(&v, (struct Vector3*)(r4 + 0x5c), (struct Vector3*)(other + 0x5c));
  if (DecIfAbove0_Byte((unsigned char*)(r4 + 0x15a)) != 0) {
    if (Vec3_HorzLen(&v) < 0xfa000) return;
  }
  *(short*)(r4 + 0x154) = 0x800;
  *(short*)(r4 + 0x156) = *(short*)(r4 + 0x94) + 0x8000;
  *(int*)(r4 + 0x130) = 0x2000;
  *(int*)(r4 + 0x14c) = 6;
}
