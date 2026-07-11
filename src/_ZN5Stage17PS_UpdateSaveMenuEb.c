typedef unsigned char u8;
typedef unsigned short u16;
extern u16 data_020755c8[];
extern u8 data_0209f2e0;
extern u8 data_0209f244;
extern u16 *_ZN3G2S12GetBG1ScrPtrEv(void);

void _ZN5Stage17PS_UpdateSaveMenuEb(int b)
{
  u16 v;
  int i;
  for (i = 0; i < 2; i++)
  {
    int j;
    u16 *p;
    if (b != 0)
    {
      v = 0x1000;
    }
    else
    {
      unsigned int t;
      if (data_0209f2e0 == i && data_0209f244 == 0) t = 0x2000;
      else t = 0x1000;
      v = (u16) t;
    }
    p = _ZN3G2S12GetBG1ScrPtrEv() + data_020755c8[i];
    for (j = 0; j < 10; j++)
    {
      p[0] = (p[0] & 0x3ff) | v;
      p[0x20] = (p[0x20] & 0x3ff) | v;
      p[0x40] = (p[0x40] & 0x3ff) | v;
      p[0x60] = (p[0x60] & 0x3ff) | v;
      p++;
    }
  }
}
