// @symbol func_ov002_020d3b9c
/* recovered: Player walk state helper (between St_Walk_Main and St_Walk_Init): steering and turn approach. */
// NONMATCHING: div 50 of 360 words. mwccarm 2004/b56, --module ov002,
// @ 0x020d3b9c size 0x5a0. Residue class: colouring.
// Draft from nearmiss/db.jsonl (stored divergence 50), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
extern int Player_ScaleByCharFactor(void *c, int a);
extern int func_ov002_020bf224(int c, int a, int b);
extern int func_ov002_020bf56c(void *c, int b);
extern void func_ov002_020bf88c(void *c);
extern int func_ov002_020c031c(void *c);
extern int func_ov002_020f035c(unsigned int sel, int r1);
extern int AngleDiff(int a, int b);
extern int __aeabi_idiv(int a, int b);
extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern void Player_AdvanceAnims(void *c);
extern void func_ov002_020d4540(void *c);
extern void func_ov002_020d454c(void *c);
extern int func_ov002_020d4d88(void *c, int a, int b);
extern void func_ov002_020c18b0(void *c, int a);
extern unsigned char data_020a0e40;
extern short data_0209f4a0[];
extern char data_0209f49c[];
extern unsigned char data_0209f4ac[];
extern unsigned char data_0209f4ae[];
extern char data_ov002_02110184;
int func_ov002_020d3b9c(char *c)
{
  int acc;
  int spd;
  s16 stick;
  int idx;
  s16 targ;
  int diff;
  acc = 0;
  idx = data_020a0e40 * 0x18;
  stick = *((s16 *) (((char *) data_0209f4a0) + idx));
  spd = acc;
  if ((stick != 0) && (((u16) ((*((u16 *) (c + 0x6ce))) & 0xc)) == 0))
  {
    *((u16 *) (c + 0x6ac)) = 4;
    if ((*((u8 *) (data_0209f4ac + idx))) == 0)
    {
      spd = Player_ScaleByCharFactor(c, 0x14000);
      acc = 0x1000;
      if (((*((u16 *) (data_0209f49c + (data_020a0e40 * 0x18)))) & 0x800) == 0)
      {
        if ((*((u16 *) (c + 0x6a4))) != 0)
        {
          spd = Player_ScaleByCharFactor(c, 0xa000);
        }
      }
      else
      {
        spd = Player_ScaleByCharFactor(c, 0x28000);
      }
    }
    else
    {
      u8 mode = *((u8 *) (data_0209f4ae + idx));
      s16 stick2 = *((s16 *) (((char *) data_0209f4a0) + idx));
      if (stick2 <= ((mode != 2) ? (0x471) : (0x555)))
      {
        spd = 0;
      }
      else
        if (stick2 <= ((mode != 2) ? (0x955) : (0xAAA)))
      {
        int t = stick2 * ((mode != 2) ? (0x24) : (0x30));
        int s = (mode != 2) ? (0xa) : (0x10);
        t = (t >> 12) - s;
        s = (mode != 2) ? (0xa) : (0x10);
        spd = t << 15;
        spd = spd / (((mode != 2) ? (0x15) : (0x20)) - s);
      }
      else
      {
        int a = Player_ScaleByCharFactor(c, 0x28000);
        int b = Player_ScaleByCharFactor(c, 0xa000);
        spd = func_ov002_020bf224((int) c, a, b);
      }
      acc = 0x1000;
      if ((*((s16 *) (((char *) data_0209f4a0) + (data_020a0e40 * 0x18)))) >= 0xf00)
      {
        acc = 0x1000;
      }
    }
    if ((*((u8 *) (c + 0x6ed))) != 0)
    {
      if (((*((u16 *) (data_0209f49c + (data_020a0e40 * 0x18)))) & 0x800) == 0)
      {
        *((u8 *) (c + 0x6ed)) = 0;
      }
      else
      {
        u8 *p = (u8 *) (((char *) c) + 0x6ED);
        *p = (u8) ((*p) - 1);
        spd = (int) (((((long long) spd) * 0x1333LL) + 0x800) >> 12);
        acc = 0x1e000;
        func_ov002_020bf88c(c);
      }
    }
    {
      int v = *((int *) (c + 0x68c));
      if (v > 0xa000)
      {
        int q = 0x6400 / (v >> 12);
        spd = (int) (((((long long) spd) * q) + 0x800) >> 12);
      }
    }
    *((int *) (c + 0x690)) = 0;
    if ((*((int *) (c + 8))) == 1)
    {
      u16 h = *((u16 *) (c + 0x6ba));
      if (((h != 0) && ((*((u8 *) (c + 0x6ea))) != 0)) && (h < 0x1e))
      {
        int a = Player_ScaleByCharFactor(c, 0x28000);
        int prod = h * a;
        {
          int cap = prod / 30;
          if (spd > cap)
          {
            spd = cap;
          }
        }
        {
          int tmp = -((0x1e - ((int) h)) << 12);
          tmp = tmp * 2;
          *((int *) (c + 0x690)) = tmp;
        }
      }
    }
    if (func_ov002_020f035c(func_ov002_020c031c(c), *((int *) (c + 0x558))) != 0)
    {
      acc = 0x1000;
    }
    targ = *((s16 *) (c + 0x6d2));
    diff = AngleDiff(*((s16 *) (c + 0x94)), targ);
    if (diff < 0)
    {
      diff = -diff;
    }
    if (diff > 0x6000)
    {
      int cur = *((int *) (c + 0x98));
      if ((cur >= Player_ScaleByCharFactor(c, 0x1c000)) && ((*((u8 *) (c + 0x703))) == 0))
      {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110184);
        Player_AdvanceAnims(c);
        return 1;
      }
      if (cur == 0)
      {
        *((s16 *) (c + 0x94)) = targ;
      }
      else
      {
        ApproachAngle((s16 *) (c + 0x94), targ, 4, 0x2000, 0x800);
      }
    }
    else
    {
      int dv;
      int step;
      int idx2;
      step = 0x800;
      idx2 = data_020a0e40 * 0x18;
      dv = 4;
      if ((*((u8 *) (data_0209f4ac + idx2))) == 0)
      {
        if ((*((s16 *) (((char *) data_0209f4a0) + idx2))) >= 0xa00)
        {
          if (AngleDiff(*((s16 *) (c + 0x94)), targ) >= 0x1000)
          {
            if ((*((u16 *) (c + 0x6f0))) < 0x18)
            {
              u16 *p = (u16 *) (((char *) c) + 0x6F0);
              *p = (u16) ((*p) + 1);
              dv = (s16) (0x10 - ((*((u16 *) (c + 0x6f0))) >> 1));
            }
          }
          else
          {
            dv = 0x10;
            if ((*((u16 *) (c + 0x6f0))) >= 2)
            {
              u16 *p = (u16 *) (((char *) c) + 0x6F0);
              *p = (u16) ((*p) - 2);
            }
          }
          step = (s16) ((0x10 - (dv - 4)) << 3);
        }
        else
          if ((*((u16 *) (c + 0x6f0))) >= 2)
        {
          u16 *p = (u16 *) (((char *) c) + 0x6F0);
          *p = (u16) ((*p) - 2);
        }
      }
      ApproachAngle((s16 *) (c + 0x94), targ, dv, 0x2000, step);
    }
    if ((*((u8 *) (c + 0x6e0))) != 0)
    {
      func_ov002_020d4540(c);
    }
  }
  else
  {
    *((u8 *) (c + 0x6ed)) = 0;
    if ((*((u8 *) (c + 0x6e0))) != 0)
    {
      acc = func_ov002_020bf56c(c, 0x2000);
      func_ov002_020bf88c(c);
    }
    else
      if ((*((u16 *) (c + 0x6ac))) == 0)
    {
      acc = ((*((u8 *) (c + 0x703))) == 0) ? (0x1000) : (0x4000);
      if ((*((int *) (c + 0x98))) < (-0xa000))
      {
        *((int *) (c + 0x98)) = -0xa000;
      }
      if ((*((u8 *) (c + 0x703))) == 0)
      {
        if ((*((int *) (c + 0x98))) > Player_ScaleByCharFactor(c, 0x10000))
        {
          func_ov002_020d454c(c);
        }
      }
    }
    if ((*((int *) (c + 0x98))) < Player_ScaleByCharFactor(c, 0xa000))
    {
      *((u16 *) (c + 0x6a4)) = 0x3c;
    }
  }
  func_ov002_020d4d88(c, spd, acc);
  func_ov002_020c18b0(c, 1);
  *((u16 *) (c + 0x90)) = 0;
  return 0;
}
