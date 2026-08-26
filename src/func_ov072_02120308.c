//cpp
#include "SnowmanHead.h"

int SnowmanHead::InitState2()
{
    mFlags &= ~1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mVertSpeed = 0x2d000;
    mStateTimer = 0x78;
    mSubstate = 0;
    mStateValue = 2;
    return 1;
}
