//cpp
#include "daBgSnmHed_c.h"

int daBgSnmHed_c::InitState1()
{
    mSubstate = 0;
    mStateTimer = 0x96;
    mStateValue = 1;
    return 1;
}
