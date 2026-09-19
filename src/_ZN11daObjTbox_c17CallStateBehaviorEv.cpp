//cpp
// @symbol _ZN11daObjTbox_c17CallStateBehaviorEv
#include "daObjTbox_c.h"

extern "C" daObjTbox_c::StateFunc data_ov064_0211c98c[][2];

void daObjTbox_c::CallStateBehavior()
{
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][1])();
}
