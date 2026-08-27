//cpp
// @symbol _ZN21cMgSmartball_object_c12SaveSnapshotEv
/* recovered: real C++ method, vtable slot 0. Name coined from the body --
   see include/cMgSmartball_object_c.h's own note on why this hierarchy has
   no names to recover. */
#include "cMgSmartball_object_c.h"
void cMgSmartball_object_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
}
