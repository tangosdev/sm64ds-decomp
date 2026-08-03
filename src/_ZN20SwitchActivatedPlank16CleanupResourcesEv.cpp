//cpp
// @symbol _ZN20SwitchActivatedPlank16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int SwitchActivatedPlank::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
