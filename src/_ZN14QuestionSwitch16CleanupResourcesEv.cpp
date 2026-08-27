//cpp
// @symbol _ZN14QuestionSwitch16CleanupResourcesEv
#include "QuestionSwitch.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov002_0210dd60;
extern SharedFilePtr data_ov002_0210dd68;
extern SharedFilePtr data_ov002_0210dd58;
extern SharedFilePtr data_ov002_0210dd50;

int QuestionSwitch::CleanupResources()
{
    if (mStaticMeshCollider.IsEnabled())
        mStaticMeshCollider.Disable();
    if (mMovingMeshCollider.IsEnabled())
        mMovingMeshCollider.Disable();
    data_ov002_0210dd60.Release();
    data_ov002_0210dd68.Release();
    data_ov002_0210dd58.Release();
    data_ov002_0210dd50.Release();
    return 1;
}
