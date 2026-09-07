//cpp
// @symbol _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjBk_Dossunbar_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov015_02114534[];

int daObjBk_Dossunbar_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114534 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114538 + mVariant * 0xc)))->Release();
    return 1;
}
