//cpp
// @symbol _ZN19FloatOnLavaPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnLavaPlatform.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov022_02114558[];

int FloatOnLavaPlatform::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov022_02114558))->Release();
    ((SharedFilePtr *)(data_ov022_02114550))->Release();
    return 1;
}
