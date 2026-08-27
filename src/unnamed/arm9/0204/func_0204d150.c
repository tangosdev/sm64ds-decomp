#include "types.h"
#pragma opt_strength_reduction off
void func_0204d150(char *r0, char *r1, int r2)
{
  u8 *t = *((u8 **) (r1 + 0x10));
  int i = 0;
  int acc;
  int count = t[8];
  u8 width;
  int new_var;
  if (count <= (new_var = 0))
  {
    return;
  }
  width = t[9];
  acc = 0;
  do
  {
    if (r2 < (acc + width))
    {
      r0[0x42] = t[i];
      return;
    }
    i++;
    acc += width;
  }
  while (i < t[8]);
}
