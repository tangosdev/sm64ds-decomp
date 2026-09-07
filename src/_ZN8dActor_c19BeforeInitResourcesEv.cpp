//cpp
/* dActor_c::BeforeInitResources() at 0x02011268, 0x60 bytes -- vtable slot 1.
 *
 * Refuses to spawn an actor that has already been killed this session, then
 * defers to the base.
 *
 *   mFlags & 0x8000000   "spawn even if killed before" -- skips the check
 *   GetBitInDeathTable() nonzero means this actor is recorded as dead
 *
 * When both say the actor should not exist, it marks itself for destruction and
 * returns true anyway: the caller reads the return as "initialisation may
 * proceed", and the destruction request is what actually removes it a frame
 * later. Returning false here would abort init before that request was honoured.
 *
 * The `? 1 : 0` is kept from the pre-migration source because it is visible in
 * the bytes: `ands` then `movne r0,#1` / `moveq r0,#0` materialises the flag
 * test as a 0/1 value before comparing it, which a plain `if (mFlags & bit)`
 * does not do.
 *
 * mFlags is named in include/dActor_c.h at 0x0b0; the old source used a shadow
 * `struct dActor_c { char pad[0xb0]; u32 flags; }`. MarkForDestruction comes from
 * include/fBase_c.h.
 */
#include "dActor_c.h"

bool dActor_c::BeforeInitResources()
{
    int spawnEvenIfKilledBefore;

    spawnEvenIfKilledBefore = (mFlags & 0x8000000) ? 1 : 0;
    if (spawnEvenIfKilledBefore != 0) {
        goto skip;
    }
    if (GetBitInDeathTable() == 0) {
        goto skip;
    }
    MarkForDestruction();
    return true;
skip:;
    {
        int r = fBase_c::BeforeInitResources();
        if (r == 0) return false;
        return true;
    }
}
