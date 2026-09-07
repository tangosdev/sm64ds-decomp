//cpp
#include "SnowmanHead.h"

void SnowmanHead::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}
