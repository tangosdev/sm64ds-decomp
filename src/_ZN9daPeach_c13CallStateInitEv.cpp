//cpp
#include "daPeach_c.h"

void daPeach_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}
