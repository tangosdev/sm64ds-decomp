//cpp
// @symbol _ZN6ToxBox16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern SharedFilePtr data_ov092_02132540;
extern SharedFilePtr data_ov092_02132548;

int ToxBox::CleanupResources()
{
    data_ov092_02132540.Release();
    data_ov092_02132548.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}
