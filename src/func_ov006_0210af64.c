typedef unsigned char u8;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0deb[];
extern int data_ov006_0213e948[];
extern void func_ov006_0210ab08(char *c, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int func_020124c4(int, int, int, int);
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
void func_ov006_0210af64(char *c)
{
  u8 idx;
  int ok;
  int off;
  int i;
  int new_var;
  int *tab;
  idx = data_020a0e40[0];
  ok = 0;
  off = idx * 4;
  new_var = 1;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[off])
    {
      ok = 1;
    }
  }
  if (ok != 0)
  {
    unsigned int v = data_020a0deb[idx * 4];
    if ((v >= 0x40) && (v < 0x58))
    {
      if ((*((u8 *) (c + 0x502b))) == new_var)
      {
        *((u8 *) (c + 0x502b)) = 0;
      }
    }
    else
      if ((v >= 0x58) && (v < 0x70))
    {
      if ((*((u8 *) (c + 0x502c))) == 1)
      {
        *((u8 *) (c + 0x502c)) = 0;
      }
    }
    else
      if ((v >= 0x70) && (v < 0x88))
    {
      if ((*((u8 *) (c + 0x502d))) == new_var)
      {
        *((u8 *) (c + 0x502d)) = 0;
      }
    }
  }
  i = 0;
  for (; i < 3; i++)
  {
    if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e))) & 0xFFFFFFFFFFFFFFFFLL)))) == 0)
    {
      int *p = (int *) (c + (i * 4));
      int val;
      int q;
      int q2;
      u8 *dir;
      tab = data_ov006_0213e948;
      p = (int *) ((int) (((long long) ((int) (((int) p) + 0x4fe4))) & 0xFFFFFFFFFFFFFFFFLL));
      val = *p;
      ;
      q = (val >> 12) / 0x50;
      if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))) & 0xFFFFFFFFFFFFFFFFLL)))) == 1)
      {
        *p = val - tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) < 0)
        {
          *p += *((int *) (c + 0x4ff0));
        }
      }
      else
      {
        *p = val + tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) >= (*((int *) (c + 0x4ff0))))
        {
          *p -= *((int *) (c + 0x4ff0));
        }
      }
      if ((*((u8 *) ((c + i) + 0x502b))) == 0)
      {
        q2 = ((*p) >> 12) / 0x50;
        if (q2 != q)
        {
          if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))) & 0xFFFFFFFFFFFFFFFFLL)))) == new_var)
          {
            *p = (q * 0x50) << 12;
          }
          else
          {
            *p = (q2 * 0x50) << 12;
          }
          func_ov006_0210ab08(c, i);
          *((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e))) & 0xFFFFFFFFFFFFFFFFLL))) = 1;
          *((u8 *) ((int) (((long long) ((int) (c + 0x503d))) & 0xFFFFFFFFFFFFFFFFLL))) += new_var;
          *((u8 *) (c + 0x503f)) = 0;
          _ZN5Sound12PlayBank2_2DEj(0x1a9);
        }
      }
    }
  }

  *((int *) (c + 0x5008)) = func_020124c4(*((int *) (c + 0x5008)), 2, 0x1a8, 0);
  if ((*((u8 *) (c + 0x503d))) >= 3u)
  {
    *((u8 *) (c + 0x503e)) = 0x1e;
    *((int *) (c + 0x5000)) = 2;
  }
}
