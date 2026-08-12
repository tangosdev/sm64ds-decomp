//cpp
// @symbol _ZN13PoleBillboard16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleBillboard.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int PoleBillboard_ClsnFile[];
extern int PoleBillboard_ModelFile[];

int PoleBillboard::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(PoleBillboard_ModelFile))->Release();
    ((SharedFilePtr *)(PoleBillboard_ClsnFile))->Release();
    return 1;
}
