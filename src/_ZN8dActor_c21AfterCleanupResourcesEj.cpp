//cpp
/* dActor_c::AfterCleanupResources(u32) at 0x02011214, 0xc bytes -- vtable slot 5.
 *
 * A tail-call veneer to fBase_c::AfterCleanupResources at 0x02043b2c:
 *
 *     ldr ip, [pc]
 *     bx  ip
 *     .word 0x02043b2c
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

void dActor_c::AfterCleanupResources(u32 vfSuccess)
{
    fBase_c::AfterCleanupResources(vfSuccess);
}
