//cpp
#include "daPeach_c.h"

void daPeach_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}
