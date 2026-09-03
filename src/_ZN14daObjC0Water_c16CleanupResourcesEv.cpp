//cpp
// @symbol _ZN14daObjC0Water_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjC0Water_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov012_021124d0[];

int daObjC0Water_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov012_021124d0))->Release();
    ((SharedFilePtr *)(data_ov012_021124c8))->Release();
    return 1;
}
