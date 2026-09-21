//cpp
// @symbol _ZN19daObjBlockItemTag_c18SpawnOneUpMushroomEv
#include "daObjBlockItemTag_c.h"
#include "common.h"

void daObjBlockItemTag_c::SpawnOneUpMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x114, 0, spawnPos, 0, mAreaId, -1);
}
