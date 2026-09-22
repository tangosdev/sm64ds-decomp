#include "types.h"

struct Widget
{
  int f_0;
  int f_4;
  int f_8;
  u16 f_c;
  u16 f_e;
  int f_10;
  char _14[0x30];
  u16 f_44;
  u16 f_46;
  u16 f_48;
  u16 f_4a;
  u16 f_4c;
  u8 f_4e;
  char _4f[1];
  int f_50;
  char _54[8];
  int *f_5c;
  u16 f_60;
  u16 f_62;
  char _64[0xc];
  u16 f_70;
};

struct dScene_c
{
  char _0[0x34];
  int f_34;
  char _38[0x64];
  int f_9c;
  char _a0[0x48];
  int f_e8;
  int f_ec;
  char _f0[4];
  int f_f4;
  int f_f8;
  char _fc[4];
  int f_100;
  int f_104;
  struct Widget *f_108[3];
};

struct Vec3
{
  int x;
  int y;
  int z;
};

extern struct dScene_c *data_ov007_0210342c;
extern void func_ov007_020b283c(void);
extern int func_ov007_020b2bd4(int a, int b);
extern int func_ov007_020beaa0(int a, int b);
extern void func_ov007_020b7708(int idx, struct Vec3 *pos);
extern struct Widget *func_ov007_020c44e4(int n);
extern void func_ov007_020c43bc(struct Widget *w, int r1);
extern void func_ov007_020c4388(struct Widget *w, int r1);

void func_ov007_020b2998(void)
{
  int i;
  struct Widget *w;
  struct Vec3 pos;

  func_ov007_020b283c();
  data_ov007_0210342c->f_f4 = func_ov007_020b2bd4(data_ov007_0210342c->f_ec,
      *((u8 *) ((((char *) data_ov007_0210342c) + data_ov007_0210342c->f_ec) + 0xfc)));
  data_ov007_0210342c->f_100 = func_ov007_020beaa0(0, data_ov007_0210342c->f_ec);
  data_ov007_0210342c->f_104 = func_ov007_020beaa0(1, data_ov007_0210342c->f_ec);
  pos.x = pos.y = pos.z = 0;
  func_ov007_020b7708(0, &pos);
  func_ov007_020b7708(1, &pos);
  data_ov007_0210342c->f_f8 = 0x1000;
  for (i = 0; i < 3; i++)
  {
    data_ov007_0210342c->f_108[i] = func_ov007_020c44e4(1);
    w = data_ov007_0210342c->f_108[i];
    w->f_4 = 0x80000;
    w->f_8 = 0x60000;
    w->f_c = w->f_e = 0x100;
    w->f_46 = w->f_4c = 0;
    w->f_48 = w->f_4a = 0;
    switch (i)
    {
    case 0:
      w->f_4e = 0x14;
      w->f_10 = -0x320ff;
      w->f_0 |= 2;
      break;
    case 1:
      w->f_4e = 0x15;
      w->f_10 = -0x32000;
      *w->f_5c = data_ov007_0210342c->f_9c;
      w->f_70 = 0x20;
      w->f_60 = 4;
      w->f_62 = 1;
      w->f_0 |= 2;
      break;
    case 2:
      w->f_4e = 0x16;
      w->f_10 = -0x31f01;
      w->f_44 = 0x7fff;
      w->f_50 = 0x1000;
      w->f_0 |= 1;
      break;
    }
    func_ov007_020c43bc(w, data_ov007_0210342c->f_34);
    func_ov007_020c4388(w, data_ov007_0210342c->f_34);
  }
  data_ov007_0210342c->f_e8 = 1;
}
