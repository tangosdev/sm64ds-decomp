//cpp
// @symbol _ZN9HugeWater16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HugeWater.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov032_02113af4[];
extern int data_ov032_02113afc[];

int HugeWater::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov032_02113afc))->Release();
    ((SharedFilePtr *)(data_ov032_02113af4))->Release();
    return 1;
}
