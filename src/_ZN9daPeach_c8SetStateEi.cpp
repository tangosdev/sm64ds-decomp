//cpp
#include "daPeach_c.h"

extern "C" daPeach_c::StateFunc data_ov085_0213055c[];

void daPeach_c::SetState(int state)
{
    mStateFuncs = data_ov085_0213055c + state * 2;
    CallStateInit();
}
