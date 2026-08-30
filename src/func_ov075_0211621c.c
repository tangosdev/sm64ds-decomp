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
// NONMATCHING: register allocation and frame layout (div=40 at the canonical mwccarm
// 2004/b56). Counts as decompiled, not matched.
//
// Every one of the 40 divergent words is routing, verified word by word rather than by
// eye: 30 differ only in register fields and 10 only in an sp displacement, and none
// changes an opcode, an immediate, an addressing mode or a condition. The implied
// correspondence is a single pair swap in each file - registers sb <-> r7, and stack
// slots 0x20 <-> 0x24 - plus a local r0/r1 exchange inside the divide-by-100 block. So
// the same operations run on the same values; only where they are held differs.
//
// That pair swap is the floor. The hundreds digit is memory-homed BELOW the loop counter
// in the ROM's frame, and mwccarm sorts a memory-homed aggregate above every spilled
// scalar, so the two slots cannot be exchanged from C. Mechanism and the levers that are
// measured-closed: notes/mwccarm-codegen.md 6bl.
//
// The shape carries matching crutches, which are deliberate and not natural source: the
// one-element array `hv` memory-homes the hundreds digit, the block of `zA`..`zF`,
// `minus1`, `off14`, `scale`, `yIcon` and `yNum` reproduces the ROM's hoisted constant
// block (6bh), and `inline_fn` forces the index call to inline.
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
  int hv[1];
  int i = 0;
  int d = 4 - count;
  int xbase;
  (void) c;
  xbase = data_ov075_0211c6e8[d];
  stride = (d * 16) + 0x38;
  {
    if (count > 0)
    {
      int xoff = i;
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
      zE = xoff;
      zD = xoff;
      zB = xoff;
      off14 = 0x14;
      zF = xoff;
      zA = xoff;
      yIcon = 0x98;
      zC = xoff;
      scale = 0x1000;
      yNum = 0xb0;
      do
      {
        int *p;
        int pid = inline_fn(i);
        int x = xbase + xoff;
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zA, data_ov075_0211c7c0, x, yIcon, minus1, minus1, scale, scale, zA, minus1);
        p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zC, data_ov075_0211c8b0, (xbase + 0x1c) + xoff, yIcon, minus1, minus1, scale, zC);
        if (p != 0)
        {
          int *q = (int *) ((int) (p + 1));
          int val = *q;
          *q = (val & (~0x3ff)) | ((data_0209f310[pid] + (((u32) (val << 22)) >> 22)) & 0x3ff);
        }
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zB, data_ov075_0211c870, x, yNum, minus1, minus1, scale, scale, zB, minus1);
        {
          int n;
          int tens;
          int dx = off14;
          n = (*((int *) (data_0209ee90 + 0x1d4))) + data_0209f358[pid];
          hv[0] = n / 100;
          n = n % 100;
          tens = n / 10;
          if (hv[0] != 0)
          {
            p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zD, data_ov075_0211c8b0, (xbase + 0x14) + xoff, yNum, minus1, minus1, scale, zD);
            if (p != 0)
            {
              int *q = (int *) ((int) (p + 1));
              int val = *q;
              *q = (val & (~0x3ff)) | ((hv[0] + (((u32) (val << 22)) >> 22)) & 0x3ff);
            }
            dx = dx + 8;
          }
          else
          {
            dx = dx + 4;
          }
          if ((tens != 0) || (hv[0] != 0))
          {
            p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zE, data_ov075_0211c8b0, (xbase + dx) + xoff, yNum, minus1, minus1, scale, zE);
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
          p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zF, data_ov075_0211c8b0, (xbase + dx) + xoff, yNum, minus1, minus1, scale, zF);
          if (p != 0)
          {
            int *q = (int *) ((int) (p + 1));
            int val = *q;
            *q = (val & (~0x3ff)) | (((n % 10) + (((u32) (val << 22)) >> 22)) & 0x3ff);
          }
        }
        xoff += stride;
        i++;
      }
      while (i < count);
    }
  }
}
