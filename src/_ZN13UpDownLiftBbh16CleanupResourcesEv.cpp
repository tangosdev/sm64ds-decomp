//cpp
// @symbol _ZN13UpDownLiftBbh16CleanupResourcesEv
/* recovered: real C++ method using the inherited collider member */
#include "UpDownLiftBbh.h"
#include "SharedFilePtr.h"

extern SharedFilePtr *data_ov095_02136f68[];
extern SharedFilePtr *data_ov095_02136f74[];

int UpDownLiftBbh::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov095_02136f68[mVariant]->Release();
    data_ov095_02136f74[mVariant]->Release();
    return 1;
}
