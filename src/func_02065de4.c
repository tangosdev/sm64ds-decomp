#include "types.h"
extern char *data_020a9db8;
extern void CpuCopy8(const void *src, void *dst, unsigned int size);
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
    CpuCopy8(a0 + 3, row + (r5 * stride), stride);
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
      /* The bitmask word is reached as +0x1000 then +0x718: spelled as one
         +0x1718 mwcc shares the constant with the |= above (kept live in ip
         from the literal pool) and emits ldr r6,[r4,ip]; the cartridge does
         add r4,r4,#0x1000 / ldr r6,[r4,#0x718]. */
      char *q = (char *) ((i * 4) + new_var) + 0x1000;
      u32 bits = *((u32 *) (q + 0x718));
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
