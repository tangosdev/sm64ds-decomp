
typedef long long s64;
struct S;
extern void func_020551f0(struct S *s, int v);
inline int inline_fn(int arg0)
{
  return arg0;
}

void func_ov007_020c8b34(char *c, int t)
{
  int new_var2;
  int v = *((int *) (c + 0x30));
  int ip = *((int *) (c + 0x2c));
  int new_var;
  if (t < 0x1000)
  {
    if (t <= 0)
    {
      v = ip;
    }
    else
    {
      int mid = (int) (((((s64) t) * (inline_fn(v) << 0xc)) + (((s64) (0x1000 - t)) * (ip << 0xc))) >> 0xc);
      new_var = mid;
      v = new_var >> 0xc;
    }
  }
  if ((*((int *) (c + 4))) & 1)
  {
    new_var2 = 0x4000050;
    func_020551f0((struct S *) new_var2, v);
  }
  if ((*((int *) (c + 4))) & 2)
  {
    func_020551f0((struct S *) 0x4001050, v);
  }
}
