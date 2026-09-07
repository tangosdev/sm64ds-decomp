//cpp
// @symbol _ZN10SlidingBox16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingBox.h"
#include "SharedFilePtr.h"
extern int data_ov016_02114e74[];

int SlidingBox::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)(data_ov016_02114e74))->Release();
    ((SharedFilePtr *)(data_ov016_02114e6c))->Release();
    return 1;
}
