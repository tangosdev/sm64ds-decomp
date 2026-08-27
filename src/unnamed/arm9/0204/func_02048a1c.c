#include "types.h"
extern u32 data_02099fac;
extern int data_020a4bec;
int func_02049018(int *p);
void *func_02050cdc(int a, int idx);
int func_02048a1c(int *a0, int kind, int id)
{
  u32 lim = data_02099fac;
  u32 new_var;
  if (kind == 3)
  {
    if (id >= 0x100 && id <= 0x103)
    {
      lim = 0x8bc;
    }
    else if (id == 0x18d)
    {
      lim = 0x1f4;
    }
    else if (id == 0x81)
    {
      lim = 0x4b0;
    }
    else
    {
      if (id >= 0x99 && id <= 0x9a)
      {
        lim = 0x384;
      }
    }
  }
  else if (kind == 1)
  {
    if (id >= 0xea)
    {
      if (id <= 0x2b)
      {
        lim = 0x44c;
      }
    }
  }
  new_var = (u32) func_02049018(a0);
  if (lim < new_var)
  {
    return 0;
  }

  if (data_020a4bec == 0)
  {
    return 1;
  }
  func_02050cdc(kind, id);
  return 1;
}
