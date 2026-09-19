//cpp
// @symbol _ZN11daObjTbox_c10InitState2Ev
#include "daObjTbox_c.h"

void daObjTbox_c::InitState2()
{
    if (mIsLastChest == 0)
        mFlags |= 1;
}
