//cpp
// @symbol _ZN11CannonHatch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CannonHatch.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov002_0210e124[];
extern int data_ov002_0210e12c[];

int CannonHatch::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov002_0210e12c))->Release();
    ((SharedFilePtr *)(data_ov002_0210e124))->Release();
    return 1;
}
