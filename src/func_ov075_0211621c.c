// @symbol func_ov075_0211621c
/* recovered: dScEntry_c draws the per-player rows of the VS/wireless entry menu.
 *
 * One row per joined player (data_0209fc50 of them). With d = 4 - players, the rows
 * start at x = data_ov075_0211c6e8[d] and step by 16d + 0x38, which is how the layout
 * spreads two players wider apart than four.
 *
 * Each row emits three things through OAM::Render: the player icon at y = 0x98, the
 * character-index sprite at x + 0x1c, and a three-digit score at y = 0xb0. The score is
 * the shared base at data_0209ee90+0x1d4 plus this player's delta from data_0209f358,
 * split by / 100, / 10 and % 10. Leading zeros are suppressed and the pen advances by 8
 * for a drawn digit and 4 for a skipped one, so the number stays centred. Where a call
 * returns an OAM attribute block, the low ten bits of the second word are the tile
 * index, and the digit or character index is ADDED to it rather than stored - that is
 * the `(val & ~0x3ff) | ((n + (val << 22 >> 22)) & 0x3ff)` in each block.
 */
// NONMATCHING: 27/229 at exact size 0x394 (was 40). Every divergence is routing --
// register fields and sp displacements only; no opcode, immediate, addressing mode or
// condition differs, so the same operations run on the same values and only where they
// are held changes. Three levers took it from 40 to 27:
//   (1) the row x is not a variable -- spell every x as xbase + K + i * pitch so the
//       induction variable stays the ROM's;
//   (2) the leading-zero pen advance is an if/else over a named pen, not a ternary;
//   (3) the digit blocks keep the OAM attribute word anonymous.
// Residue is a callee-saved pair swap (sb <-> r7) plus the stack slots that follow it.
#include "types.h"
extern u8 data_0209fc50;
extern u8 data_ov075_0211c6e8[];
extern char data_ov075_0211c7c0[];
extern char data_ov075_0211c8b0[];
extern char data_ov075_0211c870[];
extern s8 data_0209f310[];
extern s16 data_0209f358[];
extern char data_0209ee90[];
extern int func_02030958(int idx);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void *attr, int x, int y, int pal, int prio, int sx, int sy, int rot, int mode);
extern int *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int a, void *attr, int x, int y, int pal, int prio, int scale, int rot);

inline int inline_fn(int arg0)
{
  return func_02030958(arg0);
}

void func_ov075_0211621c(char *c)
{
  int count = data_0209fc50;
  int stride;
  int tens;
  int hundreds;
  int i = 0;
  int d = 4 - count;
  int xbase;
  (void) c;
  xbase = data_ov075_0211c6e8[d];
  stride = (d * 16) + 0x38;
  {
    if (count > 0)
    {
      int minus1 = -1;
      int zA;
      int yIcon;
      int zC;
      int zB;
      int off14;
      int zD;
      int zE;
      int zF;
      int scale;
      int yNum;
      zE = i;
      zD = i;
      zB = i;
      off14 = 0x14;
      zF = i;
      zA = i;
      yIcon = 0x98;
      zC = i;
      scale = 0x1000;
      yNum = 0xb0;
      do
      {
        int *p;
        int pid = inline_fn(i);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zA, data_ov075_0211c7c0, xbase + i * stride, yIcon, minus1, minus1, scale, scale, zA, minus1);
        p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zC, data_ov075_0211c8b0, xbase + 0x1c + i * stride, yIcon, minus1, minus1, scale, zC);
        if (p != 0)
        {
          int *q = (int *) ((int) (p + 1));
          int val = *q;
          *q = (val & (~0x3ff)) | ((data_0209f310[pid] + (((u32) (val << 22)) >> 22)) & 0x3ff);
        }
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zB, data_ov075_0211c870, xbase + i * stride, yNum, minus1, minus1, scale, scale, zB, minus1);
        {
          int n;
          int dx = off14;
          n = (*((int *) (data_0209ee90 + 0x1d4))) + data_0209f358[pid];
          hundreds = n / 100;
          tens = (n % 100) / 10;
          if (hundreds != 0)
          {
            p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zD, data_ov075_0211c8b0, xbase + 0x14 + i * stride, yNum, minus1, minus1, scale, zD);
            if (p != 0)
            {
              int *q = (int *) ((int) (p + 1));
              int val = *q;
              *q = (val & (~0x3ff)) | ((hundreds + (((u32) (val << 22)) >> 22)) & 0x3ff);
            }
            dx = dx + 8;
          }
          else
          {
            dx = dx + 4;
          }
          if ((tens != 0) || (hundreds != 0))
          {
            p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zE, data_ov075_0211c8b0, xbase + dx + i * stride, yNum, minus1, minus1, scale, zE);
            if (p != 0)
            {
              int *q = (int *) ((int) (p + 1));
              int val = *q;
              *q = (val & (~0x3ff)) | ((tens + (((u32) (val << 22)) >> 22)) & 0x3ff);
            }
            dx = dx + 8;
          }
          else
          {
            dx = dx + 4;
          }
          p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zF, data_ov075_0211c8b0, xbase + dx + i * stride, yNum, minus1, minus1, scale, zF);
          if (p != 0)
          {
            int *q = (int *) ((int) (p + 1));
            int val = *q;
            *q = (val & (~0x3ff)) | ((((n % 100) % 10) + (((u32) (val << 22)) >> 22)) & 0x3ff);
          }
        }
        i++;
      }
      while (i < count);
    }
  }
}
