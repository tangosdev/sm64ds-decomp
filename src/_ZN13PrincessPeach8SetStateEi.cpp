//cpp
#include "PrincessPeach.h"

extern "C" PrincessPeach::StateFunc data_ov085_0213055c[];

void PrincessPeach::SetState(int state)
{
    mStateFuncs = data_ov085_0213055c + state * 2;
    CallStateInit();
}
