//cpp
#include "WaterDiamond.h"
#include "WDW_Water.h"

struct Sound {
    static void PlayBank3(u32 id, const Vector3 &pos);
};

void WaterDiamond::CheckClsnWithPlayer()
{
    if (mActive != 0)
        return;
    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    int isPlayer = (int)(actor->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    WDW_Water *water = (WDW_Water *)dActor_c::FindWithID(mWaterID);
    int targetPosY = water->mTargetPosY;
    if (targetPosY != water->mPosY)
        return;
    if (targetPosY == mPosY)
        return;
    mActive = 1;
    water->mTargetPosY = mPosY;
    Sound::PlayBank3(0x63, *(Vector3 *)&mCamSpacePosX);
}
