//cpp
#include "PrincessPeach.h"

void PrincessPeach::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}
