//cpp
#include "daBgSnmHed_c.h"

void daBgSnmHed_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}
