//cpp
#include "SnowmanBody.h"

extern "C" SnowmanBody::StateFunc data_ov072_02122b64[];

void SnowmanBody::SetState(int state)
{
    mStateFuncs = data_ov072_02122b64 + state * 2;
    CallStateInit();
}
