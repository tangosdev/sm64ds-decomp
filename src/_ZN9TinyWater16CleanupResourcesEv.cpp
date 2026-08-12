//cpp
// @symbol _ZN9TinyWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TinyWater.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov033_021124f0[];

int TinyWater::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov033_021124f0))->Release();
    ((SharedFilePtr *)(data_ov033_021124e8))->Release();
    return 1;
}
