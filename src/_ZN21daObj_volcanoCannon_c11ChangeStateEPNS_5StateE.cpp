//cpp
// @symbol _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE
/* daObj_volcanoCannon_c's state transition helper. The state record contains two
 * pointer-to-member callbacks; transition stores the record and runs init. */
#include "daObj_volcanoCannon_c.h"

int daObj_volcanoCannon_c::ChangeState(State *state)
{
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}
