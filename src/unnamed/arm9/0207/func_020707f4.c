
extern double func_01ff8708(double x, double y);
double func_020707f4(double x, int *eptr)
{
  int hi = ((int *) (&x))[1];
  int lo = ((int *) (&x))[0];
  int ix;
  *eptr = 0;
  ix = hi & 0x7fffffff;
  if ((ix >= 0x7ff00000) || ((ix | lo) == 0))
  {
    return x;
  }
  if (ix < 0x100000)
  {
    x = func_01ff8708(x, 0x1.0p54);
    hi = ((int *) (&x))[1];
    *eptr = -54;
    ix = hi & 0x7fffffff;
  }
  *eptr += (ix >> 20) - 0x3fe;
  hi = (hi & 0x800fffff) | 0x3fe00000;
  ((int *) (&x))[1] = hi;
  return x;
}
