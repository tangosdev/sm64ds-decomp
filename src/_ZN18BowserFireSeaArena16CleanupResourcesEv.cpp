//cpp
// @symbol _ZN18BowserFireSeaArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int BowserFireSeaArena::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider2))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider2))->Disable();
    }
    ((SharedFilePtr *)(&data_ov060_0211affc))->Release();
    ((SharedFilePtr *)(&data_ov060_0211aff4))->Release();
    return 1;
}
