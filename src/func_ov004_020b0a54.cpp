//cpp
#include "dScMgBase_c.h"

extern "C" void func_ov004_020b0a54(s32 state)
{
    dScMgBase_c *scene = (dScMgBase_c *)data_ov004_020beb68;
    if (!scene)
        return;
    scene->mStateController.SetState(state);
    ((dScMgBase_c *)data_ov004_020beb68)->unk_060 = 0;
}
