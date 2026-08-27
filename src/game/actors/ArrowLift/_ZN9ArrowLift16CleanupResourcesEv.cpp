//cpp
// @symbol _ZN9ArrowLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ArrowLift.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov029_02114248[];
extern int data_ov029_02114250[];

int ArrowLift::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov029_02114250))->Release();
    ((SharedFilePtr *)(data_ov029_02114248))->Release();
    return 1;
}
