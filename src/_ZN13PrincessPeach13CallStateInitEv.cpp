//cpp
#include "PrincessPeach.h"

void PrincessPeach::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}
