
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
extern char *data_ov007_0210342c;
extern u8 data_ov007_020ccc44[];
extern void func_ov007_020c0288(u16 *p, int count, int val);
extern void func_ov007_020c0638(void *a, int b, int c, int d);
void func_ov007_020b1390(int mode)
{
  char *g = data_ov007_0210342c;
  char *p6 = *((char **) ((*((char **) (g + 0xcc))) + 4));
  char *p5 = *((char **) (*((char **) (g + 0xc0))));
  int v;
  char *base;
  v = data_ov007_020ccc44[(mode == 0) ? (1) : (0)];
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x106), 0x20, v);
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x146), 0x20, v);
  v = data_ov007_020ccc44[(mode == 1) ? (1) : (0)];
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x286), 0x20, v);
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x2c6), 0x20, v);
  v = data_ov007_020ccc44[(mode == 2) ? (1) : (0)];
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x386), 0x20, v);
  base = *((char **) (p6 + 0x14));
  func_ov007_020c0288((u16 *) (base + 0x3c6), 0x20, v);
  func_ov007_020c0638(p6, 0, 0, 0);
  switch (mode)
  {
    case 0:
    {
      char *q = *((char **) (p5 + 0x24));
      *((int *) (p5 + 8)) = (*((s16 *) (q + 2))) << 0xc;
      *((int *) (p5 + 0xc)) = 0x28000;
      break;
    }

    case 1:
    {
      char *q = *((char **) (p5 + 0x24));
      *((int *) (p5 + 8)) = (*((s16 *) (q + 2))) << 0xc;
      *((int *) (p5 + 0xc)) = 0x58000;
      break;
    }

    case 3:
    {
      char **r2 = *((char ***) (data_ov007_0210342c + 0x134));
      unsigned int o = *((u16 *) (r2[1] + 0xc));
      o = o >> 1;
      o = o + 0xc;
      *((int *) (p5 + 8)) = (*((int *) (r2[0] + 8))) - (o << 12);
      *((int *) (p5 + 0xc)) = 0xa2000;
      break;
    }

  }

}
