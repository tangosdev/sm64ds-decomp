//cpp
// @symbol _ZN4Toad8SetStateEi
#include "Toad.h"

extern Toad::State data_ov085_0212fe88[];

void Toad::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].init)();
}
