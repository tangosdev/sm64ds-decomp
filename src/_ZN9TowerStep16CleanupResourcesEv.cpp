//cpp
// @symbol _ZN14MovingBarSmall16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MovingBarSmall.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int MovingBarSmall_ClsnFile[];
extern int MovingBarSmall_ModelFile[];

int MovingBarSmall::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(MovingBarSmall_ModelFile))->Release();
    ((SharedFilePtr *)(MovingBarSmall_ClsnFile))->Release();
    return 1;
}
