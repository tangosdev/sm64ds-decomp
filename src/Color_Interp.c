inline int inline_fn(int arg0, int arg1, int arg2)
{
  return (arg0 * arg1) + arg2;
}

int Color_Interp(int a, int c1, int c2, int t)
{
  int q0;
  int q1;
  int q2;
  int s;
  unsigned int r;
  if (t >= 0x1000)
  {
    return c2;
  }
  if (t <= 0)
  {
    return c1;
  }
  s = 0x1000 - t;
  q0 = (c1 & 0x1f) * s;
  q1 = (c1 & 0x3e0) * s;
  q2 = (c1 & 0x7c00) * s;
  q0 = inline_fn(c2 & 0x1f, t, q0);
  q1 = inline_fn(c2 & 0x3e0, t, q1);
  q2 = inline_fn(c2 & 0x7c00, t, q2);
  r = ((unsigned int) ((q0 | q1) | q2)) << 4;
  return r >> 0x10;
}
