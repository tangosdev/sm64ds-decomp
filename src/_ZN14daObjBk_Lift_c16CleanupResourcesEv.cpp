//cpp
// @symbol _ZN14daObjBk_Lift_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjBk_Lift_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int MovingBarSmall_ClsnFile[];
extern int MovingBarSmall_ModelFile[];

int daObjBk_Lift_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(MovingBarSmall_ModelFile))->Release();
    ((SharedFilePtr *)(MovingBarSmall_ClsnFile))->Release();
    return 1;
}
