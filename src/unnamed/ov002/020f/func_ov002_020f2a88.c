#include "types.h"
void func_ov002_020f2a88(char *c, int i)
{
  unsigned long new_var;
  *((short *) ((c + 0x1c4) + (i * 8))) = (*((u16 *) ((c + 0x1c4) + (i * 8)))) + 1;
  new_var = i;
  if ((*((u16 *) ((c + (new_var * 8)) + 0x1c4))) < 0x39)
  {
    return;
  }
  *((u16 *) ((c + (new_var * 8)) + 0x1c4)) = 0;
  *((u8 *) ((c + 0x1c9) + (new_var * 8))) = (*((u8 *) ((c + 0x1c9) + (new_var * 8)))) + 1;
}
