//cpp
// @symbol _ZN12WaterDiamond8BehaviorEv
#include "WaterDiamond.h"
#include "WDW_Water.h"

int WaterDiamond::Behavior()
{
    SetWaterID();
    if (mWaterID == 0) return 1;
    CheckClsnWithPlayer();
    if (mActive != 0) {
        /* The address-valued temporary is load-bearing under 2004/b56: the
         * cartridge keeps r1 as &mAngleY for both the halfword load and store. */
        s16 *angleY = (s16 *)((int)(char *)this + 0x8e);
        *angleY = *angleY + 0x400;
        WDW_Water *water = (WDW_Water *)dActor_c::FindWithID(mWaterID);
        if (water != 0) {
            if (mPosY == water->mPosY) {
                if (mAngleY == 0) mActive = 0;
            }
            if (mPosY != water->mTargetPosY) {
                mActive = 0;
                mAngleY = 0;
            }
        }
    }
    UpdateModelTransform();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}
