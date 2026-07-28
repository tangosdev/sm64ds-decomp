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
  len = la + (int) ((((long long) t) * (lb - la) + 0x800) >> 12);
  vy.x = va.x + (int) ((((long long) t) * (vb.x - va.x) + 0x800) >> 12);
  vy.y = va.y + (int) ((((long long) t) * (vb.y - va.y) + 0x800) >> 12);
  vy.z = va.z + (int) ((((long long) t) * (vb.z - va.z) + 0x800) >> 12);
  if ((int) ((((long long) vy.x) * vy.x + 0x800) >> 12)
      + (int) ((((long long) vy.y) * vy.y + 0x800) >> 12)
      + (int) ((((long long) vy.z) * vy.z + 0x800) >> 12) == 0)
    return;
  NormalizeVec3(&vy, &vy);
  dst[3] = (int) ((((long long) vy.x) * len + 0x800) >> 12);
  dst[4] = (int) ((((long long) vy.y) * len + 0x800) >> 12);
  dst[5] = (int) ((((long long) vy.z) * len + 0x800) >> 12);

  va.x = m0[0];
  va.y = m0[1];
  va.z = m0[2];
  vb.x = m1[0];
  vb.y = m1[1];
  vb.z = m1[2];
  la = LenVec3(&va);
  lb = LenVec3(&vb);
  len = la + (int) ((((long long) t) * (lb - la) + 0x800) >> 12);
  vx.x = va.x + (int) ((((long long) t) * (vb.x - va.x) + 0x800) >> 12);
  vx.y = va.y + (int) ((((long long) t) * (vb.y - va.y) + 0x800) >> 12);
  vx.z = va.z + (int) ((((long long) t) * (vb.z - va.z) + 0x800) >> 12);
  if ((int) ((((long long) vx.x) * vx.x + 0x800) >> 12)
      + (int) ((((long long) vx.y) * vx.y + 0x800) >> 12)
      + (int) ((((long long) vx.z) * vx.z + 0x800) >> 12) == 0)
    return;
  NormalizeVec3(&vx, &vx);
  dst[0] = (int) ((((long long) vx.x) * len + 0x800) >> 12);
  dst[1] = (int) ((((long long) vx.y) * len + 0x800) >> 12);
  dst[2] = (int) ((((long long) vx.z) * len + 0x800) >> 12);

  va.x = m0[6];
  va.y = m0[7];
  va.z = m0[8];
  vb.x = m1[6];
  vb.y = m1[7];
  vb.z = m1[8];
  la = LenVec3(&va);
  lb = LenVec3(&vb);
  len = la + (int) ((((long long) t) * (lb - la) + 0x800) >> 12);
  vz.x = va.x + (int) ((((long long) t) * (vb.x - va.x) + 0x800) >> 12);
  vz.y = va.y + (int) ((((long long) t) * (vb.y - va.y) + 0x800) >> 12);
  vz.z = va.z + (int) ((((long long) t) * (vb.z - va.z) + 0x800) >> 12);
  if ((int) ((((long long) vz.x) * vz.x + 0x800) >> 12)
      + (int) ((((long long) vz.y) * vz.y + 0x800) >> 12)
      + (int) ((((long long) vz.z) * vz.z + 0x800) >> 12) == 0)
    return;

  vx.x = (int) ((((long long) vy.y) * (vz.z - vy.z) + 0x800) >> 12)
       - (int) ((((long long) vy.z) * (vz.y - vy.y) + 0x800) >> 12);
  vx.y = (int) ((((long long) vy.z) * (vz.x - vy.x) + 0x800) >> 12)
       - (int) ((((long long) vy.x) * (vz.z - vy.z) + 0x800) >> 12);
  vx.z = (int) ((((long long) vy.x) * (vz.y - vy.y) + 0x800) >> 12)
       - (int) ((((long long) vy.y) * (vz.x - vy.x) + 0x800) >> 12);
  if ((int) ((((long long) vx.x) * vx.x + 0x800) >> 12)
      + (int) ((((long long) vx.y) * vx.y + 0x800) >> 12)
      + (int) ((((long long) vx.z) * vx.z + 0x800) >> 12) == 0)
    return;
  vz.x = (int) ((((long long) vx.y) * (vy.z - vx.z) + 0x800) >> 12)
       - (int) ((((long long) vx.z) * (vy.y - vx.y) + 0x800) >> 12);
  vz.y = (int) ((((long long) vx.z) * (vy.x - vx.x) + 0x800) >> 12)
       - (int) ((((long long) vx.x) * (vy.z - vx.z) + 0x800) >> 12);
  vz.z = (int) ((((long long) vx.x) * (vy.y - vx.y) + 0x800) >> 12)
       - (int) ((((long long) vx.y) * (vy.x - vx.x) + 0x800) >> 12);
  if ((int) ((((long long) vz.x) * vz.x + 0x800) >> 12)
      + (int) ((((long long) vz.y) * vz.y + 0x800) >> 12)
      + (int) ((((long long) vz.z) * vz.z + 0x800) >> 12) == 0)
    return;

  NormalizeVec3(&vz, &vz);
  dst[6] = (int) ((((long long) vz.x) * len + 0x800) >> 12);
  dst[7] = (int) ((((long long) vz.y) * len + 0x800) >> 12);
  dst[8] = (int) ((((long long) vz.z) * len + 0x800) >> 12);

  dst[9] = m0[9] + (int) ((((long long) t) * (m1[9] - m0[9]) + 0x800) >> 12);
  dst[10] = m0[10] + (int) ((((long long) t) * (m1[10] - m0[10]) + 0x800) >> 12);
  dst[11] = m0[11] + (int) ((((long long) t) * (m1[11] - m0[11]) + 0x800) >> 12);
}
