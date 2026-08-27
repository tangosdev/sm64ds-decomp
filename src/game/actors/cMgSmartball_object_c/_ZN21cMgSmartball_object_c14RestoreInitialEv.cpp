//cpp
// @symbol _ZN21cMgSmartball_object_c14RestoreInitialEv
/* recovered: real C++ method, vtable slot 2. Name coined from the body --
   see include/cMgSmartball_object_c.h's own note.

   BOTH READS COME BEFORE BOTH WRITES, deliberately. The plain
   `mCurrent0 = mInitial0; mCurrent1 = mInitial1;` spelling costs 4 words:
   the ROM loads the pair and then stores the pair, which is what the two
   temporaries reproduce. SaveSnapshot below needs no such care -- its
   source and destination do not overlap in the same way, and the naive
   spelling matches there. */
#include "cMgSmartball_object_c.h"
void cMgSmartball_object_c::RestoreInitial()
{
    s32 hi = mInitial1;
    s32 lo = mInitial0;
    mCurrent0 = lo;
    mCurrent1 = hi;
}
