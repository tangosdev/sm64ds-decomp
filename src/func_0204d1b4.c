
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern int __aeabi_idiv(int a, int b);
extern u32 data_020a4d30;
struct Dst
{
  char pad0[0x40];
  u16 u40;
};
struct Color555
{
  u16 r : 5;
  u16 g : 5;
  u16 b : 5;
  u16 a : 1;
};
struct ColorEntry
{
  struct Color555 color;
  u8 field2;
  u8 pad3;
  u8 lo;
  u8 hi;
};
struct Src
{
  char pad0[0xc];
  struct ColorEntry *p0c;
};
void func_0204d1b4(struct Dst *dst, struct Src *src, int t)
{
  struct ColorEntry *e = src->p0c;
  int lo = e->lo;
  int hi = e->hi;
  int r1;
  if (t < lo)
  {
    int rr = e->color.r;
    int gg = e->color.g;
    int c = gg - rr;
    int tmp = __aeabi_idiv(t * c, lo);
    r1 = tmp + rr;
  }
  else
    if (t < hi)
  {
    r1 = e->color.g;
  }
  else
  {
    int gg = e->color.g;
    int bb = e->color.b;
    int d = bb - gg;
    int tmp = __aeabi_idiv((t - 0xff) * d, 0xff - hi);
    r1 = tmp + bb;
  }
  u16 *p_d = (u16 *) (((unsigned long long) ((unsigned int) (&dst->u40))) & 0xFFFFFFFFFFFFFFFFULL);
  u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
  data_020a4d30 = s;
  int b = e->field2;
  u16 old = *p_d;
  *p_d = (u16) ((old & (~0x3e0)) |
    ((((u32) ((r1 * (0xff - ((b * (int) (s >> 0x18)) >> 8))) << 8) >> 0x10) & 0x1f) << 5));
}
