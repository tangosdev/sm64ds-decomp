//cpp
#include "daBgSnmHed_c.h"

extern "C" daBgSnmHed_c::StateFunc data_ov072_02122c00[];

void daBgSnmHed_c::SetState(int state)
{
    mStateFuncs = data_ov072_02122c00 + state * 2;
    CallStateInit();
}
