//cpp
// @symbol _ZN8Squasher16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];

int Squasher::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
