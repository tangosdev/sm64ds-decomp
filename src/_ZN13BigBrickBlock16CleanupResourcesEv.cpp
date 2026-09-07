//cpp
// @symbol _ZN13BigBrickBlock16CleanupResourcesEv
#include "BigBrickBlock.h"
#include "SharedFilePtr.h"

struct BigBrickBlockFileRow {
    SharedFilePtr* file;
    u8 pad[8];
};

extern BigBrickBlockFileRow data_ov002_02108ab0[];
extern BigBrickBlockFileRow data_ov002_02108ab4[];

int BigBrickBlock::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov002_02108ab0[mVariant].file->Release();
    data_ov002_02108ab4[mVariant].file->Release();
    return 1;
}
