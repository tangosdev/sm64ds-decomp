//cpp
#include "SnowmanHead.h"

void SnowmanHead::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}
