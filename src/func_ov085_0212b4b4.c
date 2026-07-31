// @symbol func_ov085_0212b4b4
// @emits PrincessPeach_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daPeach_c::Kill - recovered from vtable slot identity */

typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void func_ov085_0212bc78(void *c, void *p);
extern void func_02012694(int a, void *b);
extern void _ZN7PathPtrC1Ev(void *);
extern void _ZN7PathPtr6FromIDEj(void *, u32);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *, Vector3 *, u32);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int ApproachAngle(s16 *p, s16 target, int a, int b, int c);
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 LenVec3(const Vector3 *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(Vector3 *out, const Vector3 *v, int s);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *);
int PrincessPeach_Kill(char *c)
{
  void *pl;
  char pathptr[8];
  Vector3 v;
  Vector3 pos;
  Vector3 node;
  Vector3 node2;
  Vector3 delta;
  Vector3 scaled;
  s32 len;
  s32 lim;
  s16 ang;
  int idx;
  int ysave;
  pl = _ZN5Actor13ClosestPlayerEv(c);
  if (pl != 0)
  {
    v = *((Vector3 *) (((char *) pl) + 0x5c));
    if (Vec3_Dist((Vector3 *) (c + 0x5c), &v) > 0x4b0000)
    {
      func_ov085_0212bc78(c, data_ov085_0213069c);
      return 1;
    }
  }
  {
    s32 t = (*((s32 *) (c + 0x358))) >> 12;
    if (((u16) t) == 0)
    {
      if (func_ov085_0212a788(c) == 1)
      {
        func_02012694(0x124, c + 0x74);
        func_02022a4c(*((s32 *) (c + 0x5c)), (*((s32 *) (c + 0x464))) + 0x3000, *((s32 *) (c + 0x64)));
      }
      else
      {
        func_02012694(0x123, c + 0x74);
      }
    }
  }

  if (func_ov085_0212a788(c) == 1)
  {
    s32 pair[2];
    pair[0] = *((s32 *) (c + 0x5c));
    pair[1] = *((s32 *) (c + 0x464));
    {
      s32 z = *((s32 *) (c + 0x64));
      s32 y = pair[1] + 0x3000;
      s32 x = pair[0];
      *((volatile s32 *) (&pos.x)) = x;
      *((volatile s32 *) (&pos.y)) = y;
      *((volatile s32 *) (&pos.z)) = z;
      *((s32 *) (c + 0x46c)) = func_02022cbc(*((s32 *) (c + 0x46c)), 0xe8, *((volatile s32 *) (&pos.x)), *(&pos.y), z, 0);
    }
  }

  _ZN7PathPtrC1Ev(pathptr);
  _ZN7PathPtr6FromIDEj(pathptr, *((u32 *) (c + 0x438)));
  _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &node, *((u32 *) (c + 0x448)));
  ang = Vec3_HorzAngle((Vector3 *) (c + 0x5c), &node);
  ApproachAngle((s16 *) (c + 0x94), ang, 1, 0x1000, 0x1000);
  idx = (*((s32 *) (c + 0x448))) + (*((s32 *) (c + 0x44c)));
  if (idx < 0)
  {
    idx = (*((s32 *) (c + 0x444))) - 1;
  }
  if (idx >= (*((s32 *) (c + 0x444))))
  {
    idx = 0;
  }
  _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &node2, (u32) idx);
  lim = 0x26000;
  if ((*((s32 *) (c + 0x43c))) == 7)
  {
    lim = lim >> 1;
  }
  {
    s32 y = *((s32 *) (c + 0x60));
    node.y = y;
    Vec3_Sub(&delta, (Vector3 *) (c + 0x5c), &node);
    len = LenVec3(&delta);
  }
  if ((len == 0) || (len <= lim))
  {
    *((s32 *) (c + 0x5c)) = node.x;
    *((s32 *) (c + 0x60)) = node.y;
    *((s32 *) (c + 0x64)) = node.z;
    {
      s32 *p = (s32 *) ((int) (((long long) ((int) (c + 0x448)))));
      *p = (*p) + (*((s32 *) (c + 0x44c)));
    }
    if ((*((s32 *) (c + 0x448))) >= (*((s32 *) (c + 0x444))))
    {
      *((s32 *) (c + 0x448)) = 0;
    }
    if ((*((s32 *) (c + 0x448))) < 0)
    {
      *((s32 *) (c + 0x448)) = (*((s32 *) (c + 0x444))) - 1;
    }
  }
  else
  {
    ang = Vec3_HorzAngle((Vector3 *) (c + 0x5c), &node);
    if (AngleDiff(ang, *((s16 *) (c + 0x94))) < 0x2000)
    {
      ysave = *((s32 *) (c + 0x60));
      {
        int s = _ZN4cstd4fdivEii(lim, len);
        Vec3_MulScalar(&scaled, &delta, s);
        SubVec3((Vector3 *) (c + 0x5c), &scaled, (Vector3 *) (c + 0x5c));
      }
      if ((*((s32 *) (c + 0xa8))) > 0)
      {
        *((s32 *) (c + 0x60)) = ysave;
      }
      if ((*((s32 *) (c + 0x41c))) == 0)
      {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0)
        {
          *((s32 *) (c + 0xa8)) = 0xa000;
          *((s32 *) (c + 0x41c)) = 1;
        }
      }
    }
  }
  return 1;
}
