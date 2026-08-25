//cpp
// @symbol _ZN8PathLift9AfterClsnEv
/* recovered: named members + shared header, real C++ method */
#include "PathLift.h"
extern "C" {
int func_ov002_020efedc(void *c);
int DecIfAbove0_Byte(void *p);
void func_02012694(int a, void *p);
void func_ov002_020efa54(void *c, int a);
}

void PathLift::AfterClsn()
{
    if (func_ov002_020efedc(((char *)this)) != 0 &&
        mState == 0 &&
        DecIfAbove0_Byte((char *)&mTriggerDelay) == 0) {
        int b = actorID == 0x1f;
        if (b) {
            func_02012694(0x6f, ((char *)this) + 0x74);
        }
        func_ov002_020efa54(((char *)this), 1);
    }
    mAfterClsnRan = 1;
}
