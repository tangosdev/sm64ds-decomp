//cpp
// @symbol _ZN8Squasher16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Squasher.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int Squasher_ClsnFile[];
extern int Squasher_ModelFile[];

int Squasher::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    ((SharedFilePtr *)(Squasher_ModelFile))->Release();
    ((SharedFilePtr *)(Squasher_ClsnFile))->Release();
    return 1;
}
