//cpp
// @symbol _ZN9WDW_Water16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int RotatingPlatformWdw_ClsnFile[];
extern int RotatingPlatformWdw_ModelFile[];

int WDW_Water::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(RotatingPlatformWdw_ModelFile))->Release();
    ((SharedFilePtr *)(RotatingPlatformWdw_ClsnFile))->Release();
    return 1;
}
