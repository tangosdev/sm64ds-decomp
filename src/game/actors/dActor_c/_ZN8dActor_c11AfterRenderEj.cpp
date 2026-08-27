//cpp
/* dActor_c::AfterRender(u32) at 0x02010f6c, 0xc bytes -- vtable slot 11.
 *
 * A tail-call veneer to fBase_c::AfterRender at 0x02043ac4:
 *
 *     ldr ip, [pc]
 *     bx  ip
 *     .word 0x02043ac4
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

void dActor_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}
