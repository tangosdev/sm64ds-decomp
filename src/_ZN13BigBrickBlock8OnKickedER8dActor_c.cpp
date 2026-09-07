//cpp
// @symbol _ZN13BigBrickBlock8OnKickedER8dActor_c
#include "BigBrickBlock.h"
#include "dActor_c.h"

/* BigBrickBlock::OnKicked -- vtable slot 24, ov002 0x020b36dc.
 *
 * `void`, NOT `int`. include/dActor_c.h declared slot 24 `int` until this
 * override proved it wrong: with two locals and two early returns, mwcc
 * allocates registers differently for `int` vs `void` even though the ROM
 * sets no r0 either way -- measured with tools/mangle.py, not assumed. Every
 * other slot in this file (21/22/23/27) stayed `int` and byte-matches under
 * `int`, so this is specific to this override, not a blanket rule.
 *
 * `actorID` and `other.param1` are fBase_c's own fields. Every call is this
 * class's own Kill (slot 31) through an unqualified virtual call -- the same
 * indirect dispatch the pre-migration `self->m()` vtable-slot trick
 * reproduced. The 0x2e branch calls Kill and falls through without
 * returning, same as the pre-migration recovery.
 *
 * `int b = (actorID == 0x2e);` / `b = (actorID == 0x11);` materialize the
 * bools the same way the pre-migration recovery did -- load-bearing
 * (cpp-bool-widening-cast). */
void BigBrickBlock::OnKicked(dActor_c &other)
{
    int p1;
    int b = (actorID == 0x2e);
    if (b) Kill();
    p1 = other.param1;
    if (p1 == 3) return;
    b = (actorID == 0x11);
    if (b) {
        if (p1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}
