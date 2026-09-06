//cpp
// @symbol func_ov006_020e4bd4
/* recovered: dScMgCurling2_c stylus drag update, ov006 0x020e4bd4 (768 bytes).
 * Runs while the player drags a stone before the throw. With no touch in
 * the current input record it just drops the drag flags. Otherwise it moves
 * the drag point to the stylus plus the grab offset, clamps it to the rink,
 * and bails out (restoring the old point) when the stylus moved a unit or
 * less. The vertical direction-change tracker (unk_55be, unk_55a8,
 * unk_5590) plays the scrape sound on the first move and flips phases on
 * reversals; the throw angle (unk_55b2) is atan2 of the move with the
 * horizontal halved, snapped to straight up inside [0x4000,0x8000] and to
 * zero below, then averaged with the previous frame; the throw power
 * (unk_559c) follows the move length times 9 with a 0xc000 cap, rising
 * instantly and decaying by half. Finally the grab offset is recomputed
 * from the new point and a fresh read of the input index.
 *
 * Both pragmas are load-bearing. The ROM shares nothing across an extended
 * basic block that 2004/b56 would share by default: dx >> 1 is recomputed
 * after the atan2 call and the input index is re-read at the end, so
 * opt_common_subs is off, and every value the ROM does share is a named
 * local here (i4, bx/by, dy2, j-free ax/ay). opt_propagation off keeps the
 * stylus byte loads above the first store (the ROM loads both before it)
 * and puts i4 in r4. Loading by before bx, and ay before ax, colours the
 * two scratch pairs the ROM's way; the twin func_ov006_020e1854 in
 * dScMgCurling_c reads the same. */
#include "dScMgCurling2_c.h"
#pragma opt_common_subs off
#pragma opt_propagation off

extern "C" {
extern int _ZN4cstd4sqrtEy(u64 v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int id, int v);
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
}

extern "C" void func_ov006_020e4bd4(dScMgCurling2_c *self)
{
    u8 idx;
    int dx;
    int oldx;
    int oldy;
    int diff;
    u16 oldAng;
    int mag;
    int i4;

    idx = data_020a0e40;
    i4 = idx * 4;
    if (data_020a0de8[idx * 4] != 0) {
        int dy2;
        int dy;
        u8 bx;
        u8 by;

        by = data_020a0deb[i4];
        bx = data_020a0dea[i4];
        oldx = self->unk_5584;
        oldy = self->unk_5588;
        self->unk_5584 = self->unk_5594 + (bx << 12);
        self->unk_5588 = self->unk_5598 + (by << 12);
        if (self->unk_5588 <= 0x94000) self->unk_5588 = 0x94000;
        if (self->unk_5584 <= 0x20000) self->unk_5584 = 0x20000;
        if (self->unk_5584 >= 0xe0000) self->unk_5584 = 0xe0000;
        if (self->unk_5588 >= 0xb8000) self->unk_5588 = 0xb8000;
        dx = (self->unk_5584 - oldx) >> 12;
        dy = (self->unk_5588 - oldy) >> 12;
        dy2 = dy * dy;
        if (_ZN4cstd4sqrtEy((u64)(dx * dx + dy2)) <= 1) {
            self->unk_5584 = oldx;
            self->unk_5588 = oldy;
            return;
        }
        diff = (self->unk_5588 - self->unk_5590) >> 12;
        if (self->unk_55be == 0) {
            func_02012718(0x1d6, self->unk_5584);
            self->unk_55be = 2;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        } else if (self->unk_55be == 1) {
            if (self->unk_55a8 * diff > 0) {
                if (diff < 0) diff = -diff;
                if (diff >= 0xa) self->unk_55be = 0;
            } else {
                self->unk_55a8 = diff;
                self->unk_5590 = self->unk_5588;
            }
        } else {
            if (self->unk_55a8 * diff < 0) self->unk_55be = 1;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        }
        oldAng = self->unk_55b2;
        self->unk_55b2 = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = self->unk_55b2;

            if (a <= 0x8000 && a >= 0x4000) {
                self->unk_55b2 = 0x8000;
            } else if (a <= 0x4000) {
                self->unk_55b2 = 0;
            }
        }
        self->unk_55b2 = (self->unk_55b2 + oldAng) >> 1;
        mag = _ZN4cstd4sqrtEy((u64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
        mag = (mag << 12) >> 4;
        if (mag >= 0xc000) mag = 0xc000;
        if (mag > self->unk_559c) self->unk_559c = mag;
        if (self->unk_559c > mag) self->unk_559c -= (self->unk_559c - mag) >> 1;
        {
            int j = data_020a0e40;
            int ay = (self->unk_5588 >> 12) - data_020a0deb[j * 4];
            int ax = (self->unk_5584 >> 12) - data_020a0dea[j * 4];

            self->unk_5594 = ax << 12;
            self->unk_5598 = ay << 12;
        }
        return;
    }
    self->unk_55b8 = 0;
    self->unk_55b9 = 1;
}
