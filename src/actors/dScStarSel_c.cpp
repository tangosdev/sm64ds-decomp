//cpp
/* dScStarSel_c -- the star-select screen, ov003. NINE of the class's
 * fifteen functions; see PARTIAL FOLD below for why the other six stay in
 * their own files.
 *
 * The screen between the level select and the level itself: a row of star
 * plates for the picked course, a character strip along the bottom, and a
 * cursor the D-pad walks across both. What is written here is the whole
 * drawing half of that -- Render (slot 9), the empty OnPendingDestroy
 * (slot 12), and the seven free helpers Render and Behavior drive.
 * .text 0x020adec0..0x020af038, 4472 bytes, one object, byte-identical to
 * retail through the real link.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * PARTIAL FOLD, and exactly where the line falls. The class occupies one
 * contiguous linker run, 0x020addfc..0x020b0580, fifteen functions. Two
 * separate things keep six of them out of this file:
 *
 * - Behavior (slot 6, 0x020af038..0x020af86c) does not match. It is a
 *   long-standing near-miss and it did not close when folded in here; the
 *   residual is register allocation only, no schedule change. It sits in
 *   the MIDDLE of the run, and a delinks entry carries exactly one .text
 *   range, so a licensed range cannot skip it. That splits the run in two
 *   and this file is the larger side.
 * - The destructor pair (D1 0x020addfc, D0 0x020ade54) cannot come along
 *   either, and the reason is worth recording. The cartridge places D1
 *   BELOW D0. mwccarm only emits them in that order when the destructor is
 *   defined inline in the class body AND the same TU defines the class's
 *   key function -- here InitResources, the first declared non-inline
 *   virtual. Defined out of line instead, mwccarm emits D0 before D1 and
 *   also emits a base-object D2 the cartridge does not carry. So D1/D0 can
 *   only be folded together with InitResources, which lives on the far
 *   side of Behavior. Measured both ways: with Behavior's draft body in
 *   place, the whole fifteen-function file emits every function in exact
 *   ROM-ascending order with no D2 at all. Closing Behavior therefore
 *   collapses all six leftovers at once -- it is the single blocker.
 *
 * Still in their own files: _ZN12dScStarSel_cD1Ev, _ZN12dScStarSel_cD0Ev,
 * Behavior, CleanupResources, InitResources and the factory
 * dScStarSel_c_classInit.
 *
 * #pragma opt_strength_reduction off is FILE-GLOBAL and last-wins in
 * mwccarm 2004/b56, not positional, so it applies to every function here
 * and there is no way to scope it to one. It is not decoration: of the
 * four cells of {strength reduction, loop invariants} x {on, off}, this is
 * the only one in which all of these functions reproduce. Leaving both at
 * their defaults, or turning loop invariants off as well, costs Render 0x10
 * bytes of frame. Bracketing it around a single member does nothing --
 * measured; the last directive in the file wins for the whole file.
 *
 * common.h MUST precede dScStarSel_c.h. With the order reversed, Render
 * builds 0x18 bytes long. That is the only include-order constraint found.
 *
 * deslop leftovers:
 * - The seven func_ov003_* helpers are written free, and those names are
 *   address-derived analysis labels, not recovered spellings: the image
 *   preserves no linker symbol table and RTTI supplies class identities,
 *   not function names (notes/tu-promotion-conventions.md section 1,
 *   notes/symbol-name-provenance.md). What IS measured is that every
 *   inbound relocation to each of them comes from inside this class's own
 *   run and no other module reaches any of them -- evidence to narrow
 *   ownership on later, not proof the originals were free functions.
 * - Where two legacy shards declared the same object with different
 *   shapes, one spelling had to win for the merged file, and the winner is
 *   the one the surviving call sites already hold: data_02092110 and
 *   data_02092128 as scalars, data_020a0e58 as u16[], data_020a0e5a as
 *   u16[][2] (the stride-carrying form -- the flat u16[i*2] spelling costs
 *   func_ov003_020ae358 seventeen words), OAM::TIMES as a scalar, and
 *   func_ov003_020ae1a4's first parameter as char*. Every one of these is
 *   the same address arithmetic written differently; none changes a byte.
 * - OAM::Render and the G3i/G2/GX entry points stay mangled with local
 *   shadow parameter types. The ROM names carry by-value class parameters
 *   that mwccarm passes differently at the call site, so declaring the
 *   true types breaks the byte match (notes/mwccarm-codegen.md 6az).
 *   Carried from the legacy sources unchanged.
 * - The FB/FH/FW/COS macros are the legacy shards' own this-relative field
 *   accessors, kept because the scalar layout below dScStarSel_c's Model
 *   pair has not been recovered field-by-field yet. See dScStarSel_c.h.
 */

