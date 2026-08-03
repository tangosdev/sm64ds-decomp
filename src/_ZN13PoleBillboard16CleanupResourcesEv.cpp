//cpp
// @symbol _ZN13PoleBillboard16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleBillboard.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int PoleBillboard::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
