//cpp
// @symbol _ZN22ClockPaintingHandShort16CleanupResourcesEv
/* recovered: typed file ownership through the shared class APIs */
#include "decl_common.h"
#include "ClockPaintingHandShort.h"
#include "SharedFilePtr.h"

int ClockPaintingHandShort::CleanupResources()
{
    ((SharedFilePtr *)data_ov013_021116b0[mHandIndex])->Release();
    return 1;
}
