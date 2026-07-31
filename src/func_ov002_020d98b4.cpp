//cpp
// @symbol func_ov002_020d98b4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern unsigned int _ZNK6Player14GetBodyModelIDEjb(char* c, unsigned int a, int b);
extern void MulVec3Mat4x3(void* a, void* b, struct Vector3* out);
extern void Vec3_MulScalar(struct Vector3* out, struct Vector3* in, int s);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);

void func_ov002_020d98b4(char* self){
  int t = func_ov002_020beb38(self);
  *(unsigned short*)(((int)self + 0x6a4)) -= t;
  {
    short* q = (short*)(self + 0x600);
    if (q[0x52] < 0) q[0x52] = 0;
  }
  {
    unsigned int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
    int r5 = *(int*)(*(int*)(self + (id << 2) + 0xdc) + 0x14) + 0x2d0;
    unsigned int id2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
    struct Vector3 v;
    struct Vector3 s;
    MulVec3Mat4x3((void*)(r5 + 0x24), (void*)(*(int*)(self + (id2 << 2) + 0xdc) + 0x1c), &v);
    Vec3_MulScalar(&s, &v, 0x8000);
    s.y += 0x50000;
    *(void**)(self+0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
      *(unsigned int*)(self+0x628), 0xd6, s.x, s.y, s.z, 0, 0);
  }
  *(int*)(((int)self + 0xb0)) |= 0x80;
}
}
