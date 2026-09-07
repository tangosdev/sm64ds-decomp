//cpp
// @symbol _ZN17ExtendingPlatform16CleanupResourcesEv
#include "ExtendingPlatform.h"
#include "SharedFilePtr.h"

extern SharedFilePtr PoleLift_ClsnFile;
extern SharedFilePtr PoleLift_ModelFile;

int ExtendingPlatform::CleanupResources()
{
    mCollider.Disable();
    PoleLift_ModelFile.Release();
    PoleLift_ClsnFile.Release();
    return 1;
}
