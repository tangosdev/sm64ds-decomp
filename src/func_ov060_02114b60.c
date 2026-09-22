#include "types.h"
extern void func_02012694(int a, void *b);
extern int _ZN8dActor_c14GetSubtractionEss(char *self, s16 a, s16 b);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char *self);
void func_ov060_02114b60(char *c)
{
  int *pflag = (int *) ((((int) c) + 0x378));
  *pflag |= 1;
  switch (*((u8 *) (c + 0x423)))
  {
    case 0:
      *((u8 *) (c + 0x41d)) = 0;
      *((s16 *) (c + 0x3fe)) = 0x1e;
      if ((*((u16 *) (c + 0x3fc))) == 0)
    {
      func_02012694(0xb9, c + 0x74);
    }
      if ((*((u8 *) (c + 0x41c))) != 0)
    {
      return;
    }
    {
      u8 *ps = (u8 *) ((((int) c) + 0x423));
      *ps = (*ps) + 1;
    }
      *((s16 *) (c + 0x8e)) = *((s16 *) (c + 0x406));
      return;

    case 1:
    {
      int r4 = 0;
      u16 *pd = (u16 *) ((((int) c) + 0x3fe));
      char *base3 = c + 0x300;
      int sub;
      u16 h = *pd;
      u16 h2 = *((u16 *) (base3 + 0xfe));
      *((u16 *) ((((int) c) + 0x3fe))) = h - 1;
      if (h2 != 0)
      {
        *((int *) (c + 0x98)) = 0x64000;
      }
      else
      {
        r4 = 1;
      }
      sub = _ZN8dActor_c14GetSubtractionEss(c, *((s16 *) (c + 0x8e)), *((s16 *) (c + 0x406)));
      if (sub > 0x4000)
      {
        if ((*((int *) (c + 0x3ec))) > 0x1f4000)
        {
          r4 = 1;
        }
      }
      if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x14c) == 0)
      {
        *((int *) (c + 0x5c)) = *((int *) (c + 0x3c8));
        *((int *) (c + 0x60)) = *((int *) (c + 0x3cc));
        *((int *) (c + 0x64)) = *((int *) (c + 0x3d0));
        r4 = 1;
        *((int *) (c + 0x98)) = 0;
      }
      if (r4 == 0)
      {
        return;
      }
      *((u8 *) (c + 0x423)) = 2;
      *((s16 *) (c + 0x8e)) = *((s16 *) (c + 0x406));
      return;
    }

    case 2:
      *((int *) (c + 0x98)) = 0;
      *((u8 *) (c + 0x41d)) = 0xff;
      if ((*((u8 *) (c + 0x41c))) == 0xff)
    {
      *((int *) (c + 0x40c)) = 0;
      *pflag &= ~1;
    }
      return;

    default:
      return;

  }

}
