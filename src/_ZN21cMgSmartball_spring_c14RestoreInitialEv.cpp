//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * zeroes mSoundHandle -- the ONLY offset this class's RestoreInitial reaches;
 * unk_034/mDriveBase/unk_03c/mDriveNow carry whatever SaveSnapshot last
 * left there. */
#include "cMgSmartball_spring_c.h"

void cMgSmartball_spring_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    mSoundHandle = 0;
}
