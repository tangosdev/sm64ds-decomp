//cpp
// @symbol _ZN16daObjKinokoTag_c20TrySpawnMegaMushroomEv

#include "daObjKinokoTag_c.h"

void daObjKinokoTag_c::TrySpawnMegaMushroom()
{
    if (mMovingCylinderClsn.otherOwner == 0)
        return;
    if ((mMovingCylinderClsn.hitFlags & 0x20) == 0)
        return;
    SpawnMegaMushroom();
}
