//cpp
// @symbol _ZN11daTrsTrap_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daTrsTrap_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int daTrsTrap_c::CleanupResources()
{
    ((dBgW *)((char *)&mMovingMeshCollider))->Disable();
    int idx = *(int*)((char*)&mIndex);
    ((SharedFilePtr *)((void*)(data_ov063_0211e27c[idx])))->Release();
    idx = *(int*)((char*)&mIndex);
    ((SharedFilePtr *)((void*)(data_ov063_0211e28c[idx])))->Release();
    return 1;
}
