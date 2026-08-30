//cpp
// @symbol _ZN11daDsnBase_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daDsnBase_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int daDsnBase_c::CleanupResources()
{
    void **fp;
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[0]))->Release();
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[1]))->Release();
    fp = *(void***)((char *)&mFileTable);
    if (fp[3] != 0) {
        ((SharedFilePtr *)(fp[3]))->Release();
    }
    return 1;
}
