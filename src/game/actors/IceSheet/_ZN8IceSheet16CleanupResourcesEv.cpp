//cpp
// @symbol _ZN8IceSheet16CleanupResourcesEv
#include "IceSheet.h"
#include "SharedFilePtr.h"

extern SharedFilePtr IceSheet_ClsnFile;
extern SharedFilePtr IceSheet_ModelFile;

int IceSheet::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    IceSheet_ModelFile.Release();
    IceSheet_ClsnFile.Release();
    return 1;
}
