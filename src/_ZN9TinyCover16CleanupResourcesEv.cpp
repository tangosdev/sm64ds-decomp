//cpp
// @symbol _ZN9TinyCover16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TinyCover.h"
#include "SharedFilePtr.h"
extern int data_ov033_021124f0[];

int TinyCover::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    ((SharedFilePtr *)(data_ov033_021124f0))->Release();
    ((SharedFilePtr *)(data_ov033_021124e8))->Release();
    return 1;
}
