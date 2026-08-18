//cpp
/* dActor_c::AfterBehavior(u32) at 0x02010fc8, 0xc bytes -- vtable slot 8.
 *
 * A tail-call veneer to fBase_c::AfterBehavior at 0x02043af8:
 *
 *     ldr ip, [pc]
 *     bx  ip
 *     .word 0x02043af8
 *
 * dActor_c overrides the slot only to hand it straight back to its base. Runbook
 * section 8: a veneer is emitted for the CALL, not for the argument list, so
 * restoring the real signature costs nothing and the bytes are indifferent to
 * it. The parameter therefore cannot be evidenced from these three words -- it
 * is `u32 vfSuccess` because include/fBase_c.h and include/dActor_c.h both
 * declare the slot that way, and the qualified call below is what emits the
 * direct branch.
 */
#include "dActor_c.h"

void dActor_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}
