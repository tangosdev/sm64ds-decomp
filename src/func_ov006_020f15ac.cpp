//cpp
// @symbol func_ov006_020f15ac
/* recovered: dScMgLuigi_c per-slot mover, ov006 0x020f15ac (592 bytes). One
 * slot of the 120-entry picture table per call. A slot that has not started
 * yet is armed: phase 0, mStarted set, the x speed loaded from the speed-level
 * table (positive for level 0, negated otherwise) and the y speed from its own
 * table, and the call returns. A running slot adds its two speeds to its
 * position, then runs the x phase machine -- phase 0 decelerates toward zero
 * and flips to 2 or 1 at the crossing (the direction it was moving picks the
 * new phase), phases 1 and 2 accelerate toward the level's top speed and drop
 * back to 0 on reaching it -- and finally wraps the position through
 * func_ov006_020f1dbc.
 *
 * Plain member access throughout, and that is the whole match: every read of
 * mVelX[idx] takes the `this + idx*4 + 0x4000` base with a #0xbb8 offset,
 * every read-modify-write of it takes the pool-loaded array base with the
 * scaled index, and mwccarm picks those two forms itself. The near-miss draft
 * this replaces (div 70) had forced the first form through a u64 launder and
 * `#pragma opt_common_subs off`, which is what rotated its registers. */
#include "dScMgLuigi_c.h"

extern "C" {
extern void func_ov006_020f1dbc(void *self, int idx);
extern int data_ov006_0212e888[];
extern int data_ov006_0212e898[];
extern int data_ov006_0212e8a8[];
}

extern "C" void func_ov006_020f15ac(dScMgLuigi_c *self, int idx)
{
    if (self->mStarted[idx] == 0) {
        self->mMovePhase[idx] = 0;
        self->mStarted[idx]++;
        if (self->mSpeedLevel[idx] == 0) {
            self->mVelX[idx] = data_ov006_0212e888[self->mSpeedLevel[idx]];
        } else {
            self->mVelX[idx] = -data_ov006_0212e888[self->mSpeedLevel[idx]];
        }
        self->mVelY[idx] = data_ov006_0212e898[self->mSpeedLevel[idx]];
        return;
    }
    self->mPosX[idx] += self->mVelX[idx];
    self->mPosY[idx] += self->mVelY[idx];
    if (self->mMovePhase[idx] == 0) {
        if (self->mVelX[idx] > 0) {
            self->mVelX[idx] -= data_ov006_0212e8a8[self->mSpeedLevel[idx]];
            if (self->mVelX[idx] <= 0) {
                self->mVelX[idx] = 0;
                self->mMovePhase[idx] = 2;
            }
        } else if (self->mVelX[idx] < 0) {
            self->mVelX[idx] += data_ov006_0212e8a8[self->mSpeedLevel[idx]];
            if (self->mVelX[idx] >= 0) {
                self->mVelX[idx] = 0;
                self->mMovePhase[idx] = 1;
            }
        }
    } else if (self->mMovePhase[idx] == 1) {
        self->mVelX[idx] += data_ov006_0212e8a8[self->mSpeedLevel[idx]];
        if (self->mVelX[idx] >= data_ov006_0212e888[self->mSpeedLevel[idx]]) {
            self->mMovePhase[idx] = 0;
        }
    } else {
        self->mVelX[idx] -= data_ov006_0212e8a8[self->mSpeedLevel[idx]];
        if (self->mVelX[idx] <= -data_ov006_0212e888[self->mSpeedLevel[idx]]) {
            self->mMovePhase[idx] = 0;
        }
    }
    func_ov006_020f1dbc(self, idx);
}
