#include "types.h"
struct Vec3
{
  s32 x;
  s32 y;
  s32 z;
};
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int *Vec3_AsrInPlace(int *v, int sh);
extern s32 Vec3_Dist(const struct Vec3 *a, const struct Vec3 *b);
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern s16 Vec3_HorzAngle(const struct Vec3 *v0, const struct Vec3 *v1);
extern s16 Vec3_VertAngle(const struct Vec3 *v1, const struct Vec3 *v0);
extern int func_020091f8(void *a, void *b, int c, int d);
extern u32 func_020093f4(void *p, int x);
extern int func_02009138(int *thiz, int arg);
extern void func_020089d8(void *p);
int func_02005110(char *self)
{
  struct Vec3 eye;
  struct Vec3 aim;
  struct Vec3 *target;
  s32 horzAngle;
  s32 mode;
  {
    struct Vec3 *p = (struct Vec3 *) (((long long) ((int) ((*((char **) (self + 0x110))) + 0x5c))));
    eye.x = p->x;
    eye.y = p->y;
    eye.z = p->z;
    eye.y += 0x64000;
  }
  mode = 0x48000;
  target = *((struct Vec3 **) (self + 0x11c));
  if (target == 0)
  {
    aim.x = eye.x;
    aim.y = eye.y;
    aim.z = eye.z;
    if ((*((u8 *) ((*((char **) (self + 0x110))) + 0x724))) != 0)
    {
      mode = 0xd0000;
    }
    horzAngle = *((s16 *) ((*((char **) (self + 0x110))) + 0x8e));
  }
  else
  {
    struct Vec3 diff;
    struct Vec3 sum;
    struct Vec3 newAim;
    s32 dist;
    s32 scale;
    s16 fall;
    Vec3_Add(&sum, target, &eye);
    aim.x = sum.x;
    aim.y = sum.y;
    aim.z = sum.z;
    Vec3_AsrInPlace((int *) (&aim), 1);
    dist = Vec3_Dist(&eye, &aim);
    if (dist > 0x30000)
    {
      Vec3_Sub(&diff, &aim, &eye);
      scale = _ZN4cstd4fdivEii(0x30000, dist);
      Vec3_MulScalarInPlace((int *) (&diff), scale);
      Vec3_Add(&newAim, &eye, &diff);
      aim.x = newAim.x;
      aim.y = newAim.y;
      aim.z = newAim.z;
    }
    fall = 0x2000;
    {
      struct Vec3 *rawpos = (struct Vec3 *) (((long long) ((int) ((*((char **) (self + 0x110))) + 0x5c))));
      s32 fallDist = Vec3_Dist(*((struct Vec3 **) (self + 0x11c)), rawpos);
      s32 t = fallDist - 0x200000;
      if (t > 0)
      {
        s16 scaled = (s16) (((((s64) t) << 2) + 0x800) >> 12);
        if (scaled > fall)
        {
          scaled = fall;
        }
        fall -= scaled;
      }
    }
    s16 diffAngle = Vec3_HorzAngle(*((struct Vec3 **) (self + 0x11c)), &eye) - (*((s16 *) (self + 0x17c)));
    fall = (diffAngle >= 0) ? (-fall) : (fall);
    if ((!newAim.x) && (!newAim.x))
    {
    }
    horzAngle = (s16) (Vec3_HorzAngle(*((struct Vec3 **) (self + 0x11c)), &eye) + fall);
  }
  {
    s32 vert = Vec3_VertAngle(&aim, &eye);
    if (vert < (-0x2800))
    {
      vert = -0x2800;
    }
    else
      if (vert > 0x2800)
    {
      vert = 0x2800;
    }
    func_020091f8(self, &aim, horzAngle, vert);
  }
  {
    u32 r = func_020093f4((void *) self, mode);
    if (func_02009138((int *) self, r) == 0)
    {
      func_020089d8((void *) self);
    }
  }
  return 1;
}
