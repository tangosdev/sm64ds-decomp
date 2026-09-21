//cpp
#include "daBgSnmHed_c.h"

void daBgSnmHed_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}
