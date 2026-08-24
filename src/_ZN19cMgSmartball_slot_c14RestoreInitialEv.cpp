//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * exhaustively zeroes every field this class adds, in the ROM's own
 * per-index-then-next-field order for the two 3-element arrays (A[i].a,
 * A[i].b, B[i].a, B[i].b, C[i], D[i] before moving to i+1), followed by
 * the four trailing scalars in address order. This function is the
 * strongest evidence for the field list in the header. */
#include "cMgSmartball_slot_c.h"

void cMgSmartball_slot_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    for (int i = 0; i < 3; i++) {
        mReelOffset[i].x = 0;
        mReelOffset[i].y = 0;
        mReelVel[i].x = 0;
        mReelVel[i].y = 0;
        mReelStopCount[i] = 0;
        mReelStopping[i] = 0;
    }
    mIsSpinning = 0;
    mIsSpeedingUp = 0;
    mIsSlowingDown = 0;
    mSpinsQueued = 0;
    mSpinCooldown = 0;
    mSpinTimer = 0;
    mSoundHandle = 0;
}
