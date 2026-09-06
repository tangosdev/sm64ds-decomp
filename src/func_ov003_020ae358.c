// NONMATCHING: 6/230 at exact size 0x398. The draft that stood here was 0x394 -- four
// bytes SHORT, an incomplete reconstruction rather than a near-miss.
//
// 8 -> 6 came from the permuter and the lever is worth keeping: the FIRST of the two
// reads through `rec` is spelled inline as data_020a0de8[data_020a0e40][2] while the
// SECOND stays rec[3]. That one asymmetry fixes the order in which the two hoisted pool
// addresses (&data_020a0e40, &data_020a0de8) are loaded at +0xfc/+0x100. Inlining both
// reads, inlining only the second, or moving the `rec` declaration below the first test
// all cost 53 more words.
//
// The residue is the last six words: the two address temps `c + i` and
// &data_020a0de8[data_020a0e40] swap r2 and r3 (+0x11c..+0x148). Naming either of them
// costs 52 words; both must stay anonymous, and no ordering of the two subtractions
// moves the pair.

extern void func_02012790(int a);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int i);
extern int func_ov003_020adec0(char *c, unsigned int r6);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned char data_02092128[];
extern unsigned char data_02092114[];
extern unsigned short data_020a0e58;
extern unsigned short data_020a0e5a[][2];
void func_ov003_020ae358(char *c)
{
  int idx = data_020a0e40;
  int unlocked = 0;
  int i;
  if (data_020a0de8[idx][0] != 0)
  {
    unlocked = data_020a0de9[idx][0] != 0;
  }
  if (unlocked == 0)
  {
    goto sect2;
  }
  if ((((unsigned char) (data_020a0dea[idx][0] - 0x58)) < 0x50) && (((unsigned char) ((data_020a0deb[idx][0] - (*((unsigned char *) (c + 0x12b)))) + 0x28)) < 0x50))
  {
    *((unsigned char *) (c + 0x133)) = 2;
    *((unsigned char *) (c + 0x132)) = 3;
    *((unsigned char *) (c + 0x118)) = (unsigned char) (data_0208ee44 * 6);
    *((unsigned char *) (c + 0x139)) = 1;
    *((unsigned char *) (c + 0x119)) = 0x10;
    func_02012790(data_0209caa0[0x41] + 0x3c);
    return;
  }
  if ((*((unsigned char *) (c + 0x130))) <= 1)
  {
    return;
  }
  if (data_0209caa0[0x41] != 3)
  {
    return;
  }
  for (i = 0; i < 3; i++)
  {
    if (_ZN8SaveData19IsCharacterUnlockedEj(i) != 0)
    {
      unsigned char *rec = data_020a0de8[data_020a0e40];
      if (((unsigned short) ((data_020a0de8[data_020a0e40][2] - (*((unsigned char *) ((c + i) + 0x124)))) + 0x18)) < 0x30)
      {
        if (((unsigned short) ((rec[3] - (*((unsigned char *) ((c + i) + 0x128)))) + 0x18)) < 0x2b)
        {
          *((unsigned char *) (c + 0x133)) = 1;
          *((unsigned char *) (c + 0x134)) = (unsigned char) func_ov003_020adec0(c, i);
          data_02092128[0] = (unsigned char) i;
          data_02092114[0] = (unsigned char) i;
          *((unsigned char *) (c + 0x132)) = (unsigned char) i;
          *((unsigned char *) (c + 0x118)) = (unsigned char) (data_0208ee44 * 3);
          *((unsigned char *) (c + 0x139)) = 2;
          *((unsigned char *) (c + 0x119)) = 0x10;
          func_02012790(data_0209caa0[0x41] + 0x3c);
          return;
        }
      }
    }
  }

  return;
  sect2:
  if (data_0209caa0[0x42] == 0)
  {
    unsigned short ctrl = data_020a0e58;
    if ((ctrl & 0x30) != 0)
    {
      unsigned short timer = *((unsigned short *) (c + 0x106));
      unsigned char nr;
      if (timer != 0)
      {
        *((unsigned short *) (c + 0x106)) -= 1;
        return;
      }
      if ((*((unsigned char *) (c + 0x135))) == 0)
      {
        return;
      }
      if ((*((unsigned char *) (c + 0x133))) != 1)
      {
        return;
      }
      if ((*((unsigned char *) (c + 0x130))) < 3)
      {
        return;
      }
      nr = *((unsigned char *) (c + 0x134));
      if (ctrl & 0x20)
      {
        if ((((&data_020a0e58)[1] & 0x20) != 0) || (timer == 0))
        {
          *((unsigned short *) (c + 0x106)) = (data_020a0e5a[idx][0] & 0x20) ? (0x10) : (8);
          if ((*((unsigned char *) (c + 0x134))) != 0)
          {
            nr = nr - 1;
          }
        }
      }
      else
        if (ctrl & 0x10)
      {
        if ((((&data_020a0e58)[1] & 0x10) != 0) || (timer == 0))
        {
          *((unsigned short *) (c + 0x106)) = (data_020a0e5a[idx][0] & 0x10) ? (0x10) : (8);
          if ((*((unsigned char *) (c + 0x134))) != ((*((unsigned char *) (c + 0x130))) - 2))
          {
            nr = nr + 1;
          }
        }
      }
      if (nr == (*((unsigned char *) (c + 0x134))))
      {
        return;
      }
      *((unsigned char *) (c + 0x134)) = nr;
      func_02012790(0x12e);
      return;
    }
  }

  *((unsigned short *) (c + 0x106)) = 0;
}
