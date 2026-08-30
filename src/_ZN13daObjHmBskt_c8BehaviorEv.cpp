//cpp
// @symbol _ZN13daObjHmBskt_c8BehaviorEv
/* The class identity and slot are ROM-proven by _ZTV13daObjHmBskt_c. The two
 * integer arguments below really are Fix12<int> by value, which is a known
 * mwccarm language-mode wall, so that callee remains the narrow C-linkage
 * boundary. This method itself is ordinary typed C++ throughout. */
#include "daObjHmBskt_c.h"

extern "C" int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, s32 range, s32 offsetY);
extern "C" int func_ov030_0211124c(daObjHmBskt_c *self, dBgCh_Actr *clsn);

int daObjHmBskt_c::Behavior()
{
    volatile int dummy[4];
    (void)&dummy;

    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    mAngleY = mAngleY + mHorzSpeed;

    if (mVertAccel != 0) {
        mFlags &= ~1;
        UpdatePos(0);
        func_ov030_0211124c(this, &mWithMeshClsn);

        s32 y = mPosY;
        s32 z = mPosZ;
        dActor_c *star = mStarActor;
        s32 x = mPosX;
        star->mPosX = x;
        star->mPosY = y + 0x3c000;
        star->mPosZ = z;
    }

    return 1;
}
