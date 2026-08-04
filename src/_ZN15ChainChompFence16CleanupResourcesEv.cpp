//cpp
// @symbol _ZN15ChainChompFence16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChompFence.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov014_021149b8[];
extern int data_ov014_021149c0[];

int ChainChompFence::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov014_021149c0))->Release();
    ((SharedFilePtr *)(data_ov014_021149b8))->Release();
    return 1;
}
