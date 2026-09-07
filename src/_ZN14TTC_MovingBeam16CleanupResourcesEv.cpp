//cpp
// @symbol _ZN14TTC_MovingBeam16CleanupResourcesEv
/* recovered: real C++ method on shared collider and file-handle declarations */
#include "TTC_MovingBeam.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov065_0211d9cc;
extern SharedFilePtr data_ov065_0211d9d4;

int TTC_MovingBeam::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d9d4.Release();
    data_ov065_0211d9cc.Release();
    return 1;
}
