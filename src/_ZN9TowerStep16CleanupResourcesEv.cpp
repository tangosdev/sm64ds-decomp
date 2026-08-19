//cpp
// @symbol _ZN9TowerStep16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int MovingBarSmall_ClsnFile[];
extern int MovingBarSmall_ModelFile[];

int TowerStep::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(MovingBarSmall_ModelFile))->Release();
    ((SharedFilePtr *)(MovingBarSmall_ClsnFile))->Release();
    return 1;
}
