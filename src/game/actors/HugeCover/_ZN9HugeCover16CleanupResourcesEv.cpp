//cpp
// @symbol _ZN9HugeCover16CleanupResourcesEv
#include "HugeCover.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov032_02113acc;
extern "C" SharedFilePtr data_ov032_02113ad4;

s32 HugeCover::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113ad4.Release();
    data_ov032_02113acc.Release();
    return 1;
}
