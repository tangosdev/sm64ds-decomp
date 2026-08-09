//cpp
// @symbol _ZN11CastleWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Releases the model and collision files, then disables the mesh collider --
 * but only if it is still enabled, so a second cleanup is harmless.
 */
#include "CastleWater.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

extern "C" {
extern int data_ov009_02113c68[];
extern int data_ov009_02113c70[];
}

int CastleWater::CleanupResources()
{
    ((SharedFilePtr *)((void*)data_ov009_02113c68))->Release();
    ((SharedFilePtr *)((void*)data_ov009_02113c70))->Release();
    if (((MeshColliderBase *)&mMeshCollider)->IsEnabled())
        ((MeshColliderBase *)&mMeshCollider)->Disable();
    return 1;
}
