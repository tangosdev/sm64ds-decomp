//cpp
#include "SnowmanHead.h"

int SnowmanHead::InitState1()
{
    mSubstate = 0;
    mStateTimer = 0x96;
    mStateValue = 1;
    return 1;
}
