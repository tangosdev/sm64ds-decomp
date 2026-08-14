//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * zeroes unk_044 -- the ONLY offset this class's RestoreInitial reaches;
 * unk_034/038/03c/040 carry whatever SaveSnapshot last left there. */
#include "cMgSmartball_spring_c.h"

void cMgSmartball_spring_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_044 = 0;
}
