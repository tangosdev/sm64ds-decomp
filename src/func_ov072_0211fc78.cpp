//cpp
#include "SnowmanBody.h"

void SnowmanBody::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}
