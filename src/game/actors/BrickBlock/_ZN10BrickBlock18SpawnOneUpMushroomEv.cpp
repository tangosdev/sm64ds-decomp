//cpp
// @symbol _ZN10BrickBlock18SpawnOneUpMushroomEv
#include "BrickBlock.h"
#include "common.h"

void BrickBlock::SpawnOneUpMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x114, 0, spawnPos, 0, mAreaId, -1);
}
