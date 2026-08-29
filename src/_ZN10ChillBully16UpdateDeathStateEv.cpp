//cpp
#include "ChillBully.h"

extern "C" int func_ov064_0211616c(void *self);

void ChillBully::UpdateDeathState()
{
    int result = func_ov064_0211616c(this);
    if (result == 0)
        return;
    TriplePoofDust();
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x64000;
    UntrackAndSpawnStar(mStarSlot, (mStarIdx | 0x40) & 0xff, pos, 4);
}
