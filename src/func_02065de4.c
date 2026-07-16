
typedef unsigned char u8;
typedef unsigned int u32;
extern char *data_020a9db8;
extern void func_0205a61c(const void *src, void *dst, unsigned int size);
int func_02065de4(u8 *a0, int idx)
{
  int new_var;
  int new_var2;
  char *g = data_020a9db8;
  int r5 = (u8) a0[2];
  int i = idx - 1;
  char *base;
  int cnt = *((int *) ((g + 0x1000) + 0x320));
  char *row = g - (-0x1538);
  {
    int too_big = r5 > cnt;
    row = row + (i << 5);
    if (too_big)
    {
      return 0;
    }
  }
  new_var = (int) g;
  {
    int stride = *((int *) ((g + 0x1000) + 0x31c));
    func_0205a61c(a0 + 3, row + (r5 * stride), stride);
  }
  {
    ((u32 *) (g + 0x1718))[i] |= 1u << r5;
  }
  {
    int j;
    int found = new_var2 = 0;
    j = found;
    char *g2 = data_020a9db8;
    char *b = g2 + 0x1000;
    int n = *((int *) (b + 0x320));
    if (n > 0)
    {
      u32 bits = *((u32 *) (((i * 4) + new_var) + 0x1718));
      do
      {
        if ((1u << j) & bits)
        {
          found++;
          found &= 0xff;
        }
        j = (j + 1) & 0xff;
      }
      while (j < (*((volatile int *) ((g2 + 0x1000) + 0x320))));
    }
    return found == n;
  }
}
