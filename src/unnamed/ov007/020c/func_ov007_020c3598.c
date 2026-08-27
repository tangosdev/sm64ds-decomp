
extern int _ZN4cstd3divEii(int, int);
int func_ov007_020c3598(int val, int n, char *buf, int base)
{
  int res = 0;
  int fnd = 0;
  int r7 = 1;
  int i;
  for (i = 0; i < (n - 1); i++)
  {
    r7 = r7 * base;
  }

  for (i = 0; i < n; i++)
  {
    int neg1 = -1;
    int one = 1;
    int pos = (n - 1) - i;
    char *p = buf + pos;
    buf[pos] = (char) _ZN4cstd3divEii(val, r7);
    val -= r7 * ((signed char) buf[pos]);
    r7 = _ZN4cstd3divEii(r7, base);
    if (fnd == 0)
    {
      if (((signed char) (*p)) != 0)
      {
        fnd = one;
        res = pos + 1;
      }
      else if (pos > 0)
      {
        *p = neg1;
      }
    }
  }

  return res;
}
