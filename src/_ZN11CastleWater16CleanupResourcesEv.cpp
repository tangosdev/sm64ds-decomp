//cpp
// @symbol _ZN11CastleWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CastleWater.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int CastleWater::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
