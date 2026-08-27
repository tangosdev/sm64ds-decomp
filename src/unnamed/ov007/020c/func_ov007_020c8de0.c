typedef short s16;
typedef long long s64;

struct Vec3
{
  int x;
  int y;
  int z;
};

extern void func_ov007_020c90c0(void *self);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern int LenVec3(struct Vec3 *v);
extern void NormalizeVec3(struct Vec3 *a, struct Vec3 *b);
extern void func_02053320(int s, int *a, int *b, int *out);

struct T
{
  int state;
  int sub;
  char pad_8[4];
  s16 f_c;
  char pad_e[2];
  int *f10;
  int f14;
  int f18;
  int f1c;
  int f20;
  int f24;
  int f28;
};

int func_ov007_020c8de0(struct T *o)
{
  struct Vec3 tmp;
  if (o->state == 2)
  {
    o->state = 3;
  }
  if (o->state != 1)
  {
    goto done;
  }
  switch (o->sub)
  {
    case 0:
    {
      int t0 = o->f_c;
      int a0 = o->f20;
      int b0 = o->f14;
      o->f10[0] = (t0 >= 0x1000) ? (a0) : ((t0 <= 0) ? (b0) : (((((s64) (0x1000 - t0)) * b0) + (((s64) t0) * o->f20)) >> 0xc));
    }
    {
      int t1 = o->f_c;
      int a1 = o->f24;
      int b1 = o->f18;
      o->f10[1] = (t1 >= 0x1000) ? (a1) : ((t1 <= 0) ? (b1) : (((((s64) (0x1000 - t1)) * b1) + (((s64) t1) * o->f24)) >> 0xc));
    }
    {
      int t2 = o->f_c;
      int a2 = o->f28;
      int b2 = o->f1c;
      o->f10[2] = (t2 >= 0x1000) ? (a2) : ((t2 <= 0) ? (b2) : (((((s64) (0x1000 - t2)) * b2) + (((s64) t2) * o->f28)) >> 0xc));
    }
    func_ov007_020c90c0(o);
    break;

    case 1:
    {
      int *dst = o->f10;
      int r5 = *((int *) (((char *) o) + 8));
      int ok;
      SubVec3((struct Vec3 *) (&o->f20), (struct Vec3 *) dst, &tmp);
      if (LenVec3(&tmp) <= r5)
      {
        *((struct Vec3 *) dst) = *((struct Vec3 *) (&o->f20));
        ok = 1;
      }
      else
      {
        NormalizeVec3(&tmp, &tmp);
        func_02053320(r5, (int *) (&tmp), dst, dst);
        ok = 0;
      }
      if (ok)
      {
        o->state = 2;
      }
      break;
    }
  }

done:
  return (o->state == 3) ? (1) : (0);
}
