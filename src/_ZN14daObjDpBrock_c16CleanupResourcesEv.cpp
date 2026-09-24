//cpp
// @symbol _ZN14daObjDpBrock_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjDpBrock_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov025_02113ab0[];
extern int data_ov025_02113ab8[];

int daObjDpBrock_c::CleanupResources()
{
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(data_ov025_02113ab8))->Release();
    ((SharedFilePtr *)(data_ov025_02113ab0))->Release();
    return 1;
}
