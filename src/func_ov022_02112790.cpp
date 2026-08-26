//cpp
// @symbol func_ov022_02112790
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
