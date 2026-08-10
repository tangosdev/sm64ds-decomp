//cpp
// @symbol _ZN9TowerStep16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int TowerStep_ClsnFile[];
extern int TowerStep_ModelFile[];

int TowerStep::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(TowerStep_ModelFile))->Release();
    ((SharedFilePtr *)(TowerStep_ClsnFile))->Release();
    return 1;
}
