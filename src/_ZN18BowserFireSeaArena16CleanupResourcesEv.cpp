//cpp
// @symbol _ZN18BowserFireSeaArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];

int BowserFireSeaArena::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider2)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider2);
    }
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
