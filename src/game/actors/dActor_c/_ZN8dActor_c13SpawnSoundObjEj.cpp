//cpp
// @symbol _ZN8dActor_c13SpawnSoundObjEj
/* dActor_c::SpawnSoundObj(u32) at 0x02010bf0.
 *
 * Spawns actor 0x167 -- the positional sound emitter -- at this actor's own
 * position and area, with no rotation and no death-table slot.
 *
 * dActor_c::Spawn is called through its mangled name rather than a declared method
 * because its signature carries by-value class parameters; see
 * notes/mwccarm-codegen.md 6az.
 */
#include "dActor_c.h"

struct Vector3_16 { short x, y, z; };

extern "C" dActor_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param, const void *pos, const Vector3_16 *rot,
    s32 areaID, s32 deathTableID);

void dActor_c::SpawnSoundObj(u32 soundObjParam)
{
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0x167, soundObjParam, (const void *)&mPosX, (const Vector3_16 *)0,
        (s32)mAreaId, -1);
}
