//cpp
#include "dMg3DEspModel_c.h"

extern "C" dMg3DEspModel_c::State data_ov006_0213c774;

// @symbol _ZN15dMg3DEspModel_c5ResetEv
void dMg3DEspModel_c::Reset()
{
    mAnimSet.Reset();
    unk_208 = 0;
    mState = data_ov006_0213c774;
}
