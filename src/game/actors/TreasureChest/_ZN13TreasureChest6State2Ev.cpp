//cpp
// @symbol _ZN13TreasureChest6State2Ev
#include "TreasureChest.h"

extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);

void TreasureChest::State2()
{
    if (mIsLastChest == 0)
        return;
    if (mStateTimer == 0)
        return;
    if (DecIfAbove0_Short(&mStateTimer) != 0)
        return;

    if (mStarID != 0xff) {
        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;
        UntrackAndSpawnStar(mTrackedStarID, mStarID, spawnPos, 4);
    }

    mFlags |= 1;
}
