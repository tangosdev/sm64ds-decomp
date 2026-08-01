//cpp
// @symbol _ZN29FloatOnWaterPlatformWdwSquare16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnWaterPlatformWdwSquare.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];

int FloatOnWaterPlatformWdwSquare::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
