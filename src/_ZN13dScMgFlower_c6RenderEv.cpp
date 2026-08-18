//cpp
// @symbol _ZN13dScMgFlower_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgFlower_c.h"
/* dScMgFlower_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable; the third of the three slots (0, 6, 9) where
 * Flower's table differs from dScMgSingle3DBase_c's. The four addresses that once
 * carried `recovered name: dScMgFlower_c_*` for slots 2/5/7/10 were the parent's
 * and moved up in commit 4f7406b9c -- see include/dScMgFlower_c.h.
 *
 * Scrolls the two background layers off a sine table (0x5ff4 is the phase, stepped
 * 0xc0 a frame), draws the round's banner sprite, then one sprite per live flower.
 * The flower being dragged -- index 0x5fc8 -- is drawn with a different palette
 * argument, which is how the player sees which one is held.
 *
 * WAS A C99 FILE, so the three declarations move inside `extern "C"`; in C++ they
 * would mangle and resolve to nothing. */

extern "C" {
extern s16 data_02082214[];
void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020af770(void *a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
}

struct S2 { int a, b; };

s32 dScMgFlower_c::Render()
{
    char *self = (char *)this;
    int i;
    u8 (*barr)[0x20] = (u8 (*)[0x20])self;
    int (*iarr)[8] = (int (*)[8])self;
    u16 (*harr)[0x10] = (u16 (*)[0x10])self;

    *(u16*)(self + 0x5ff4) += 0xc0;
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = v + 0x80;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, off + 8);
    }
    {
        int v = data_02082214[(*(u16*)(self + 0x5ff4) >> 4) << 1];
        int t = 0x80 - v;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg3Offset(off, off);
    }
    func_ov004_020afdd0(data_ov006_0213ab94[*(int*)(self + 0x5fec)], 0x80, 0x60, -1, 1);

    for (i = 0; i < 0x16; i++) {
        if (barr[i][0x4f38] != 0) {
            struct S2 loc = *(struct S2*)data_ov006_0213abe0;
            int a4 = 1;
            loc.a = (loc.a & 0xc1fffcff) | 0x100;
            if (*(int*)(self + 0x5fc8) == i)
                a4 = 0;
            func_ov004_020af770(
                &loc,
                iarr[i][0x13cf] >> 12,
                iarr[i][0x13d0] >> 12,
                -1,
                a4,
                0x1000,
                (u16)(s16)(harr[i][0x27aa] + 0x8000));
        }
    }

    func_ov006_020c3b2c(self + 0x4660);
    func_ov006_020c3bf4(self + 0x51f8);
    return 1;
}
