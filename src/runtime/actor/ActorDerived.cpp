//cpp
/* dBase_c -- the middle link of the actor hierarchy: fBase_c -> dBase_c ->
 * dActor_c (arm9 0x02013e80..0x02013f4c).
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
 * - Spawn's trailing (int, int) keep the header's a/b: dActor_c::Spawn
 *   passes (spawnParam, 2) and other callers (mode/entranceId, 0), and this
 *   veneer hands both straight on to func_02042ffc's factory call, but their
 *   roles belong to the factory's TU (func_02043098), not this one.
 * - func_02042ffc / func_02042fe4 keep their ROM labels for the same reason.
 */
#include "dBase_c.h"

extern "C" {
extern int func_02042fe4(int a, unsigned int b, int c);
extern fBase_c *func_02042ffc(u32 actorID, fBase_c *parent, u32 a, int b);
/* local extern: no header declares it; the next ROM entry's crash-forward
 * target, called only by the absorbed spin below. */
extern void func_02013f4c(void);
}

/* Absorbed from the retired arm9 helper func_02013f28
 * (arm9 0x02013f28..0x02013f4c): fatal-error spin, IME off then call the
 * next entry forever. Defined first because this file emits in reverse
 * source order, so the tail body lands at the run's right edge. */
extern "C" void func_02013f28(void)
{
    *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 0;
    for (;;)
        func_02013f4c();
}

void dBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    fBase_c::AfterInitResources(vfSuccess);
}

fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int a, int b) {
    return func_02042ffc(actorID, parent, a, b);
}

extern "C" int func_02013edc(int a, int b, int c) {
    return func_02042fe4(a, b, c);
}
