//cpp
// @symbol _ZN13CannonShutter16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CannonShutter.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov002_0210e124[];
extern int data_ov002_0210e12c[];

int CannonShutter::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov002_0210e12c))->Release();
    ((SharedFilePtr *)(data_ov002_0210e124))->Release();
    return 1;
}
