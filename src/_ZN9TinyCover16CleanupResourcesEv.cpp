//cpp
// @symbol _ZN9TinyCover16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TinyCover.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov033_021124f0[];

int TinyCover::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov033_021124f0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov033_021124e8);
    return 1;
}
