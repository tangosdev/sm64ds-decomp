//cpp
// @symbol _ZN13BigBrickBlock11OnAttacked2ER8dActor_c
#include "BigBrickBlock.h"
#include "dActor_c.h"

/* BigBrickBlock::OnAttacked2 -- vtable slot 23, ov002 0x020b3788.
 * include/dActor_c.h's own slot 23 supplies the signature, `virtual int
 * OnAttacked2(dActor_c &other)`.
 *
 * `actorID` and `other.param1` are fBase_c's own fields. Both branches call
 * this class's own Kill (slot 31) through an unqualified virtual call -- the
 * same indirect dispatch the pre-migration `(*(Vt**)c)->fn(c)` read
 * reproduced by hand.
 *
 * `int b = (actorID == 0x11);` materializes the bool the same way the
 * pre-migration recovery did -- load-bearing (cpp-bool-widening-cast). */
int BigBrickBlock::OnAttacked2(dActor_c &other)
{
    int b = (actorID == 0x11);
    if (b) {
        if (other.param1 != 2) return;
        Kill();
        return;
    }
    Kill();
}
