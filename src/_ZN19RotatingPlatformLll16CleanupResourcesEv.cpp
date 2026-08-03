//cpp
// @symbol _ZN19RotatingPlatformLll16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingPlatformLll.h"
#include "SharedFilePtr.h"
extern int data_ov022_02114558[];

int RotatingPlatformLll::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    ((SharedFilePtr *)(data_ov022_02114558))->Release();
    ((SharedFilePtr *)(data_ov022_02114550))->Release();
    return 1;
}
