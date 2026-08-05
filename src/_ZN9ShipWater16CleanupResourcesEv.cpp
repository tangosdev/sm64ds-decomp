//cpp
// @symbol _ZN9ShipWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipWater.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov017_02111c80[];
extern int data_ov017_02111c88[];

int ShipWater::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov017_02111c88))->Release();
    ((SharedFilePtr *)(data_ov017_02111c80))->Release();
    return 1;
}
