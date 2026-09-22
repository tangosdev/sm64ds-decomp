//cpp
// @symbol func_ov006_020d7c4c
#include "dScMgBomroom_c.h"
/* dScMgBomroom_c bomb idx step: advance x/y along the angle at unk_10 per
   frame (sine table, Fix12), then bounce off the wall box selected by
   unk_36 (inner room 0xc0..0x100 x 0x40..0x80, outer 0..0x40 x 0x40..0x80):
   a horizontal wall mirrors the angle (0x8000 - a), a vertical one negates
   it, and the coordinate is pinned just inside the wall. Both products keep
   the sine-table value as the first factor (named s16 sinv/cosv); inline
   table reads flip the smull operand order. */
extern "C" {
extern s16 data_02082214[];
}

extern "C" void func_ov006_020d7c4c(dScMgBomroom_c *self, int idx)
{
    int xv;
    int yv;
    s16 sinv;
    s16 cosv;

    sinv = data_02082214[(self->mBombs[idx].angle >> 4) * 2 + 1];
    self->mBombs[idx].x += (s32)(((s64)sinv * self->mBombs[idx].unk_10 + 0x800) >> 12);
    cosv = data_02082214[(self->mBombs[idx].angle >> 4) * 2];
    self->mBombs[idx].y += (s32)(((s64)cosv * self->mBombs[idx].unk_10 + 0x800) >> 12);
    xv = self->mBombs[idx].x >> 12;
    yv = self->mBombs[idx].y >> 12;
    if (self->mBombs[idx].unk_36 != 0) {
        if (xv + 0xc > 0x100) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xf4000;
        } else if (xv - 0xc < 0xc0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xcc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    } else {
        if (xv + 0xc > 0x40) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0x34000;
        } else if (xv - 0xc < 0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    }
}
