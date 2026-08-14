//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * clears the base's unk_031 flag and this class's own countdown. */
#include "cMgSmartball_pushswitch_c.h"

void cMgSmartball_pushswitch_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_031 = 0;
    unk_034 = 0;
}
