//cpp
// @symbol _ZN19FloatingFloorLllBig16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatingFloorLllBig.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int FloatingFloorLllBig_ClsnFile[];
extern int FloatingFloorLllBig_ModelFile[];

int FloatingFloorLllBig::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(FloatingFloorLllBig_ModelFile))->Release();
    ((SharedFilePtr *)(FloatingFloorLllBig_ClsnFile))->Release();
    return 1;
}
