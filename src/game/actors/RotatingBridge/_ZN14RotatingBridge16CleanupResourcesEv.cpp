//cpp
// @symbol _ZN14RotatingBridge16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingBridge.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int TowerStep_ClsnFile[];
extern int TowerStep_ModelFile[];

int RotatingBridge::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(TowerStep_ModelFile))->Release();
    ((SharedFilePtr *)(TowerStep_ClsnFile))->Release();
    return 1;
}
