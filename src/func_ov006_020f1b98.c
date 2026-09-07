//cpp
// @symbol func_ov006_020f1b98
/* dScMgLuigi_c per-slot mover, ov006 0x020f1b98 (284 bytes). A slot that has not started takes its phase from unk_515c[speed level].
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through func_ov006_020f1dbc.
 *
 * Plain member access on the class header is the match under 2004/b56: the
 * twice-read mMovePhase[idx] takes the `this + idx*2 + 0x4f00` base with a
 * #0x7c offset as a compiler temp, and the two RMWs take the pool-loaded array
 * base with the scaled index. The raw char* form this replaces pooled 0x4f7c
 * whole (+8 bytes) and, once that was fixed by hand, still swapped the r4/ip
 * pair in the second update. */
#include "dScMgLuigi_c.h"

extern "C" {
extern void func_ov006_020f1dbc(void *self, int idx);
extern int data_ov006_0212e8d8[];
extern s16 data_02082214[];
}

extern "C" void func_ov006_020f1b98(dScMgLuigi_c *self, int idx)
{
    if (self->mStarted[idx] == 0) {
        u8 t = self->mSpeedLevel[idx];
        self->mMovePhase[idx] = self->unk_515c[t];
        self->mStarted[idx]++;
    } else {
        int a = self->mMovePhase[idx] >> 4;
        self->mPosX[idx] = self->mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8d8[self->mSpeedLevel[idx]] + 0x800) >> 12);
        a = self->mMovePhase[idx] >> 4;
        self->mPosY[idx] = self->mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e8d8[self->mSpeedLevel[idx]] + 0x800) >> 12);
        func_ov006_020f1dbc(self, idx);
    }
}
