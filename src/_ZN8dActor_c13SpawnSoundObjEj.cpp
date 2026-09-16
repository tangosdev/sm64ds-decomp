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


void dActor_c::SpawnSoundObj(u32 soundObjParam)
{
    dActor_c::Spawn(0x167, soundObjParam, *(const Vector3 *)&mPosX, (const Vector3_16 *)0, (s32)mAreaId, -1);
}
