//cpp
// @symbol _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnWaterPlatformJrb.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov016_02114e74[];

int FloatOnWaterPlatformJrb::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov016_02114e74))->Release();
    ((SharedFilePtr *)(data_ov016_02114e6c))->Release();
    return 1;
}
