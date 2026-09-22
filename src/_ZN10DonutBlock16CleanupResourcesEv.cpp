//cpp
// @symbol _ZN10DonutBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov036_0211408c[];

int DonutBlock::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov036_0211408c))->Release();
    ((SharedFilePtr *)(data_ov036_02114084))->Release();
    return 1;
}
