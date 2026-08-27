#include "types.h"
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int data_ov006_021428c8;
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int func_ov006_02108650(int, int);
void func_ov006_02108d28(int *p)
{
  int idx;
  int flag;
  int new_var;
  int dx;
  int dy;
  int px, py, bx, by;
  s16 ang;
  new_var = 4;
  if (data_ov006_021428c8 != 0) return;
  idx = data_020a0e40;
  flag = 0;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0) return;
  if (*((u8 *)(((char *)p) + 0x32)) != 1) return;
  idx = data_020a0e40;
  px = p[0];
  py = p[1];
  bx = data_020a0dea[idx * new_var];
  ang = *((s16 *)(((char *)p) + 0x2c));
  px >>= 12;
  by = data_020a0deb[idx * 4];
  py >>= 12;
  dx = px - bx;
  dy = py - by;
  if (ang != 0x25) return;
  if (func_ov006_02108650(bx, by) == 0x25) return;
  _ZN5Sound12PlayBank2_2DEj(0x15d);
  data_ov006_021428c8 = (int)p;
  *((u8 *)(((char *)p) + 0x32)) = 2;
  p[2] = dx << 12;
  p[3] = dy << 12;
}
