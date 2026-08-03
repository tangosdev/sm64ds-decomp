//cpp
// @symbol _ZN18BowserFireSeaArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int BowserFireSeaArena::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider2))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider2))->Disable();
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
