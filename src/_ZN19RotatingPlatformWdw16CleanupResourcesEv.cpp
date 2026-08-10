//cpp
// @symbol _ZN19RotatingPlatformWdw16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingPlatformWdw.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int RotatingPlatformWdw_ClsnFile[];
extern int RotatingPlatformWdw_ModelFile[];

int RotatingPlatformWdw::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(RotatingPlatformWdw_ModelFile))->Release();
    ((SharedFilePtr *)(RotatingPlatformWdw_ClsnFile))->Release();
    return 1;
}
