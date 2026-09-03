//cpp
// @symbol _ZN12daObjClock_c13InitResourcesEv
/* recovered: typed actor, model, and shared-file ownership */
#include "decl_common.h"
#include "daObjClock_c.h"
#include "SharedFilePtr.h"

int daObjClock_c::InitResources()
{
    int isShortHand = (int)(actorID == 0x125);
    if (isShortHand != 0)
        mHandIndex = 0;
    else
        mHandIndex = 1;
    {
        unsigned char index = mHandIndex;
        SharedFilePtr &file = *(SharedFilePtr *)data_ov013_021116b0[index];
        mModel.SetFile((BMD_File *)Model::LoadFile(file), 1, -1);
    }
    func_ov013_02111430((char *)this);
    return 1;
}
