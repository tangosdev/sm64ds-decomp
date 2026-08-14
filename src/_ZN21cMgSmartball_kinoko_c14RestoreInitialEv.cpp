//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial, qualified --
 * unqualified would dispatch virtually and recurse), then zeroes unk_038 and
 * unk_03c. Does NOT touch unk_034 -- read exhaustively, this function simply
 * never mentions it. */
#include "cMgSmartball_kinoko_c.h"

void cMgSmartball_kinoko_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_038 = 0;
    unk_03c = 0;
}
