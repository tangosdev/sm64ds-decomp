//cpp
// @symbol _ZN17daObjKm2_Nobiru_c16CleanupResourcesEv
#include "daObjKm2_Nobiru_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr PoleLift_ClsnFile;
extern SharedFilePtr PoleLift_ModelFile;

int daObjKm2_Nobiru_c::CleanupResources()
{
    mCollider.Disable();
    PoleLift_ModelFile.Release();
    PoleLift_ClsnFile.Release();
    return 1;
}
