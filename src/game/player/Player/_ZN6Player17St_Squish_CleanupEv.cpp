//cpp
// @symbol _ZN6Player17St_Squish_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::St_Squish_Cleanup()
{
    int val = 0x1000;
    if (mIsMega != 0)
        val = 0x3000;

    mScaleX = val;
    mScaleY = val;
    mScaleZ = val;

    *(int *)((char *)&mdCcAcPos_c.flags) &= ~4;
    return 1;
}
