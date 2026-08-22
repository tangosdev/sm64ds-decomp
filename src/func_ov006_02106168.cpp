//cpp
// func_ov006_02106168 at 0x02106168 (ov006), 0x238 bytes.
// dScMgPanel_c: the panel LAYOUT PICKER. Not a state -- vtable slot 0
// (InitResources, func_ov006_021073b0, bl at 0x021077a4) and vtable slot 18
// (state reset, func_ov006_021071fc, bl at 0x0210729c) both call it directly,
// and func_ov006_021067a4 zeroes the two panel arrays just before it runs.
//
// It deals a board and then re-deals until the board is worth playing:
//   * every panel's position comes from data_ov006_0213dd4c (x) and
//     data_ov006_0213dd58 (y), both indexed by [c+0x4cbc]-4 -- the board side
//     length minus four -- as u16 and shifted 12 into the fixed-point words at
//     c+0x4cc4+i*4 and c+0x4d54+i*4;
//   * every panel's CURRENT face (c+0x4f1e+i) and TARGET face (c+0x4f42+i)
//     come from ONE OF TWO face-set tables chosen by vtable slot 35, the mode
//     gate dScMgBase_c spells as `(this[2] & 0xff) != 0`: nonzero takes
//     data_ov006_0213ded0, zero takes data_ov006_0213e070, both indexed by the
//     face-set number func_ov006_021063a0 rolled into c+0x4cb4;
//   * c+0x4cc0 random flip moves are applied through func_ov006_02106080 and
//     recorded in c+0x4fae, with c+0x4fde as the write cursor;
//   * the deal is REJECTED and repeated unless at least one panel has current
//     != target, all the recorded move indices are distinct, and
//     func_ov006_02106664 agrees the move set is not one of the last five.
//
// THREE mwcc levers were needed and each is a real property of the ROM code.
// The panel-count reload at the tail of the first loop is what keeps that
// count live in a register into the second loop's multiply (0x02106280 is
// `mul r0, r6, r0` against the register the loop bound was last read into),
// and it is that extra live value which forces the constant zero onto the
// stack slot at 0x02106180. The counter pointer is assigned inside the second
// loop so the address lands in that loop's preheader (0x0210626c) rather than
// at the top of the function. The per-branch `char *o = c + i` is what gives
// the store base its own register instead of reusing the face table's.
#include "types.h"
#define RND ((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)

struct O
{
  virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
  virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
  virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
  virtual void v32(); virtual void v33(); virtual void v34();
  virtual int m8c();
};

extern "C" {

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u16 *data_ov006_0213dd4c[];
extern u16 *data_ov006_0213dd58[];
extern u8 *data_ov006_0213ded0[];
extern u8 *data_ov006_0213e070[];
extern void func_ov006_02106080(char *c, int x);
extern int func_ov006_02106664(char *c);

#pragma opt_common_subs off
#pragma opt_strength_reduction off
void func_ov006_02106168(char *c)
{
  int v;
  int i;
  int m;
  int j;
  int bad;
  int dup;
  int a;
  int b;
  int k;
  int n;
  u8 *p;

  do
  {
    *(u8 *)(c + 0x4fde) = 0;
    n = *(int *)(c + 0x4cb8);
    for (i = 0; i < n; i++)
    {
      int row = *(int *)(c + 0x4cbc) - 4;
      int fs = *(int *)(c + 0x4cb4);
      *(int *)(c + i * 4 + 0x4cc4) = data_ov006_0213dd4c[row][i] << 12;
      *(int *)(c + i * 4 + 0x4d54) = data_ov006_0213dd58[row][i] << 12;
      if (((O *)c)->m8c() != 0)
      {
        char *o = c + i;
        *(u8 *)(o + 0x4f1e) = data_ov006_0213ded0[fs][i];
        *(u8 *)(o + 0x4f42) = data_ov006_0213ded0[fs][i];
      }
      else
      {
        char *o = c + i;
        *(u8 *)(o + 0x4f1e) = data_ov006_0213e070[fs][i];
        *(u8 *)(o + 0x4f42) = data_ov006_0213e070[fs][i];
      }
      n = *(int *)(c + 0x4cb8);
    }
    m = *(int *)(c + 0x4cc0);
    for (j = 0; j < m; j++)
    {
      p = (u8 *)(c + 0x4fde);
      v = (int)((n * RND) >> 15);
      func_ov006_02106080(c, v);
      *(u8 *)(c + *(u8 *)(c + 0x4fde) + 0x4fae) = (u8)v;
      *p += 1;
    }
    bad = 0;
    for (k = 0; k < *(int *)(c + 0x4cb8); k++)
    {
      if (*(u8 *)(c + k + 0x4f1e) != *(u8 *)(c + k + 0x4f42))
      {
        bad++;
        break;
      }
    }
    dup = 0;
    for (a = 0; a < m; a++)
    {
      for (b = a + 1; b < m; b++)
      {
        if (*(u8 *)(c + a + 0x4fae) == *(u8 *)(c + b + 0x4fae))
        {
          dup++;
          break;
        }
      }
    }
    if (dup != 0)
      bad = 0;
  } while (func_ov006_02106664(c) == 0 || bad == 0);
}

}
