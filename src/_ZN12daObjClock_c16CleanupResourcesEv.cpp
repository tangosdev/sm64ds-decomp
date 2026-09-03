//cpp
// @symbol _ZN12daObjClock_c16CleanupResourcesEv
/* recovered: typed file ownership through the shared class APIs */
#include "decl_common.h"
#include "daObjClock_c.h"
#include "SharedFilePtr.h"

int daObjClock_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov013_021116b0[mHandIndex])->Release();
    return 1;
}