#pragma opt_strength_reduction off
#include "common.h"
#include "dScStarSel_c.h"
#include "decl_common.h"
#include "OAM.h"
#include "SaveData.h"

/* Field accessors and externs the nine functions below use, reconciled to
 * one spelling each -- see "deslop leftovers" above for which spelling won
 * and why. Everything reachable through a real project header is reached
 * that way; what stays local is what no header in the tree declares yet,
 * or what a real header's true types would change the codegen of. */

#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]

extern "C" {
extern unsigned char data_ov003_020b169c[];
extern unsigned short data_ov003_020b16ac[];
extern void func_02012790(int a);
extern int func_ov003_020adec0(char *c, unsigned int r6);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned short data_020a0e5a[][2];
int IsStarCollectedInLevel(s8 levelID, s32 starID);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void *, int, int, int, int, int, int, int, int);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int, int, int, int, int, int, int, void *);
void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(const void *, const void *, const void *, int, Matrix4x3 *);
void _Z13CopyToViewMatPK9Matrix4x3(const Matrix4x3 *);
void Matrix4x3_FromTranslation(Matrix4x3 *, s32, s32, s32);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, s32, s32, s32);
extern s8 data_02092110;
extern s16 data_02082214[];
extern s32 data_020a0db0;
extern u8 data_0209caa0[];
extern Matrix4x3 data_020a0e68;
extern void *_ZN3OAM7NUMBERSE[];
extern u8 data_ov001_020ab938;
extern u8 data_ov001_020ab940;
extern void *data_ov001_020abb18[];
extern u8 data_ov001_020abd78;
extern u8 data_ov001_020abd80;
extern u8 data_ov001_020abb34;
extern u8 data_ov001_020abb54;
extern u8 data_ov001_020abb74;
extern u8 data_ov001_020abb94;
extern void *data_ov001_020abcb4[];
extern u8 data_ov001_020abbb4;
extern u8 data_ov001_020abbf4;
extern u8 data_02092128;
extern u8 data_020a0de8[][4];
extern u8 data_020a0de9[][4];
extern u16 data_020a0e58[];
extern u8 data_0209caa0[0x50];
/* SaveData::IsCharacterUnlocked was declared three ways across the legacy
 * shards -- `int (unsigned int)`, the same with the parameter named, and
 * `u32 (u32)`. u32 IS unsigned int here, so the parameter type never
 * actually differed; only the return spelling did, and every call site in
 * this file consumes it as `!= 0`. The declaration above is the one they
 * all already held. */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN12dScStarSel_c6RenderEv, 0x020ae6f4, size 0x944 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScStarSel_c6RenderEv
