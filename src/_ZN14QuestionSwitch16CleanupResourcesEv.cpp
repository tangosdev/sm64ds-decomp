//cpp
// @symbol _ZN14QuestionSwitch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "QuestionSwitch.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov002_0210dd60[];
extern int data_ov002_0210dd68[];
extern int data_ov002_0210dd58[];
extern int data_ov002_0210dd50[];

int QuestionSwitch::CleanupResources()
{
    if (((dBgW *)((char *)&mStaticMeshCollider))->IsEnabled()) ((dBgW *)((char *)&mStaticMeshCollider))->Disable();
    if (((dBgW *)((char *)&mMovingMeshCollider))->IsEnabled()) ((dBgW *)((char *)&mMovingMeshCollider))->Disable();
    ((SharedFilePtr *)(data_ov002_0210dd60))->Release();
    ((SharedFilePtr *)(data_ov002_0210dd68))->Release();
    ((SharedFilePtr *)(data_ov002_0210dd58))->Release();
    ((SharedFilePtr *)(data_ov002_0210dd50))->Release();
    return 1;
}
