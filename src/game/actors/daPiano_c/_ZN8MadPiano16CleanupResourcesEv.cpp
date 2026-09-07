//cpp
// @symbol _ZN8MadPiano16CleanupResourcesEv
/* Release the three shared assets after removing the inherited moving mesh
 * collider from the collision world. */
#include "MadPiano.h"
#include "SharedFilePtr.h"

extern SharedFilePtr MadPiano_AnimFile;
extern SharedFilePtr MadPiano_ClsnFile;
extern SharedFilePtr MadPiano_ModelFile;

int MadPiano::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    MadPiano_ModelFile.Release();
    MadPiano_AnimFile.Release();
    MadPiano_ClsnFile.Release();
    return 1;
}
