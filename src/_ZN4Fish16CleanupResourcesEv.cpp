//cpp
// @symbol _ZN4Fish16CleanupResourcesEv
/* recovered: typed actor and shared-file ownership */
#include "decl_common.h"
#include "Fish.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov100_021489cc;
extern SharedFilePtr *data_ov100_021473a4[];
extern SharedFilePtr *data_ov100_021473b0[];

int Fish::CleanupResources()
{
    data_ov100_021489cc.Release();
    data_ov100_021473a4[mModelIndex]->Release();
    data_ov100_021473b0[mModelIndex]->Release();
    if (mHidden == 0 && dActor_c::FindWithID(mUniqueID_13c))
        func_ov100_02146280();
    return 1;
}
