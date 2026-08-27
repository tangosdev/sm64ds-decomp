//cpp
// @symbol _ZN13daObjDorifu_c16CleanupResourcesEP20daObjDorifuResources
#include "daObjDorifu_c.h"

/* Shared cleanup for the three concrete drifting-platform descendants. Their
 * slot-3 overrides pass their own five-entry resource table here. */
s32 daObjDorifu_c::CleanupResources(daObjDorifuResources *resources)
{
    for (s32 i = 0; i < 5; ++i) {
        if (mPlankClsn[i].IsEnabled())
            mPlankClsn[i].Disable();
        resources[i].modelFile->Release();
        resources[i].clsnFile->Release();
    }
    return 1;
}
