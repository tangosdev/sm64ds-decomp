//cpp
// @symbol _ZN8Squasher16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
#include "SharedFilePtr.h"
extern int G0[];

int Squasher::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
