//cpp
// @symbol _ZN19FloatingFloorLllBig16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatingFloorLllBig.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int FloatingFloorLllBig::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov022_02114620))->Release();
    ((SharedFilePtr *)(data_ov022_02114618))->Release();
    return 1;
}
