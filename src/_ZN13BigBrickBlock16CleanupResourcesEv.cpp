//cpp
// @symbol _ZN13daObjBlockL_c16CleanupResourcesEv
#include "daObjBlockL_c.h"
#include "SharedFilePtr.h"

struct BigBrickBlockFileRow {
    SharedFilePtr* file;
    u8 pad[8];
};

extern BigBrickBlockFileRow data_ov002_02108ab0[];
extern BigBrickBlockFileRow data_ov002_02108ab4[];

int daObjBlockL_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_02108ab0[mVariant].file->Release();
    data_ov002_02108ab4[mVariant].file->Release();
    return 1;
}
