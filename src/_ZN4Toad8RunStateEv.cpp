//cpp
// @symbol _ZN4Toad8RunStateEv
#include "Toad.h"

extern Toad::State data_ov085_0212fe88[];

void Toad::RunState()
{
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].main)();
}
