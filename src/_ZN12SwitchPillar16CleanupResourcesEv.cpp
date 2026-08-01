//cpp
// @symbol _ZN12SwitchPillar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchPillar.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov012_021124d0[];

int SwitchPillar::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124d0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124c8);
    return 1;
}
