typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
extern void func_ov007_020c1448(void *tex, int texGen, int repeat, int flip, int color0);
extern void func_ov007_020c1404(void *tex);
extern void func_ov007_020c49bc(int a0, int a1, int a2, int a3, int a4, int a5, u16 *a6, int a7);
extern int data_ov007_0210325c;
void func_ov007_020c4684(u8 *self, int color, int p3, int p4, int flags, s16 *vec)
{
  u8 *s;
  s16 sx;
  s16 *v;
  int new_var2;
  int tw;
  u16 *tilemap;
  s16 sy;
  u16 us;
  u16 ut;
  int xl;
  int th;
  volatile int *new_var;
  int yl;
  int *tex;
  int orig_tw;
  int w;
  int h;
  s16 vz;
  s16 vy;
  int scale;
  s64 mul_w;
  s64 mul_h;
  int use_col;
  u16 y0;
  u16 x0;
  u16 y1;
  u16 x1;
  s = self;
  sx = *((s16 *) (s + 0x70));
  sy = *((s16 *) (s + 0x72));
  v = vec;
  tw = 0;
  us = *((u16 *) (s + 0x60));
  ut = *((u16 *) (s + 0x62));
  th = tw;
  xl = sx << 12;
  yl = sy << 12;
  if (flags & 8)
  {
    tilemap = *((u16 **) (s + 0x84));
  }
  else
  {
    tilemap = 0;
  }
  if (p3 == 0)
  {
    return;
  }
  tex = *((int **) ((*((int *) (s + 0x5c))) + ((*((u16 *) (s + 0x5a))) << 2)));
  *((volatile int *) 0x04000480) = color;
  if (tex != 0)
  {
    tw = 8 << tex[5];
    th = 8 << tex[6];
  }
  func_ov007_020c1448(tex, 1, *((int *) (s + 0x64)), *((int *) (s + 0x68)), *((int *) (s + 0x6c)));
  func_ov007_020c1404(tex);
  orig_tw = tw;
  new_var2 = flags;
  *((volatile int *) 0x040004A4) = (((*((int *) (s + 0x54))) | 0x80) | (p4 << 24)) | (p3 << 16);
  if (tex != 0)
  {
    if (us == 1)
    {
      tw = *((u16 *) (s + 0xc));
    }
    if (ut == 1)
    {
      th = *((u16 *) (s + 0xe));
    }
  }
  *((volatile int *) 0x04000444) = 0;
  w = *((volatile int *) (s + 0x20));
  h = *((volatile int *) (s + 0x24));
  {
    int t2 = (int) v[2];
    int t1 = ((int) v[1]) + ((-h) / 2);
    int t0 = ((int) v[0]) + ((-w) / 2);
    *((volatile unsigned int *) 0x4000470) = t0;
    *((volatile unsigned int *) 0x4000470) = t1;
    *((volatile unsigned int *) 0x4000470) = t2;
  }
  scale = data_ov007_0210325c;
  mul_w = (((s64) w) * scale) + 0x800;
  mul_h = (((s64) h) * scale) + 0x800;
  *((volatile int *) 0x0400046C) = (int) (mul_w >> 12);
  *((volatile int *) 0x0400046C) = (int) (mul_h >> 12);
  *((volatile int *) 0x0400046C) = 0x1000;
  if (((flags & 1) == 0) && ((new_var2 & 2) == 0))
  {
    func_ov007_020c49bc(tw, th, xl, yl, us, ut, tilemap, orig_tw);
  }
  else
  {
    use_col = (*((int *) s)) & 2;
    *((volatile int *) 0x04000500) = 1;
    if (use_col != 0)
    {
      *((volatile int *) 0x04000480) = *((u16 *) (s + 0x46));
    }

    h = yl << 8;
    w = h ? (xl << 8) : (xl << 8);
    y0 = (u16) (((s32) h) >> 16);
    x0 = (u16) (((s32) w) >> 16);
    *((volatile int *) 0x04000488) = (y0, x0 | (y0 << 16));

    *((volatile int *) 0x0400048C) = 0x10000000;
    *((volatile int *) 0x0400048C) = 0;
    if (use_col != (0x54 * 0))
    {
      *((volatile int *) 0x04000480) = *((u16 *) (s + 0x48));
    }
    y1 = (u16) (((s32) ((((th << 12) * ut) + yl) << 8)) >> 16);
    x0 = (u16) (((s32) (xl << 8)) >> 16);
    *((volatile int *) 0x04000488) = x0 | (y1 << 16);
    *((volatile int *) 0x0400049C) = 0;
    if (use_col != 0)
    {
      *((volatile int *) 0x04000480) = *((u16 *) (s + 0x4a));
    }
    new_var = (volatile int *) 0x04000488;
    x1 = (u16) (((s32) ((((tw << 12) * us) + xl) << 8)) >> 16);
    *new_var = x1 | (y1 << 16);
    *((volatile int *) 0x04000498) = 0x1000;
    if (use_col)
    {
      *((volatile int *) 0x04000480) = *((u16 *) (s + 0x4c));
    }
    *((volatile int *) 0x04000488) = x1 | (y0 << 16);
    *((volatile int *) 0x0400049C) = 0x1000;
    *((volatile int *) 0x04000504) = 0;
  }
  *((volatile int *) 0x04000448) = 1;
}
