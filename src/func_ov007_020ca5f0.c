
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;
extern void func_ov007_020c1448(void *a, int b, int c, int d, int e);
extern void func_ov007_020c1404(void *c);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020caeac(int a0, int a1, int a2, int a3, int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8);
extern void func_ov007_020ca86c(int a0, int a1, int a2, int a3, int s0);
void func_ov007_020ca5f0(char *a1, char *a2, int *a3, int a4, int a5)
{
  int *r7 = *((int **) a2);
  int *pdim = *((int **) (a1 + 4));
  int d0 = pdim[0] - 1;
  int d1 = pdim[1] - 1;
  int tw = 0;
  int th = 0;
  *((volatile u32 *) 0x40004c0) = (((u32) (*((u16 *) (a2 + 0x28)))) | (((u32) (*((u16 *) (a2 + 0x2a)))) << 16)) | 0x8000;
  *((volatile u32 *) 0x40004c4) = 0;
  if (r7 != 0)
  {
    tw = 8 << r7[0x14 / 4];
    th = 8 << r7[0x18 / 4];
  }
  func_ov007_020c1448(r7, 1, *((int *) (a2 + 0x1c)), 0, *((int *) (a2 + 0x20)));
  func_ov007_020c1404(r7);
  *((volatile u32 *) 0x40004a4) = ((0x8c1 | ((*((int *) (a2 + 0x2c))) << 4)) | ((*((int *) (a2 + 0x30))) << 24)) | ((*((int *) (a2 + 0x24))) << 16);
  *((volatile u32 *) 0x4000444) = 0;
  {
    int t2 = a3[2];
    int t1 = a3[1];
    int t0 = a3[0];
    *((volatile u32 *) 0x4000470) = t0;
    *((volatile u32 *) 0x4000470) = t1;
    *((volatile u32 *) 0x4000470) = t2;
  }
  *((volatile u32 *) 0x400046c) = 0x1000;
  *((volatile u32 *) 0x400046c) = 0x1000;
  *((volatile u32 *) 0x400046c) = a4;
  *((volatile u32 *) 0x4000444) = 0;
  *((volatile u32 *) 0x400046c) = 0x40000;
  *((volatile u32 *) 0x400046c) = 0x40000;
  *((volatile u32 *) 0x400046c) = 0x40000;
  if (a5 != 0)
  {
    int q1 = _ZN4cstd3divEii(tw * (*((int *) (a2 + 0x14))), d0);
    int q2 = _ZN4cstd3divEii(th * (*((int *) (a2 + 0x18))), d1);
    int fx1 = (int) (((((s64) (*((s16 *) (a1 + 0x24)))) * (*((int *) (a2 + 0xc)))) + 0x800) >> 12);
    int fx2 = (int) (((((s64) (*((s16 *) (a1 + 0x26)))) * (*((int *) (a2 + 0x10)))) + 0x800) >> 12);
    func_ov007_020caeac(*((int *) a1), *((int *) (a1 + 0x2c)), *((int *) (a1 + 0x30)), *((int *) (a1 + 0x34)), d0, d1, *((int *) (a2 + 4)), *((int *) (a2 + 8)), q1, q2, fx1, fx2, 6);
  }
  else
  {
    int n = (d1 * d0) << 1;
    if (n > 0)
    {
      int i = 0;
      int off = 0;
      tw = n;
      do
      {
        s16 *p;
        p = (s16 *) ((*((int *) (a1 + 0x28))) + off);
        if ((*p) > 0xfff)
        {
          *p = 0xfff;
        }
        p = (s16 *) ((*((int *) (a1 + 0x28))) + off);
        if (p[1] > 0xfff)
        {
          p[1] = 0xfff;
        }
        p = (s16 *) ((*((int *) (a1 + 0x28))) + off);
        if (p[2] > 0xfff)
        {
          p[2] = 0xfff;
        }
        off += 6;
        i++;
      }
      while (i < tw);
    }
    func_ov007_020ca86c(*((int *) a1), *((int *) (a1 + 0x28)), d0, d1, 6);
  }
  *((volatile u32 *) 0x4000448) = 2;
}
