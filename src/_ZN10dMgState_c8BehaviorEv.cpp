//cpp
// @symbol _ZN10dMgState_c8BehaviorEv
#include "dMgState_c.h"

int ApproachLinear(s32 &value, s32 target, s32 step);

void dMgState_c::Behavior()
{
    if (mState == -1)
        return;
    ApproachLinear(mTimer, 0, 1);
    if (mBehavior == 0)
        return;
    (this->*mBehavior)();
}
