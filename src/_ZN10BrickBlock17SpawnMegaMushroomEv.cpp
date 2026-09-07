//cpp
// @symbol _ZN10BrickBlock17SpawnMegaMushroomEv
#include "BrickBlock.h"
#include "common.h"

void BrickBlock::SpawnMegaMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x115, 0, spawnPos, 0, mAreaId, -1);
}
