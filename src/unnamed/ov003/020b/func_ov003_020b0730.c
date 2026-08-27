extern unsigned short *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned short data_ov003_020b174c[];
void func_ov003_020b0730(void *self, int arg)
{
  unsigned char *s = (unsigned char *) self;
  int i;
  for (i = 0; i < 2; i++)
  {
    unsigned int v;
    int j;
    unsigned short *p = _ZN3G2S12GetBG1ScrPtrEv();
    p += data_ov003_020b174c[i];
    if (arg)
    {
      v = 0x1000;
    }
    else
    {
      unsigned int w;
      if ((s[0x91] == i) && (s[0x93] == 0))
        w = 0x2000;
      else
        w = 0x1000;
      v = (unsigned short) w;
    }
    for (j = 0; j < 0x20; j++)
    {
      p[0] = (p[0] & 0x3ff) | v;
      p[0x20] = (p[0x20] & 0x3ff) | v;
      p[0x40] = (p[0x40] & 0x3ff) | v;
      p[0x60] = (p[0x60] & 0x3ff) | v;
      p += 1;
    }
  }
}
