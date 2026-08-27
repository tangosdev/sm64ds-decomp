//cpp
// @symbol _ZN22ClockPaintingHandShort13InitResourcesEv
/* recovered: typed actor, model, and shared-file ownership */
#include "decl_common.h"
#include "ClockPaintingHandShort.h"
#include "SharedFilePtr.h"

int ClockPaintingHandShort::InitResources()
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
