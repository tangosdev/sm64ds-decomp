//cpp
// @symbol _ZN11daKinopio_c8RunStateEv
#include "daKinopio_c.h"

extern daKinopio_c::State data_ov085_0212fe88[];

void daKinopio_c::RunState()
{
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].main)();
}
