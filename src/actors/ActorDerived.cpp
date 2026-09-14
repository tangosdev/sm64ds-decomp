//cpp
/* dBase_c -- the middle link of the actor hierarchy: fBase_c -> dBase_c ->
 * dActor_c (arm9 0x02013e80..0x02013f28).
 *
 * Adds no members and no new virtuals; AfterInitResources (slot 2) is the key
 * function, so this TU owns the vtable with D1/D0 dragged in beside it and no
 * forcing scaffold. Spawn veneers the actor factory; func_02013edc veneers the
 * scene-spawn path. Written last-to-first: mwccarm emits .text in reverse
 * source order. Size is asserted in include/dBase_c.h (0x50, fBase_c's own).
 *
 * deslop leftovers:
 * - func_02013edc keeps its ROM label: the scene-creation veneer, enclosed by
 *   this class's own run. Spelled void although its only caller
 *   (dScene_c::SpawnIfNecessary) reads r0 as the spawned scene actor -- a
 *   0xc-byte veneer cannot evidence a return value, the same doctrine
 *   include/dBase_c.h records for Spawn.
 * - Spawn's trailing (int, int) stay unnamed: dActor_c::Spawn passes
 *   (spawnParam, 2) and other callers (mode/entranceId, 0), and both words
 *   ride r2/r3 through this veneer into func_02042ffc's factory call, but
 *   their roles belong to the factory's TU (func_02043098), not this one.
 * - func_02042ffc / func_02042fe4 keep their ROM labels for the same reason.
 */
#include "dBase_c.h"

extern "C" {
extern void func_02042fe4(int a, int b, int c);
extern fBase_c *func_02042ffc(u32 actorID, fBase_c *parent);
}

void dBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    fBase_c::AfterInitResources(vfSuccess);
}

fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int, int)
{
    return func_02042ffc(actorID, parent);
}

extern "C" void func_02013edc(int a, int b, int c)
{
    func_02042fe4(a, b, c);
}
