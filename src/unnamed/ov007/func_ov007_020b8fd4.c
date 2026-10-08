#include "types.h"
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020c4388(char *r0, int r1);
extern void func_ov007_020c421c(char *r4);
extern char *data_ov007_02104b9c;
extern char *data_ov007_02104ba0;
extern char *data_ov007_0210342c;
void func_ov007_020b8fd4(int param)
{
  int v;
  int mode;
  int d;
  char *b;
  int other;
  int scale;
  char *p_c;
  char *p_10;
  char *p5;
  int f8;
  char *inner;
  char *node;
  s16 *ps;
  int flag;
  int f14;
  int sh;
  v = *((int *) data_ov007_02104b9c);
  if (v <= 0x1000)
  {
    *((int *) ((*((char **) (data_ov007_02104b9c + 8))) + 4)) = 0x80000;
    b = *((char **) (data_ov007_02104b9c + 8));
    *((int *) (b + 8)) = 0x60000;
    mode = *((int *) (data_ov007_02104ba0 + 0x20));
    switch (mode)
    {
      case 0:
        b = *((char **) (data_ov007_02104b9c + 8));
        *((int *) (b + 4)) += (0x1000 - v) << 8;
        break;
      case 3:
        d = 0x1000 - v;
        b = *((char **) (data_ov007_02104b9c + 8));
        *((int *) (b + 8)) += (-d) << 8;
        break;
    }
    func_ov007_020c4388(*((char **) (data_ov007_02104b9c + 8)), *((int *) (data_ov007_0210342c + 0x34)));
  }
  func_ov007_020c421c(*((char **) (data_ov007_02104b9c + 4)));
  func_ov007_020c421c(*((char **) (data_ov007_02104b9c + 8)));

  {
    char *c;
    char *g;
    f8 = (int) data_ov007_02104b9c;
    c = *((char * volatile *) &data_ov007_02104ba0);
    g = *((char * volatile *) &data_ov007_0210342c);
    flag = *((int *) (c + 0x30));
    p5 = *((char **) (g + 0x128));
    inner = *((char **) c);
    node = *((char **) p5);
    ps = *((s16 **) (node + 4));
    p_c = *((char **) (((char *) f8) + 0xc));
    p_10 = *((char **) (((char *) f8) + 0x10));
    f8 = *((int *) (inner + 8));
    sh = *ps;
    f14 = *((int *) (inner + 0x14));
    if (flag)
    {
      f8 = 0;
      f14 = f8;
    }
  }

  if (sh == 1)
  {
    return;
  }
  scale = _ZN4cstd3divEii(f14 << 12, 500);
  other = _ZN4cstd3divEii(f8 << 12, 200);
  if (scale <= other)
  {
    scale = other;
  }
  *((int *) (p_c + 4)) = (*((int *) ((*((char **) p5)) + 8))) + (((int) ((*((u16 *) (p_c + 0xc))) * scale)) / 2);
  *((int *) (p_c + 8)) = *((int *) ((*((char **) p5)) + 0xc));
  *((int *) (p_c + 0x3c)) = 0x1000 - scale;
  *((int *) (p_10 + 4)) = *((int *) ((*((char **) p5)) + 8));
  *((int *) (p_10 + 8)) = *((int *) ((*((char **) p5)) + 0xc));
  func_ov007_020c4388(p_c, param);
  func_ov007_020c4388(p_10, param);
  func_ov007_020c421c(p_c);
  func_ov007_020c421c(p_10);
}
