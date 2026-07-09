
extern int func_ov007_020c25fc(void *a, int b, int c, int d, int e, int f);
extern void func_ov007_020bfe4c(void *a, int b, int c, int d, int e);
struct G
{
  char p8[8];
  int f8;
  int fc;
  int f10;
};
struct B
{
  char p8[8];
  unsigned short f8;
  char p[0x16];
  int *f20;
  int *f24;
  int *f28;
};
struct T
{
  char p10[0x10];
  int f10;
  int f14;
  char p18[4];
  struct G *f1c;
  struct B *f20;
  int f24;
  char pad[0x80];
  char *fa8;
};
int func_ov007_020c65bc(struct T *t, int a1, int a2, int a3)
{
  int ret = 0;
  struct B *b = t->f20;
  if (t->f14 < t->f10)
  {
    int m = b->f8 - 1;
    struct G *g = t->f1c;
    if (func_ov007_020c25fc(b, a1 << 12, a2 << 12, g->f8, g->fc, g->f10) != 0)
    {
      int idx = m + 1;
      int *p = (int *)(((int)t + 0x14) & 0xFFFFFFFFFFFFFFFF);
      b->f20[idx] = a3;
      *p = *p + 1;
      func_ov007_020bfe4c(t->fa8, b->f24[idx], b->f28[idx], -(*((int *) (t->fa8 + 0x2c))), t->f24 + (idx * 0xc));
      ret = 1;
    }
  }
  else
  {
    ret = 2;
  }
  return ret;
}
