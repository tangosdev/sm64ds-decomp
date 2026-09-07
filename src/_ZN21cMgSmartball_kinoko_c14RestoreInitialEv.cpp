//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial, qualified --
 * unqualified would dispatch virtually and recurse), then zeroes mWasHit and
 * mHitTimer. Does NOT touch mVariant -- read exhaustively, this function simply
 * never mentions it. */
#include "cMgSmartball_kinoko_c.h"

void cMgSmartball_kinoko_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    mWasHit = 0;
    mHitTimer = 0;
}
