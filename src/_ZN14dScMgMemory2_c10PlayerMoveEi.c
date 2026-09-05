#include "types.h"
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
void _ZN14dScMgMemory2_c10PlayerMoveEi(char *base, int i)
{
  int n = i * 0x14;
  int tx = (i << 4) + 0xc;
  int tz = -0x2c;
  u16 *new_var;
  int dx;
  int dz;
  dx = tx - ((*((int *) ((base + 0x5388) + n))) >> 12);
  dz = tz - ((*((int *) ((base + 0x538c) + n))) >> 12);
  *((s16 *) ((base + 0x5394) + n)) = _ZN4cstd5atan2E5Fix12IiES1_(dz, dx);
  *((int *) ((base + 0x5390) + n)) = (*((int *) ((base + 0x5390) + n))) + 0x200;
  new_var = &(*((u16 *) ((base + 0x5394) + n)));
  *((int *) ((base + 0x5388) + n)) = (*((int *) ((base + 0x5388) + n))) + ((int) (((((s64) data_02082214[(((*new_var) >> 4) << 1) + 1]) * (*((int *) ((base + 0x5390) + n)))) + 0x800) >> 0xc));
  *((int *) ((base + 0x538c) + n)) = (*((int *) ((base + 0x538c) + n))) + ((int) (((((s64) data_02082214[((*new_var) >> 4) << 1]) * (*((int *) ((base + 0x5390) + n)))) + 0x800) >> 0xc));
  dx = tx - ((*((int *) ((base + 0x5388) + n))) >> 12);
  dz = tz - ((*((int *) ((base + 0x538c) + n))) >> 12);
  if ((dx < (-3)) || (dx > 3))
  {
    return;
  }
  if ((dz < (-3)) || (dz > 3))
  {
    return;
  }
  *((int *) ((base + 0x5388) + n)) = tx << 12;
  *((int *) ((base + 0x538c) + n)) = tz << 12;
  *((unsigned char *) ((base + n) + 0x539a)) = 2;
}
