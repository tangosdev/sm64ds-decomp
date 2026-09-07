//cpp
// @symbol _ZN16dPathLiftActor_c9AfterClsnEi
/* recovered: named members + shared header, real C++ method */
#include "PathLift.h"
extern "C" {
int func_ov002_020efedc(dPathLiftActor_c *lift);
u8 DecIfAbove0_Byte(u8 *p);
void func_02012694(int a, void *p);
void func_ov002_020efa54(dPathLiftActor_c *lift, int state);
}

void dPathLiftActor_c::AfterClsn(int)
{
    /* These two helpers keep their configured address names until the combined
       class TU owns them; typed calls avoid the old char* shadow ABI here. */
    if (func_ov002_020efedc(this) != 0 &&
        mState == 0 &&
        DecIfAbove0_Byte(&mTriggerDelay) == 0) {
        int b = actorID == 0x1f;
        if (b) {
            func_02012694(0x6f, &mCamSpacePosX);
        }
        func_ov002_020efa54(this, 1);
    }
    mAfterClsnRan = 1;
}
