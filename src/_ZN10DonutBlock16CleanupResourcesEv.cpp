//cpp
// @symbol _ZN8ShipWing16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipWing.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov036_0211408c[];

int ShipWing::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov036_0211408c))->Release();
    ((SharedFilePtr *)(data_ov036_02114084))->Release();
    return 1;
}
