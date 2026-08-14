//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), latches
 * mCurrent into the unk_114/unk_118 position pair, zeroes the base's
 * velocity pair, then exhaustively zeroes every field this class adds --
 * this function is the strongest evidence for the field list in the header,
 * since its zero pass is the one place all of them are touched at once. */
#include "cMgSmartball_ball_c.h"

void cMgSmartball_ball_c::RestoreInitial()
{
    int i;

    cMgSmartball_object_c::RestoreInitial();
    unk_114 = mCurrent0;
    unk_118 = mCurrent1;
    unk_020 = 0;
    unk_024 = 0;
    for (i = 0; i < 0x20; i++) {
        hit[i] = 0;
        hitA[i] = 0;
        hitC[i] = 0;
        hitB[i] = 0;
    }
    for (i = 0; i < 0x19; i++) {
        nearby[i] = 0;
    }
    anyHit = 0;
    specialHit = 0;
    unk_038 = 0;
    unk_039 = 0;
    unk_034 = 0;
    state3a = 0;
    state3b = 0;
    unk_03c = 0;
    targetIndex = -1;
    unk_0fc = 0;
    unk_100 = 1;
    unk_104 = 0;
    unk_040 = 0;
    unk_110 = 0;
    unk_11c = 0;
    unk_120 = 0;
    unk_121 = 1;
    if ((*(int *)((char *)unk_004 + 8) & 0xff) && unk_02c >= 9)
        unk_121 = 0;
    else
        unk_121 = 1;
    soundTimer = 0;
    soundPlayed = 0;
    unk_129 = 0;
}
