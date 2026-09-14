// @symbol func_ov006_020cf2fc
/* recovered: shared minigame 3D code (the block before dScMgAmida_c): builds an object's placement matrix from translation and scale. */
// NONMATCHING: div 164 of 279 words. mwccarm 2004/b56, --module ov006,
// @ 0x020cf2fc size 0x45c. Residue class: pure 6bs register recycle (CRK2-J: needs the ROM's own compiler build).
// Draft from nearmiss/db.jsonl (stored divergence 164), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
typedef volatile unsigned int vu32;
typedef struct
{
  int x;
  int y;
  int z;
} Vec3;
struct Matrix4x3;
struct Mtx43;
extern struct Matrix4x3 data_020a0e68;
extern struct Matrix4x3 data_0209b3ec;
extern unsigned short data_ov006_0212e060[];
extern unsigned short data_ov006_0212e068[];
extern int data_ov006_0212e0b0[];
extern void *data_ov006_02140844;
extern void *data_ov006_02140814;
extern void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
extern void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3 *m, int x, int y, int z);
extern void func_020553a4(int *m);
void func_ov006_020cf2fc(char *obj)
{
  int i;
  int j;
  int prim;
  int k0;
  int prim2;
  int zero2;
  int m2[12];
  int m1[12];
  Matrix4x3_FromTranslation((struct Mtx43 *) &data_020a0e68, *((int *) (obj + 8)), *((int *) (obj + 0xc)), *((int *) (obj + 0x10)));
  MulMat4x3Mat4x3((const int *) &data_020a0e68, (const int *) &data_0209b3ec, m1);
  Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, *((int *) (obj + 0x2c)), *((int *) (obj + 0x30)), *((int *) (obj + 0x34)));
  MulMat4x3Mat4x3((const int *) &data_020a0e68, (const int *) &data_0209b3ec, m2);
  *((vu32 *) 0x4000440) = 2;
  func_020553a4(m1);
  *((vu32 *) 0x4000440) = 1;
  func_020553a4(m2);
  *((vu32 *) 0x400046c) = 0x100000;
  *((vu32 *) 0x400046c) = 0x100000;
  *((vu32 *) 0x400046c) = 0x100000;
  {
    int z = 0;
    *((vu32 *) 0x4000440) = 3;
    *((vu32 *) 0x4000454) = z;
    i = z;
  }
  *((vu32 *) 0x40004a8) = 0x8da70000 | (((unsigned int) data_ov006_02140844) >> 3);
  *((vu32 *) 0x40004ac) = ((unsigned int) data_ov006_02140814) >> 4;
  {
    short *p31e = (short *) (obj + 0x31e);
    int sh = *p31e;
    unsigned short *t0 = data_ov006_0212e060;
    volatile unsigned char *ppl = (volatile unsigned char *) (obj + 0x329);
    int pl = *ppl;
    *((vu32 *) 0x40004a4) = (((sh + 1) << 24) | 0x82) | (pl << 16);
    {
      unsigned short *p326 = (unsigned short *) (obj + 0x326);
      unsigned idx = *p326;
      *((vu32 *) 0x40004c0) = t0[idx] | (data_ov006_0212e068[idx] << 16);
    }
  }
  *((vu32 *) 0x40004c4) = 0x8000;
  #pragma opt_strength_reduction off
  {
    int i4;
    j = i;
    k0 = i;
    prim = 2;
    i4 = i;
    for (; i < 3; i++)
    {
      Vec3 *va;
      Vec3 *vb;
      int *na;
      int *nb;
      int n;
      int k;
      n = (i + 1) * 4;
      *((vu32 *) 0x4000500) = prim;
      va = ((Vec3 *) (obj + 0x5c)) + i4;
      vb = ((Vec3 *) (obj + 0x5c)) + n;
      na = ((int *) (obj + 0x2dc)) + i4;
      nb = ((int *) (obj + 0x2dc)) + n;
      for (k = k0; k < 4; k++)
      {
        *((vu32 *) 0x4000488) = data_ov006_0212e0b0[i4 + k];
        *((vu32 *) 0x4000484) = (0 - (*(na++))) & ((long long) (~0xC0000000));
        {
          int x = va->x;
          int y = va->y;
          int z = va->z;
          int sx = (x << 8) >> 16;
          int sy = (y << 8) >> 16;
          int sz = (z << 8) >> 16;
          *((vu32 *) 0x400048c) = ((unsigned short) ((x) ? (sx) : (sx))) | (((unsigned int) ((unsigned short) sy)) << 16);
          *((vu32 *) 0x400048c) = (unsigned short) sz;
        }
        va++;
        *((vu32 *) 0x4000488) = data_ov006_0212e0b0[n];
        *((vu32 *) 0x4000484) = (0 - (*(nb++))) & (~0xC0000000);
        n++;
        {
          int x = vb->x;
          int y = vb->y;
          int z = vb->z;
          int sx = (x << 8) >> 16;
          int sy = (y << 8) >> 16;
          int sz = (z << 8) >> 16;
          *((vu32 *) 0x400048c) = ((unsigned short) ((x) ? (sx) : (sx))) | (((unsigned int) ((unsigned short) sy)) << 16);
          *((vu32 *) 0x400048c) = (unsigned short) sz;
        }
        vb++;
      }
      i4 += 4;
      *((vu32 *) 0x4000504) = j;
    }
    zero2 = 0;
    prim2 = 2;
    {
      int j4 = 0;
      for (; j < 3; j++)
      {
        Vec3 *va;
        Vec3 *vb;
        int *na;
        int *nb;
        int n;
        int k;
        n = (j + 1) * 4;
        *((vu32 *) 0x4000500) = prim2;
        va = ((Vec3 *) (obj + 0x5c)) + j4;
        vb = ((Vec3 *) (obj + 0x5c)) + n;
        na = ((int *) (obj + 0x2dc)) + j4;
        nb = ((int *) (obj + 0x2dc)) + n;
        for (k = zero2; k < 4; k++)
        {
          *((vu32 *) 0x4000488) = data_ov006_0212e0b0[n];
          *((vu32 *) 0x4000484) = *(nb++);
          n++;
          *((vu32 *) 0x400048c) = ((unsigned short) ((vb->x << 8) >> 16)) | (((unsigned int) ((unsigned short) ((vb->y << 8) >> 16))) << 16);
          *((vu32 *) 0x400048c) = (unsigned short) ((vb->z << 8) >> 16);
          vb++;
          *((vu32 *) 0x4000488) = data_ov006_0212e0b0[j4 + k];
          *((vu32 *) 0x4000484) = *(na++);
          *((vu32 *) 0x400048c) = ((unsigned short) ((va->x << 8) >> 16)) | (((unsigned int) ((unsigned short) ((va->y << 8) >> 16))) << 16);
          *((vu32 *) 0x400048c) = (unsigned short) ((va->z << 8) >> 16);
          va++;
        }
        j4 += 4;
        *((vu32 *) 0x4000504) = zero2;
      }
    }
  }
}
