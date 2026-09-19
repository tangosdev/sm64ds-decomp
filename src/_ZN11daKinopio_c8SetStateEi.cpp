//cpp
// @symbol _ZN11daKinopio_c8SetStateEi
#include "daKinopio_c.h"

extern daKinopio_c::State data_ov085_0212fe88[];

void daKinopio_c::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].init)();
}
