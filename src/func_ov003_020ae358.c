// @symbol func_ov003_020ae358
/* recovered: the per-frame update for the ov003 character-select cursor.
 *
 * data_020a0e40 selects the active record in the 4-byte-stride tables at
 * data_020a0de8..deb. If byte 0 of that record is set and byte 1 (data_020a0de9)
 * says the slot is unlocked, the cursor tests whether the touch point at c+0x12b
 * is inside the record's box; a hit arms the "picked" state at c+0x133/0x132,
 * seeds the timers at c+0x118/0x119 and plays sound data_0209caa0[0x41] + 0x3c.
 *
 * Failing that, the three unlocked characters are scanned in order: for each i,
 * the touch point at (c+i)+0x124 / +0x128 is tested against bytes 2 and 3 of the
 * same record, and a hit records the character index in c+0x132, data_02092128
 * and data_02092114 before playing the same sound.
 *
 * With no record active (sect2) the d-pad half of the control word data_020a0e58
 * moves the selection: the repeat timer at c+0x106 counts down, and while it is
 * zero a held left/right steps c+0x134 within [0, c+0x130 - 2] and reloads the
 * timer with 0x10 or 8 depending on data_020a0e5a.
 *
 * Codegen note: the stride belongs in the TYPE, and the INDEX is what gets named.
 * The 4-byte records are declared `[][4]` so each read refolds its own scale
 * (`add r2, r4, r1, lsl #2`); flattening data_020a0dea/deb to a bare `[]` with an
 * explicit `* 4` costs 17 words, and the same is true of data_020a0e5a. The last
 * six words were a register transposition between the `c + i` and record-row
 * address temps, and what closed it was naming the INDEX (`int ri`) for the second
 * record read instead of naming the row POINTER: a named row pointer welds both
 * reads onto one address temp and inverts the r2/r3 assignment, while the named
 * index leaves each read to fold its own scale and hands the row temp r2.
 */
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
      int ri = data_020a0e40;
      if (((unsigned short) ((data_020a0de8[data_020a0e40][2] - (*((unsigned char *) ((c + i) + 0x124)))) + 0x18)) < 0x30)
      {
        if (((unsigned short) ((data_020a0de8[ri][3] - (*((unsigned char *) ((c + i) + 0x128)))) + 0x18)) < 0x2b)
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
