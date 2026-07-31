//cpp
// @symbol func_ov006_020ce8a0
/* recovered: shared common types */
#include "common.h"

extern "C" void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern "C" int DotVec3(struct Vector3* a, struct Vector3* b);

struct Obj {
  virtual struct Vector3* GetA();
  virtual struct Vector3* GetB();
};

extern "C" void func_ov006_020ce8a0(char* self, struct Obj* o, int* outR5, int* outR4){
  struct Vector3 a, b, sa, sb;
  struct Vector3* p;
  int t;
  p=o->GetA();
  a.x=p->x; a.y=p->y; a.z=p->z;
  p=o->GetB();
  b.x=p->x; b.y=p->y; b.z=p->z;
  Vec3_Sub(&sa,&a,(struct Vector3*)(self+8));
  Vec3_Sub(&sb,&b,(struct Vector3*)(self+0x14));
  sa.y += 0xc000;
  t=DotVec3((struct Vector3*)(self+0x38),&sa);
  outR5[0]=DotVec3((struct Vector3*)(self+0x44),&sa);
  outR5[1]=t;
  t=DotVec3((struct Vector3*)(self+0x50),&sb);
  outR4[0]=DotVec3((struct Vector3*)(self+0x5c),&sb);
  outR4[1]=t;
}
