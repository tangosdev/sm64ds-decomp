//cpp
/* Slot 2. A real override, not a tail-call veneer: it calls the base's
 * RestoreInitial() (qualified -- unqualified would dispatch through the
 * vtable and recurse) and then also zeroes unk_031 and this child's own
 * two fields. */
#include "cMgSmartball_pakkun_c.h"

void cMgSmartball_pakkun_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_031 = 0;
    mIdleTimer = 0;
    mActionTimer = 0;
}
