//cpp
// @symbol _ZN13BasementWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BasementWater.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov012_021124d0[];

int BasementWater::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov012_021124d0))->Release();
    ((SharedFilePtr *)(data_ov012_021124c8))->Release();
    return 1;
}
