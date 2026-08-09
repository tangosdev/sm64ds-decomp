//cpp
// @symbol _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as
/* Actor::Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
 *              const Vector3_16 *rot, s8 areaID, s16 deathTableID)
 * at 0x02010e2c.
 *
 * The tree's one entry point for creating an actor from an ID. It parks the
 * placement -- position, rotation, area, death-table slot -- in the staging
 * area that func_02010e78 owns, then asks ActorDerived::Spawn to build the
 * object and parent it to whatever 0x0209f5c0 currently holds.
 *
 * Static: r0 carries actorID, not `this`. Nothing here touches a field.
 *
 * THE LAST TWO PARAMETERS ARE WHY THE SYMBOL WAS RENAMED, ii -> as. They are
 * read `ldrsb r2, [sp, #0x10]` and `ldrsh r3, [sp, #0x14]` -- signed byte and
 * signed halfword. Declared (s32, s32) mwcc emits `ldr` for both, the function
 * comes out 0x5c long instead of 0x4c, and it misses; declared (s8, s16) every
 * one of the 0x4c bytes reproduces. Both were built before this was written.
 *
 * No caller could have caught it, which is why it survived 162 spellings of the
 * wrong name: arguments five and six are passed in stack slots as full words
 * whichever way they are declared, so no call site's bytes depend on the
 * difference. The narrowing is the CALLEE's, and it is visible only here.
 *
 * The return type is recovered here too, and this is the only place it could
 * be. ActorDerived::Spawn is a three-word veneer, so its own definition cannot
 * evidence a return value -- but this function's `bl` is followed straight by
 * the epilogue, so r0 flows out untouched. It returns the actor it built;
 * ActorDerived.h said `void` and now says so correctly.
 */
#include "Actor.h"

extern "C" {
/* The parent every actor spawned through this path is attached to. */
extern ActorBase *data_0209f5c0;

/* 0x02010e78. Stages the placement for the spawn that follows; its own return
   value is discarded here, so nothing about it is evidenced. */
ActorBase *func_02010e78(const Vector3 *pos, const Vector3_16 *rot,
                         s8 areaID, s16 deathTableID);
}

Actor *Actor::Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                    const Vector3_16 *rot, s8 areaID, s16 deathTableID)
{
    func_02010e78(&pos, rot, areaID, deathTableID);
    return (Actor *)ActorDerived::Spawn(actorID, data_0209f5c0, spawnParam, 2);
}
