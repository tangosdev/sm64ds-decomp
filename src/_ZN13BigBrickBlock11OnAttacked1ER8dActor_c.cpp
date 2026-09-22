//cpp
// @symbol _ZN13BigBrickBlock11OnAttacked1ER8dActor_c
#include "BigBrickBlock.h"
#include "dActor_c.h"

/* BigBrickBlock::OnAttacked1 -- vtable slot 22, ov002 0x020b37ec. Was a plain
 * C recovery (func_ov002_020b37ec.c) with a single `void *this` parameter --
 * `other` was never read, so the pre-migration stub dropped it from the
 * declared prototype without changing the compiled bytes. The real override
 * keeps `other` in the signature (include/dActor_c.h slot 22) but still never
 * touches it.
 *
 * `actorID` is fBase_c's own field. The single call is this class's own Kill
 * (slot 31), reached through an unqualified virtual call -- the same
 * indirect dispatch the pre-migration `*(int*)((*(int*)c)+0x7c)` vtable read
 * reproduced by hand.
 *
 * `int b = (actorID == 0x11);` materializes the bool the same way the
 * pre-migration recovery did -- load-bearing (cpp-bool-widening-cast). */
int BigBrickBlock::OnAttacked1(dActor_c &other)
{
    int b = (actorID == 0x11);
    if (b) return;
    Kill();
}
