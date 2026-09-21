//cpp
// @symbol _ZN19daObjBlockItemTag_c15SpawnKoopaShellEv
#include "daObjBlockItemTag_c.h"
#include "common.h"

void daObjBlockItemTag_c::SpawnKoopaShell()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    dActor_c *shell = Spawn(0x11d, 0, spawnPos, 0, mAreaId, -1);
    if (shell)
        *(u8 *)((char *)shell + 0x3c6) = 0xb4;
}
