//cpp
// @symbol _ZN11VolcanoFire11ChangeStateEPNS_5StateE
/* VolcanoFire's state transition helper. The state record contains two
 * pointer-to-member callbacks; transition stores the record and runs init. */
#include "VolcanoFire.h"

int VolcanoFire::ChangeState(State *state)
{
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}
