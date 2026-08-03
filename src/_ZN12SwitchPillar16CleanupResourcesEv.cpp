//cpp
// @symbol _ZN12SwitchPillar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchPillar.h"
#include "SharedFilePtr.h"
extern int data_ov012_021124d0[];

int SwitchPillar::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    ((SharedFilePtr *)(data_ov012_021124d0))->Release();
    ((SharedFilePtr *)(data_ov012_021124c8))->Release();
    return 1;
}