// Real dScStarSel_c method over include/dScStarSel_c.h, vtable slot 9.
//
// The two-digit level-number block was the last residue (13 words): the cartridge
// materialises the tens-digit x (mov r5,#0x77) right before the OAM::Render argument
// copy, and starts the /10 magic-multiply chain one instruction earlier. Writing the
// test in its POSITIVE sense -- `if (v >= 10) { render tens; c += 9; } else c = 0x7c;`
// instead of `if (v < 10) c = 0x7c; else { ... }` -- puts the tens branch first in the
// linearisation and the whole 13-word argument-setup block falls into the cartridge's
// order. The two branches are otherwise identical, so this is a source-shape fact and
// not a code change.
s32 dScStarSel_c::Render()
{
    s32 x;
    s32 i;
    s32 idx;
    Model *m;
    s32 yoff;
    Matrix4x3 mtx;

    func_ov003_020ae238((char *)this);
    func_ov003_020ae0b0((char *)this);

    if (SublevelToLevel(data_02092110) < 0xf) {
        u8 v;
        s32 c;
        func_ov003_020adfc8((char *)this);
        v = SublevelToLevel(data_02092110) + 1;
        if (v <= 0xf) {
            if (v >= 10) {
                OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[v / 10], c = 0x77, 0x7a, 8, -1, 0);
                c += 9;
            } else {
                c = 0x7c;
            }
            OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[v % 10], c, 0x7a, 8, -1, 0);
        }
    }

    if (SublevelToLevel(data_02092110) <= 0xe) {
        _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(data_02082214[0x16], data_02082214[0x17], 0x1555, 0x1000, 0x1388000, 0x1000, 1, 0);
        _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(&data_ov003_020b132c, &data_ov003_020b1338, &data_ov003_020b1344, 1, &mtx);
        _Z13CopyToViewMatPK9Matrix4x3(&mtx);

        x = -(((s32)FB(this, 0x114) - 1) * 0x18000 / 2);
        for (i = 1; i <= FB(this, 0x114); i++) {
            Matrix4x3_FromTranslation(&data_020a0e68, x, 0x28000, 0);
            if (i - 1 == FB(this, 0x115)) {
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(data_020a0db0 * 0x300));
                Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, 0x1400, 0x1400, 0x1400);
            }
            idx = (IsStarCollectedInLevel(data_02092110, i) == 0) ? 1 : 0;
            m = &models[idx];
            m->mat4x3 = data_020a0e68;
            m->Render(0);
            x += 0x18000;
        }

        for (i = 0; i < FB(this, 0x114); i++) {
            if ((FB(this, 0x131) >> i) & 1) {
                if (i == FB(this, 0x115)) {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab938, FB((u8 *)this + i, 0x11a), 0x18);
                } else {
                    OAM::RenderSub((OamAttr *)&data_ov001_020ab940, FB((u8 *)this + i, 0x11a), 0x18);
                }
            }
            OAM::Render(0, (OamAttr *)data_ov001_020abb18[i], FB((u8 *)this + i, 0x11a), 8, -1, -1, 0);
        }

        if (FB(this, 0x135) != 0) {
            if (FB(this, 0x133) == 0) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abd78, FB((u8 *)this + FB(this, 0x115), 0x11a), 6);
            } else if (FB(this, 0x133) == 1) {
                u8 *sel = (u8 *)this + func_ov003_020adf50((char *)this);
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, FB(sel, 0x124) - 0x24, FB(sel, 0x128) - 8);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abd80, 0x50, FB(this, 0x12b) + 8);
            }
        }

        if (FB(this, 0x139) != 0) {
        if (FB(this, 0x139) == 1) {
            switch (FB(this, 0x137)) {
            case 0:
                FW(this, 0x50) += 0x400;
                if (FW(this, 0x50) >= 0x1800) {
                    FW(this, 0x50) = 0x1800;
                    FB(this, 0x137) = 1;
                }
                break;
            case 1:
                FW(this, 0x50) -= 0x400;
                if (FW(this, 0x50) <= 0x1000) {
                    FW(this, 0x50) = 0x1000;
                    FB(this, 0x137) = 2;
                }
                break;
            case 2:
                FW(this, 0x50) -= 0x80;
                if (FB(this, 0x119) == 0) {
                    FB(this, 0x137) = 3;
                }
                break;
            case 3:
                FW(this, 0x50) += 0x100;
                FH(this, 0x110) += 0x1000;
                break;
            }
            FW(this, 0x54) = FW(this, 0x50);
        } else if (FB(this, 0x139) == 2) {
            FW(this, 0x58) = (0x1000 - COS((s16)(FH(this, 0x112) - 0x4000))) * 10 + 0x1000;
            FW(this, 0x5c) = 0x1000;
            FH(this, 0x112) = (FH(this, 0x112) + 0x1000) & 0x7fff;
            if (FH(this, 0x112) == 0x4000) {
                FB(this, 0x13a) ^= 1;
            }
            if (FW(this, 0x60) != 0) {
                FW(this, 0x60) += 0x1000;
                if (FW(this, 0x60) >= 0x10000) {
                    FW(this, 0x60) = 0;
                }
            }
        }
        }

        switch (data_0209caa0[0x41]) {
        case 0:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb34, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 1:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb54, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 2:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb74, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            break;
        case 3:
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, &data_ov001_020abb94, 0x80, FB(this, 0x12b), -1, -1, FW(this, 0x50), FW(this, 0x54), (u16)FH(this, 0x110), -1);
            for (i = 0; i < 3; i++) {
                yoff = 0;
                if (FB(this, 0x132) == i && FB(this, 0x118) != 0) {
                    yoff = 3;
                }
                if (SaveData::IsCharacterUnlocked(i) != 0) {
                    if (FB(this, 0x139) == 2) {
                        if (i == func_ov003_020adf50((char *)this)) {
                            if (FB(this, 0x13a) == 0) {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[i + 3], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                            } else {
                                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[FB((u8 *)this + (i ^ ((i != 0) ? 3 : 0)), 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x58), FW(this, 0x5c), 0, -1);
                            }
                        } else if (FW(this, 0x60) != 0) {
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[i + 3], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x60), FW(this, 0x60), 0, -1);
                            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, data_ov001_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), FB((u8 *)this + i, 0x128), -1, -1, FW(this, 0x60), FW(this, 0x60), 0, -1);
                        }
                    } else {
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[i + 3], FB((u8 *)this + i, 0x124), yoff + FB((u8 *)this + i, 0x128));
                        OAM::RenderSub((OamAttr *)data_ov001_020abcb4[FB((u8 *)this + i, 0x12c)], FB((u8 *)this + i, 0x124), yoff + FB((u8 *)this + i, 0x128));
                    }
                }
            }
            break;
        }

        if (FB(this, 0x130) <= 1 || data_0209caa0[0x41] != 3) {
            if (SublevelToLevel(data_02092110) <= 0xe) {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbb4, 0x80, (FH(this, 0x10a) >> 8) + 0xa0);
            } else {
                OAM::RenderSub((OamAttr *)&data_ov001_020abbf4, 0x80, (FH(this, 0x10a) >> 8) + 0xa0);
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN12dScStarSel_c16OnPendingDestroyEv, 0x020ae6f0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScStarSel_c16OnPendingDestroyEv
/* recovered: real C++ method */
/* dScStarSel_c::OnPendingDestroy() -- vtable slot 12. Empty override. */
void dScStarSel_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov003_020ae358, 0x020ae358, size 0x398 */
/* -------------------------------------------------------------------------- */
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
extern "C" {  /* .c-derived member: C linkage for the whole block */
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
    if (SaveData::IsCharacterUnlocked(i) != 0)
    {
      int ri = data_020a0e40;
      if (((unsigned short) ((data_020a0de8[data_020a0e40][2] - (*((unsigned char *) ((c + i) + 0x124)))) + 0x18)) < 0x30)
      {
        if (((unsigned short) ((data_020a0de8[ri][3] - (*((unsigned char *) ((c + i) + 0x128)))) + 0x18)) < 0x2b)
        {
          *((unsigned char *) (c + 0x133)) = 1;
          *((unsigned char *) (c + 0x134)) = (unsigned char) func_ov003_020adec0(c, i);
          data_02092128 = (unsigned char) i;
          data_02092114 = (unsigned char) i;
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
    unsigned short ctrl = data_020a0e58[0];
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
        if (((data_020a0e58[1] & 0x20) != 0) || (timer == 0))
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
        if (((data_020a0e58[1] & 0x10) != 0) || (timer == 0))
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov003_020ae238, 0x020ae238, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020ae238
extern "C" {  /* .c-derived member: C linkage for the whole block */
int SublevelToLevel(int i);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int x, int y, int a, int cc, int fx, int t, int e, int f);
void func_ov003_020ae1a4(char *sl, int r);
extern signed char data_0209f2f4[];
extern void *_ZN3OAM10LIFE_ICONSE[];
extern void *_ZN3OAM5TIMESE;
extern void *_ZN3OAM7NUMBERSE[];
void func_ov003_020ae238(char *sl)
{
  int sb;
  int r8;
  if (SublevelToLevel(data_02092110) >= 0xf)
  {
    r8 = 0xa0;
    sb = 0x50;
  }
  else
  {
    sb = 0x10;
    r8 = 0xac;
  }
  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM10LIFE_ICONSE[*((unsigned char *) (sl + 0x116))], sb, r8 + 8, -1, -1, 0x1000, 0x1000, 0, -1);
  OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, sb + 0x10, r8 + 8, -1, -1, 0);
  func_ov003_020ae1a4(sl, (unsigned short) data_0209f2f4[0]);
  {
    int i = 0;
    sb += 0x18;
    do
    {
      signed char d = *((signed char *) ((sl + i) + 0x121));
      if (d >= 0)
      {
        OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], sb, r8, 8, -1, 0);
        sb += 9;
      }
      i++;
    }
    while (i < 3);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov003_020ae1a4, 0x020ae1a4, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020ae1a4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020ae1a4(char *c, int sb)
{
    int found = 0;
    int one = 1;
    int m1 = -1;
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short d = data_ov003_020b16ac[i];
        int q = sb / d;
        if (q == 0 && found == 0 && i != 2) {
            (c + i)[0x121] = (char)m1;
        } else {
            found = one;
            (c + i)[0x121] = (char)q;
        }
        sb = (unsigned short)(sb % d);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov003_020ae0b0, 0x020ae0b0, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020ae0b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int SublevelToLevel(int i);
unsigned char NumStars(void);
void func_ov003_020ae1a4(char *sl, int r);
extern void *_ZN3OAM7NUMBERSE[];
extern void *_ZN3OAM5TIMESE;
extern void *_ZN3OAM10POWER_STARE;
void func_ov003_020ae0b0(char *sl)
{
  int sb;
  int r8;
  if (SublevelToLevel(data_02092110) >= 0xf)
  {
    r8 = 0xa0;
    sb = 0xb8;
  }
  else
  {
    r8 = 0xac;
    sb = 0xf4;
  }
  func_ov003_020ae1a4(sl, NumStars());
  {
    int i = 2;
    do
    {
      signed char d = *((signed char *) ((sl + i) + 0x121));
      if (d >= 0)
      {
        OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], sb, r8, 8, -1, 0);
        sb -= 9;
      }
      i--;
    }
    while (i >= 0);
  }
  OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, sb, r8 + 8, -1, -1, 0);
  OAM::Render(0, (OamAttr *)&_ZN3OAM10POWER_STARE, sb - 0x10, r8 + 8, -1, -1, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov003_020adfc8, 0x020adfc8, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020adfc8
extern "C" {
extern int SublevelToLevel(int i);
extern void func_ov003_020ae1a4(char* sl, int r);
extern void* _ZN3OAM7NUMBERSE[];
extern void* _ZN3OAM4COINE[];
void func_ov003_020adfc8(char* sl) {
    int sb = 0xb8;
    int lvl = SublevelToLevel(data_02092110);
    int coin = SaveData::GetCoinRecord(lvl);
    func_ov003_020ae1a4(sl, coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = *(signed char*)(sl + i + 0x121);
        if (d >= 0) {
            OAM::Render(0, (OamAttr *)_ZN3OAM7NUMBERSE[d], sb, 0x4c, 8, -1, 0);
            sb -= 9;
        }
    }
    OAM::Render(0, (OamAttr *)&_ZN3OAM5TIMESE, sb, 0x54, -1, -1, 0);
    OAM::Render(0, (OamAttr *)_ZN3OAM4COINE, sb - 0x10, 0x4c, -1, -1, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov003_020adf50, 0x020adf50, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020adf50
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov003_020adf50(char* c){
  unsigned char m = *(unsigned char*)(c+0x130);
  if(m == 4){
    return data_ov003_020b169c[*(unsigned char*)(c+0x134)];
  }
  if(m >= 2){
    int r5 = 0;
    int r4 = 0;
    for(; r4 < 3; r4++){
      if(SaveData::IsCharacterUnlocked((unsigned int)r4) != 0){
        if(r5 == *(unsigned char*)(c+0x134)) return r4;
        r5++;
      }
    }
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov003_020adec0, 0x020adec0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020adec0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov003_020adec0(char* c, unsigned int r6){
  unsigned char m = *(unsigned char*)(c+0x130);
  if(m == 4){
    unsigned char* p = data_ov003_020b169c;
    int r0;
    for(r0 = 0; r0 < 3; r0++){
      if(r6 == *p) return r0;
      p++;
    }
  } else if(m >= 2){
    int r5 = 0;
    int r4;
    for(r4 = 0; r4 < 3; r4++){
      if(SaveData::IsCharacterUnlocked((unsigned int)r4) != 0){
        if((unsigned int)r4 == r6) return r5;
        r5++;
      }
    }
  }
  return 0;
}
}
