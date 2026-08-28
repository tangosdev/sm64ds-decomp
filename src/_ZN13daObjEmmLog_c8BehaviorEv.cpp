//cpp
// @symbol _ZN13daObjEmmLog_c8BehaviorEv
/* daObjEmmLog_c::Behavior -- vtable slot 6. The rolling log of Tiny-Huge
 * Island: it spins at a fixed rate and rides up and down on a sine of its own
 * spin angle, mBobAmplitude being the amplitude and mBasePosY the centre height. */
#include "daObjEmmLog_c.h"

extern "C" {
void func_020393a4(int *clsn, int v);
/* The name claims Fix12<int> by value; the bytes take a plain word, the same
   disagreement include/dBgActor_c.h records for both IsClsnInRange symbols. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int range, int b);
extern short data_02082214[];
}

int daObjEmmLog_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x600000);

    int idx = (u16)mSpinAngle >> 4;
    int s = *(short *)((char *)data_02082214 + (idx << 2));
    int m = (int)(((long long)mBobAmplitude * s + 0x800) >> 12);
    mPosY = mBasePosY + m;

    mSpinAngle += 0x200;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x600000, 0);
    return 1;
}
