//cpp
// @symbol _ZN15FireSeaElevator16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FireSeaElevator.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];

int FireSeaElevator::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
