//cpp
// @symbol _ZN8IceBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "IceBlock.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov081_02128fd8[];

int IceBlock::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov081_02128fd8))->Release();
    ((SharedFilePtr *)(data_ov081_02128fd0))->Release();
    return 1;
}
