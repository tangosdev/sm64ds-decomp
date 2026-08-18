//cpp
// @symbol _ZN13BigBrickBlock15OnGroundPoundedER8dActor_c
#include "BigBrickBlock.h"
#include "dActor_c.h"

/* BigBrickBlock::OnGroundPounded -- vtable slot 21, ov002 0x020b382c.
 *
 * Attributed by the vtable, not the pre-migration `recovered name:` comment
 * (BigBrickBlock_OnGroundPounded, one class too far down -- see
 * actor-class-names-off-by-one). include/dActor_c.h's own slot 21 supplies
 * the signature, `virtual void OnGroundPounded(dActor_c &other)` -- corrected
 * from `int` by Stump::OnGroundPounded (include/Stump.h); re-verified here
 * rather than assumed.
 *
 * `other.param1 == 3` and `actorID == 0x11` are fBase_c's own fields, already
 * named in include/fBase_c.h. Both branches call this class's own Kill
 * (slot 31, include/BigBrickBlock.h) through an unqualified virtual call --
 * the same indirect dispatch the pre-migration `Obj::target()` vtable-slot
 * trick reproduced. Neither branch sets r0, so the bare `return;` in this
 * `void`-returning override compiles to the same bytes as the ROM's stub,
 * which never touches r0 either.
 *
 * `int b = (actorID == 0x11);` materializes the bool into a temporary before
 * branching on it, same as the pre-migration recovery -- the temporary is
 * load-bearing (see cpp-bool-widening-cast): writing the comparison directly
 * as `if (actorID == 0x11)` compiles 3 instructions shorter than the ROM. */
void BigBrickBlock::OnGroundPounded(dActor_c &other)
{
    if (other.param1 == 3) return;
    int b = (actorID == 0x11);
    if (b) {
        if (other.param1 != 2) return;
        Kill();
    } else {
        Kill();
    }
}
