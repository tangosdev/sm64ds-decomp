//cpp
// @symbol _ZN18BowserFireSeaArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
#include "SharedFilePtr.h"
extern int G0[];

int BowserFireSeaArena::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider2)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider2);
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
