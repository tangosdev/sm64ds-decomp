//cpp
#include "SnowmanHead.h"

extern "C" SnowmanHead::StateFunc data_ov072_02122c00[];

void SnowmanHead::SetState(int state)
{
    mStateFuncs = data_ov072_02122c00 + state * 2;
    CallStateInit();
}
