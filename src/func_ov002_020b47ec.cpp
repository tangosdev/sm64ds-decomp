//cpp
// @symbol _ZN21MegaMushroomCreateTag20TrySpawnMegaMushroomEv

#include "MegaMushroomCreateTag.h"

void MegaMushroomCreateTag::TrySpawnMegaMushroom()
{
    if (mMovingCylinderClsn.otherOwner == 0)
        return;
    if ((mMovingCylinderClsn.hitFlags & 0x20) == 0)
        return;
    SpawnMegaMushroom();
}
