//cpp
// @symbol _ZN19daObjBlockItemTag_c17SpawnMegaMushroomEv
#include "daObjBlockItemTag_c.h"
#include "common.h"

void daObjBlockItemTag_c::SpawnMegaMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x115, 0, spawnPos, 0, mAreaId, -1);
}
