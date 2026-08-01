//cpp
// @symbol _ZN19RotatingPlatformLll16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingPlatformLll.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov022_02114558[];

int RotatingPlatformLll::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114558);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114550);
    return 1;
}
