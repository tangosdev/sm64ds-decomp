//cpp
// @symbol _ZN16SpinningPlatform16CleanupResourcesEv
#include "SpinningPlatform.h"
#include "SharedFilePtr.h"
extern SharedFilePtr RotatingClockHand_ClsnFile;
extern SharedFilePtr RotatingClockHand_ModelFile;

int SpinningPlatform::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    RotatingClockHand_ModelFile.Release();
    RotatingClockHand_ClsnFile.Release();
    return 1;
}
