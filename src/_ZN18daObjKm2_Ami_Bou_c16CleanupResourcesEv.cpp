//cpp
// @symbol _ZN18daObjKm2_Ami_Bou_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjKm2_Ami_Bou_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov045_021131b0[];

int daObjKm2_Ami_Bou_c::CleanupResources()
{
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(data_ov045_021131b0))->Release();
    ((SharedFilePtr *)(data_ov045_021131a8))->Release();
    return 1;
}
