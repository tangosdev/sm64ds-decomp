
int _Z15ApproachLinear2Rsss(short *a, short b, short c);
void func_ov004_020b1b40(void *c);
int _ZN4cstd4fdivEii(int a, int b);
inline long long inline_fn(int arg0)
{
  return (long long) arg0;
}

void func_ov004_020addcc(char *r5)
{
  if (_Z15ApproachLinear2Rsss((short *) (r5 + 0x1a), 0, 1))
  {
    func_ov004_020b1b40((void *) 1);
  }
  int a = *((short *) (r5 + 0x1a));
  int b = *((short *) (r5 + 0x1c));
  int s = _ZN4cstd4fdivEii(a << 0xc, b << 0xc);
  int c3 = *((short *) (r5 + 0x1c));
  int c2 = *((short *) (r5 + 0x1a));
  int t = _ZN4cstd4fdivEii((c3 - c2) << 0xc, c3 << 0xc);
  int v8 = *((int *) (r5 + 8));
  int v10 = *((int *) (r5 + 0x10));
  long long m1 = inline_fn(v8) * inline_fn(s);
  long long m2 = inline_fn(v10) * inline_fn(t);
  long long m3 = inline_fn(s) * inline_fn(s);
  long long m4 = inline_fn((int) ((m3 + 0x800) >> 0xc)) * inline_fn((*((int *) (r5 + 0xc))) - (*((int *) (r5 + 0x14))));
  *((int *) r5) = ((int) ((m1 + 0x800) >> 0xc)) + ((int) ((m2 + 0x800) >> 0xc));
  *((int *) (r5 + 4)) = (*((int *) (r5 + 0x14))) + ((int) ((m4 + 0x800) >> 0xc));
}
