//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), latches
 * mCurrent into the mLastPos0/mLastPos1 position pair, zeroes the base's
 * velocity pair, then exhaustively zeroes every field this class adds --
 * this function is the strongest evidence for the field list in the header,
 * since its zero pass is the one place all of them are touched at once. */
#include "cMgSmartball_ball_c.h"

void cMgSmartball_ball_c::RestoreInitial()
{
    int i;

    cMgSmartball_object_c::RestoreInitial();
    mLastPos0 = mCurrent0;
    mLastPos1 = mCurrent1;
    mVel0 = 0;
    mVel1 = 0;
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
    mUpperWallSolid = 0;
    unk_039 = 0;
    mZoneCooldown = 0;
    state3a = 0;
    state3b = 0;
    mZoneDwell = 0;
    targetIndex = -1;
    unk_0fc = 0;
    mIsWaiting = 1;
    mQueueGap = 0;
    mExpireTimer = 0;
    mIsFrozen = 0;
    mStuckFrames = 0;
    unk_120 = 0;
    mInPlay = 1;
    if ((*(int *)((char *)mpManager + 8) & 0xff) && mIndex >= 9)
        mInPlay = 0;
    else
        mInPlay = 1;
    soundTimer = 0;
    soundPlayed = 0;
    mExitGateOpen = 0;
}
