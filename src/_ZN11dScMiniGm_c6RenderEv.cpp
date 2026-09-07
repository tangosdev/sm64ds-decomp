//cpp
// @symbol _ZN11dScMiniGm_c6RenderEv
/* recovered: real C++ method against include/dScMiniGm_c.h.

   dScMiniGm_c::Render -- vtable slot 9, ov005 0x020c0b04, 0x434 bytes. The minigame
   select screen: the character tabs down the left, a "new game" icon per tab whose
   nine games (ids base + 4*j) has one both unlocked and unseen, the page arrows and
   the exit button with their bounce scales.

   Rewritten from the disassembly. Credit for the earlier unmatched draft that sat here
   before stays with it: it came in with the minigame class wave (#1741), and its
   extern set and slot identification are what this rewrite started from.
   The flat draft forced the inner loop's two induction registers by hand; they
   fall out of the real source once the tab table is declared what the ROM says it
   is: data_ov005_020c2250 is the first word of ov005's .rodata, so it is `const`,
   and with a const table the compiler keeps ONE load of data_ov005_020c2250[i]
   across the two calls (an anonymous temp, r4) while strength reduction still
   gives the two textual `+ j * 4` uses their own induction registers (r6 for the
   first, r5 + r4 for the second). A named `base` local instead unifies both uses
   into one induction and colours r7. The
   second call spells `j * 4 + table[i]` because the cartridge adds the induction
   temp first. y1 is declared before y0 so y1 keeps fp and y0 spills. */
#include "dScMiniGm_c.h"

struct OamAttr;

extern "C" {
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, OamAttr *attr, int x, int y, int pal, int prio, int sx, int sy, int c, int d);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, OamAttr *attr, int x, int y, int pal, int prio, int s, int c);
extern void _ZN3OAM9RenderSubEP7OamAttriiii(OamAttr *attr, int x, int y, int pal, int prio);
extern int func_ov005_020c00b4(void *self, int id);
extern int func_ov005_020c007c(void *self, int id);
extern int func_ov005_020c00e4(void *self);
extern void func_ov005_020c1130(void *self);
extern void func_ov005_020c0f38(void *self);

extern u8 data_0209b300;
extern u8 data_0209b304;
extern const int data_ov005_020c2250[];
extern OamAttr *data_ov005_020c2c28[];
extern OamAttr *data_ov005_020c2e88;
extern OamAttr *data_ov005_020c2ea4[];
extern OamAttr *data_ov005_020c2f60[];
extern OamAttr *data_ov005_020c2f88;
extern OamAttr *data_ov005_020c2fcc;
}

s32 dScMiniGm_c::Render()
{
    int i;
    int blink;
    int y1;
    int y0;
    int t;
    int scale;

    if (data_0209b300 == 1) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2f88, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2fcc, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
    }

    y0 = 0x18;
    y1 = 0x28;
    for (i = 0; i < 4; i++) {
        int k = i;
        int j;
        if (mIconBlinkPhase >= 0x20)
            blink = 1;
        else
            blink = 0;
        for (j = 0; j < 9; j++) {
            if (func_ov005_020c00b4(this, data_ov005_020c2250[i] + j * 4) != 0 && func_ov005_020c007c(this, j * 4 + data_ov005_020c2250[i]) != 0) {
                _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f60[blink], 0x18, y1, -1, 0);
                break;
            }
        }
        if (mGroupBase != data_ov005_020c2250[i])
            k += 4;
        _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2c28[k], 0x18, y0, -1, 0);
        y1 += 0x30;
        y0 += 0x30;
    }

    if (mPageFlipped == 0) {
        if (data_0209b304 != 0) {
            t = mPrevPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - mArrowBobPhase / 2, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - (0x40 - mArrowBobPhase) / 2, 0x60, -1, 0, scale, 0);
        } else if (func_ov005_020c00e4(this) != 0) {
            t = mNextPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], mArrowBobPhase / 2 + 0xe0, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], (0x40 - mArrowBobPhase) / 2 + 0xe0, 0x60, -1, 0, scale, 0);
        }
    }

    func_ov005_020c1130(this);
    func_ov005_020c0f38(this);

    t = mExitTimer;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 12) / 16) + 0x1000;
    else
        scale = (((0x1c - t) << 12) / 16) + 0x1000;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2e88, 0xe8, 0xa0, -1, 0, scale, 0);
    return 1;
}
