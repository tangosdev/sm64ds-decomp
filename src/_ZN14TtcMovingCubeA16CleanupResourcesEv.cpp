//cpp
// @symbol _ZN14TtcMovingCubeA16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcMovingCubeA.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov065_0211d9cc[];
extern int data_ov065_0211d9d4[];

int TtcMovingCubeA::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov065_0211d9d4))->Release();
    ((SharedFilePtr *)(data_ov065_0211d9cc))->Release();
    return 1;
}
