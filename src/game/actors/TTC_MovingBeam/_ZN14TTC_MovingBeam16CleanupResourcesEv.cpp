//cpp
// @symbol _ZN14TTC_MovingBeam16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TTC_MovingBeam.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov065_0211d9cc[];
extern int data_ov065_0211d9d4[];

int TTC_MovingBeam::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov065_0211d9d4))->Release();
    ((SharedFilePtr *)(data_ov065_0211d9cc))->Release();
    return 1;
}
