//cpp
#include "SnowmanBody.h"

void SnowmanBody::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}
