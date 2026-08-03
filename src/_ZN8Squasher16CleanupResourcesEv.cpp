//cpp
// @symbol _ZN8Squasher16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int Squasher::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
