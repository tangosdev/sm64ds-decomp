//cpp
// @symbol _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnWaterPlatformJrb.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov016_02114e74[];

int FloatOnWaterPlatformJrb::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114e74);
    _ZN13SharedFilePtr7ReleaseEv(data_ov016_02114e6c);
    return 1;
}
