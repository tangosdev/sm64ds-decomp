//cpp
// @symbol _ZN14daObjTtWater_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjTtWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov033_021124f0[];

int daObjTtWater_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov033_021124f0))->Release();
    ((SharedFilePtr *)(data_ov033_021124e8))->Release();
    return 1;
}
