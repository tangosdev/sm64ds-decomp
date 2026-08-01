//cpp
// @symbol func_ov085_0212d5dc
/* recovered: shared common types */
#include "common.h"

struct Range { int a, b, c, d, e, f; };
extern "C" {
void _ZN6Camera9SetFlag_3Ev(void* self);
void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void* dst, const void* src, int step);
void _ZN6Camera9SetLookAtERK7Vector3(void* self, const void* v);
void _ZN6Camera6SetPosERK7Vector3(void* self, const void* v);
int Vec3_Dist(const void* a, const void* b);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, bool e);
void* _ZN5Actor13ClosestPlayerEv(void* self);
void func_ov002_020c3e8c(void);
void func_ov085_0212e728(void* c, void* p);
}
extern void* data_0209f318;
extern int data_0209caa0[];
extern int data_ov085_021307e0;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

extern "C" int func_ov085_0212d5dc(char* c) {
  Range r;
  void* cam = *(void**)&data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  r.a = -0x4b0000;
  r.b = 0x19f000;
  r.c = 0x1a90000;
  r.d = -0x4b0000;
  r.e = 0x250000;
  r.f = 0x1d4c000;
  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c+0x2b0, &r, 0x70000);
  _ZN6Camera9SetLookAtERK7Vector3(cam, c+0x2b0);
  _ZN6Camera6SetPosERK7Vector3(cam, c+0x2bc);
  Vec3_Dist(c+0x2b0, &r);
  (*(int*)AT(c, 0x2c8))++;
  if (*(int*)(c+0x2c8) > 0x64) {
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x7f, 0, 0x7222, false) != 0) {
      *(int*)AT(cam, 0x154) &= ~8;
      *(int*)(c+0x98) = 0;
      *(int*)(c+0x2c8) = 0;
      *(int*)(c+0x2cc) = 0;
      *(int*)(c+0xa4) = 0;
      *(int*)(c+0xa8) = 0;
      *(int*)(c+0xac) = 0;
      *(short*)(c+0x8c) = 0;
      if (_ZN5Actor13ClosestPlayerEv(c) != 0) {
        func_ov002_020c3e8c();
        data_0209caa0[2] |= 0x80;
      }
      func_ov085_0212e728(c, &data_ov085_021307e0);
    }
  }
  return 1;
}
