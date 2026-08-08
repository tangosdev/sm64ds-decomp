//cpp
// @symbol _ZN5Actor13SpawnSoundObjEj
/* Actor::SpawnSoundObj(u32) at 0x02010bf0.
 *
 * Spawns actor 0x167 -- the positional sound emitter -- at this actor's own
 * position and area, with no rotation and no death-table slot.
 *
 * Actor::Spawn is called through its mangled name rather than a declared method
 * because its signature carries by-value class parameters; see
 * notes/mwccarm-codegen.md 6az.
 */
#include "Actor.h"

struct Vector3_16 { short x, y, z; };

extern "C" Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 actorID, u32 param, const void *pos, const Vector3_16 *rot,
    s32 areaID, s32 deathTableID);

void Actor::SpawnSoundObj(u32 soundObjParam)
{
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x167, soundObjParam, (const void *)&mPosX, (const Vector3_16 *)0,
        (s32)mAreaId, -1);
}
