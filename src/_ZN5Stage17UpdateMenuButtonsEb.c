
    typedef unsigned char u8;
    typedef unsigned short u16;
    extern u8 data_0209f2b4;
    extern u16 data_0209f360[];
    extern u8 data_0209f2e0;
    extern u8 data_0209f244;
    extern u8 data_0209f2c4;
    extern u8 data_0209f248;
    extern unsigned short *_ZN3G2S12GetBG1ScrPtrEv(void);
    void _ZN5Stage17UpdateMenuButtonsEb(int b)
    {
      int i;
      for (i = 0; i < data_0209f2b4; i++)
      {
        unsigned int v;
        int j;
        u16 *p;
        p = _ZN3G2S12GetBG1ScrPtrEv() + data_0209f360[i];
        if (b == 0)
        {
          if (data_0209f2e0 == i && data_0209f244 == 0)
          {
            v = 0x2000;
          }
          else
          {
            v = 0x1000;
          }
          v = (u16) v;
        }
        else
        {
          v = 0x1000;
        }
        if (data_0209f2c4 != 0)
        {
          u8 t = (u8) (data_0209f248 + 0xf9);
          if (t <= 1)
          {
            v = (u16) (v + 0x4000);
          }
        }
        for (j = 0; j < 0x20; j++)
        {
          p[0] = (p[0] & 0x3ff) | v;
          p[0x20] = (p[0x20] & 0x3ff) | v;
          p[0x40] = (p[0x40] & 0x3ff) | v;
          p[0x60] = (p[0x60] & 0x3ff) | v;
          p++;
        }

      }

    }
