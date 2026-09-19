//cpp
// @symbol _ZN11daObjTbox_c10InitState1Ev
#include "daObjTbox_c.h"

void daObjTbox_c::InitState1()
{
    if (mIsLastChest != 0)
        mStateTimer = 0x2d;

    mFlags &= ~1;
}
