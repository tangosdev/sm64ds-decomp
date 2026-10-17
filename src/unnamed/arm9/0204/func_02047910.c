#include "nitro/fx/fx.h"
struct Vec3
{
  int x;
  int y;
  int z;
};

extern int LenVec3(struct Vec3 *v);
extern void NormalizeVec3(struct Vec3 *dst, struct Vec3 *src);

void func_02047910(int *m0, int *m1, int *dst, int t)
{
  struct Vec3 va;
  struct Vec3 vb;
  struct Vec3 vx;
  struct Vec3 vy;
  struct Vec3 vz;
  int la;
  int lb;
  int len;

  va.x = m0[3];
  va.y = m0[4];
  va.z = m0[5];
  vb.x = m1[3];
  vb.y = m1[4];
  vb.z = m1[5];
  la = LenVec3(&va);
  lb = LenVec3(&vb);
  len = la + FX_MUL(t, (lb - la));
  vy.x = va.x + FX_MUL(t, (vb.x - va.x));
  vy.y = va.y + FX_MUL(t, (vb.y - va.y));
  vy.z = va.z + FX_MUL(t, (vb.z - va.z));
  if (FX_MUL(vy.x, vy.x) + FX_MUL(vy.y, vy.y) + FX_MUL(vy.z, vy.z) == 0)
      return;
  NormalizeVec3(&vy, &vy);
  dst[3] = FX_MUL(vy.x, len);
  dst[4] = FX_MUL(vy.y, len);
  dst[5] = FX_MUL(vy.z, len);

  va.x = m0[0];
  va.y = m0[1];
  va.z = m0[2];
  vb.x = m1[0];
  vb.y = m1[1];
  vb.z = m1[2];
  la = LenVec3(&va);
  lb = LenVec3(&vb);
  len = la + FX_MUL(t, (lb - la));
  vx.x = va.x + FX_MUL(t, (vb.x - va.x));
  vx.y = va.y + FX_MUL(t, (vb.y - va.y));
  vx.z = va.z + FX_MUL(t, (vb.z - va.z));
  if (FX_MUL(vx.x, vx.x) + FX_MUL(vx.y, vx.y) + FX_MUL(vx.z, vx.z) == 0)
      return;
  NormalizeVec3(&vx, &vx);
  dst[0] = FX_MUL(vx.x, len);
  dst[1] = FX_MUL(vx.y, len);
  dst[2] = FX_MUL(vx.z, len);

  va.x = m0[6];
  va.y = m0[7];
  va.z = m0[8];
  vb.x = m1[6];
  vb.y = m1[7];
  vb.z = m1[8];
  la = LenVec3(&va);
  lb = LenVec3(&vb);
  len = la + FX_MUL(t, (lb - la));
  vz.x = va.x + FX_MUL(t, (vb.x - va.x));
  vz.y = va.y + FX_MUL(t, (vb.y - va.y));
  vz.z = va.z + FX_MUL(t, (vb.z - va.z));
  if (FX_MUL(vz.x, vz.x) + FX_MUL(vz.y, vz.y) + FX_MUL(vz.z, vz.z) == 0)
      return;

  vx.x = FX_MUL(vy.y, (vz.z - vy.z)) - FX_MUL(vy.z, (vz.y - vy.y));
  vx.y = FX_MUL(vy.z, (vz.x - vy.x)) - FX_MUL(vy.x, (vz.z - vy.z));
  vx.z = FX_MUL(vy.x, (vz.y - vy.y)) - FX_MUL(vy.y, (vz.x - vy.x));
  if (FX_MUL(vx.x, vx.x) + FX_MUL(vx.y, vx.y) + FX_MUL(vx.z, vx.z) == 0)
      return;
  vz.x = FX_MUL(vx.y, (vy.z - vx.z)) - FX_MUL(vx.z, (vy.y - vx.y));
  vz.y = FX_MUL(vx.z, (vy.x - vx.x)) - FX_MUL(vx.x, (vy.z - vx.z));
  vz.z = FX_MUL(vx.x, (vy.y - vx.y)) - FX_MUL(vx.y, (vy.x - vx.x));
  if (FX_MUL(vz.x, vz.x) + FX_MUL(vz.y, vz.y) + FX_MUL(vz.z, vz.z) == 0)
      return;

  NormalizeVec3(&vz, &vz);
  dst[6] = FX_MUL(vz.x, len);
  dst[7] = FX_MUL(vz.y, len);
  dst[8] = FX_MUL(vz.z, len);

  dst[9] = m0[9] + FX_MUL(t, (m1[9] - m0[9]));
  dst[10] = m0[10] + FX_MUL(t, (m1[10] - m0[10]));
  dst[11] = m0[11] + FX_MUL(t, (m1[11] - m0[11]));
}
