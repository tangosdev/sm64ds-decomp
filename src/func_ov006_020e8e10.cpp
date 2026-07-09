//cpp
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, u16 a6);

struct Ent { u16 f0, f2, f4, f6; };
extern Ent *data_ov006_02133f24[];

struct P {
  char pad[0x208];
  int i208;
  int i20c;
  char pad2[0x219 - 0x210];
  u8 b219;
  u8 b21a;
  u8 b21b;
  u8 b21c;
  char pad3[0x54e - 0x21d];
  u8 b54e;
};

#define PP(base) ((P *)((base) + 0x5000))

extern "C" void func_ov006_020e8e10(char *c0) {
  int i;
  int x;
  int y;
  int c;
  Ent *r7;
  char *c2;
  u8 b;
  c2 = c0;
  for (i = 0; i < 5; i++) {
    if (PP(c2)->b21a != 0) {
      b = PP(c2)->b21b;
      x = PP(c2)->i208 >> 0xc;
      y = PP(c2)->i20c >> 0xc;
      c = PP(c2)->b21c;
      r7 = data_ov006_02133f24[b];
      for (;;) {
        int *r0 = (int *)func_ov004_020af770(
            (int)r7, x, y, -1, c, 0x1000, 0);
        if (PP(c0)->b54e && !PP(c2)->b219) {
          int v = r0[0];
          int w = r0[1];
          int bits = (int)((unsigned int)(w << 0x10) >> 0x1c);
          r0[0] = (v & ~0xc00) | 0x400;
          *(u16 *)((char *)r0 + 4) = (u16)((*(u16 *)((char *)r0 + 4) & ~0xf000) | (bits << 12));
        }
        if (r7->f6 == 0xffff) break;
        r7++;
      }
    }
    c2 += 0x18;
  }
}
