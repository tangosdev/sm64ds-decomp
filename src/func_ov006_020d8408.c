#include "types.h"

extern int RandomIntInternal(int *seed);
extern void func_ov006_020d66c4(char *base, int idx);
extern int data_0209d4b8;
extern u16 data_ov006_0212e2e8[];
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_loop_invariants off
void func_ov006_020d8408(char *c)
{
  s32 sb;
  s32 step;
  s32 ang;
  s32 i;
  s32 one;
  s32 off;
  s32 t0;
  u32 rnd;
  s32 cnt;
  s32 flag;
  s32 j;
  s32 z0;
  s32 z1;
  s32 z2;
  s32 z3;
  s32 v80000;
  s32 new_var;
  s32 v4;
  s32 v200;
  s32 vB8000;
  s32 j0;
  if ((*((u16 *) ((c + 0x6200) + 0xe2))) != 0)
  {
    (*((u16 *) ( ((int) (c + 0x62e2)))))--;
    if ((*((s16 *) ((c + 0x6200) + 0xe2))) < 0)
    {
      *((u16 *) ((c + 0x6200) + 0xe2)) = 0;
    }
    return;
  }
  t0 = *((s32 *) ((c + 0x6000) + 0x2d8));
  sb = 0;
  if (t0 >= 0x12c)
  {
    sb = 0xc;
  }
  else
    if (t0 >= 0xc6)
  {
    sb = 0xb;
  }
  else
    if (t0 >= 0x9f)
  {
    sb = 0xa;
  }
  else
    if (t0 >= 0x84)
  {
    sb = 9;
  }
  else
    if (t0 >= 0x5c)
  {
    sb = 8;
  }
  else
    if (t0 >= 0x39)
  {
    sb = 7;
  }
  else
    if (t0 >= 0x21)
  {
    sb = 6;
  }
  else
    if (t0 >= 0x1b)
  {
    sb = 5;
  }
  else
    if (t0 >= 0x15)
  {
    sb = 4;
  }
  else
    if (t0 >= 0xf)
  {
    sb = 3;
  }
  else
    if (t0 >= 9)
  {
    sb = 2;
  }
  else
    if (t0 >= 3)
  {
    sb = 1;
  }
  cnt = 1;
  step = 0;
  if (sb >= 5)
  {
    cnt = 2;
  }
  if (sb == 9)
  {
    cnt = 1;
  }
  flag = 0;
  if (sb == 7)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 2;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x3000;
  }
  new_var = sb;
  if (new_var == 8)
  {
    s32 bit = (*((unsigned char *) ((c + 0x6000) + 0x2fc))) & 1;
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 x = *pf;
    cnt = bit + 2;
    *pf = x ^ 1;
    flag = bit + 1;
    if (cnt == 2)
    {
      step = 0x3000;
    }
    else
    {
      step = 0x1800;
    }
  }
  if (new_var == 10)
  {
    cnt = 4;
    step = 0x3000;
  }
  if (sb == 11)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 3;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x1800;
  }
  if (sb >= 12)
  {
    cnt = 6;
    step = 0x1800;
  }
  i = 0;
  if (cnt > 0)
  {
    v80000 = 0x80000;
    v4 = 4;
    v200 = 0x200;
    vB8000 = 0xb8000;
    off = 0x4698;
    ang = 0;
    z0 = 0;
    z1 = 0;
    z2 = 0;
    z3 = 0;
    one = 1;
    j0 = 0;
    do
    {
      s32 z = z0;
      j = j0;
      while (1)
      {
        char *row;
        unsigned char *slot = (unsigned char *) ((c + (j << 6)) + off);
        if ((*slot) == 0)
        {
          unsigned char *ptype;
          *slot = (unsigned char) one;
          (c + (j << 6))[0x4697] = (char) z;
          (c + (j << 6))[0x469b] = (char) z;
          (c + (j << 6))[0x469c] = (char) z;
          (c + (j << 6))[0x469d] = (char) z;
          *((s32 *) ((c + (j << 6)) + 0x4660)) = v80000;
          *((s16 *) ((c + (j << 6)) + 0x4690)) = (s16) v4;
          *((s16 *) ((c + (j << 6)) + 0x4692)) = (s16) v200;
          rnd = (u32) RandomIntInternal(&data_0209d4b8);
          ptype = (unsigned char *) ((int) ( ((int) ((c + (j << 6)) + 0x4696))));
          *ptype = (((rnd >> 16) & 0x7fff) << 1) >> 15;
          *((s32 *) ((c + (j << 6)) + 0x4670)) = 0x999;
          *((s32 *) ((c + (j << 6)) + 0x4688)) = z1;
          if (sb == 0)
          {
            *ptype = (unsigned char) one;
            ((void (*)(char *)) func_ov006_020d66c4)(c);
          }
          if (sb <= 1)
          {
            rnd = (u32) RandomIntInternal(&data_0209d4b8);
            *((s16 *) ((((0, c)) + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
          }
          else
            if (flag != 0)
          {
            if (flag == 1)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0x2000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
              func_ov006_020d66c4(c, z2);
            }
            else
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0xa000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
              func_ov006_020d66c4(c, one);
            }
          }
          else
            if (((*((s32 *) ((c + 0x6000) + 0x2d8))) & 1) != 0)
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0x2800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z3;
            func_ov006_020d66c4(c, z3);
          }
          else
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0xa800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0xa000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
            func_ov006_020d66c4(c, one);
          }
          (*((s32 *) ((int) ( ((int) (c + 0x62d8))))))++;
          break;
        }
        j++;
        if (j >= 0x70)
        {
          break;
        }
      }

      ang += step;
      i++;
    }
    while (i < cnt);
  }
  *((u16 *) ((c + 0x6200) + 0xe2)) = data_ov006_0212e2e8[sb];
}
