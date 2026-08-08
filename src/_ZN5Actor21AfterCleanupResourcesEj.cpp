//cpp
/* Actor::AfterCleanupResources(u32) at 0x02011214, 0xc bytes -- vtable slot 5.
 *
 * A tail-call veneer to ActorBase::AfterCleanupResources at 0x02043b2c:
 *
 *     ldr ip, [pc]
 *     bx  ip
 *     .word 0x02043b2c
 *
 * Actor overrides the slot only to hand it straight back to its base. Runbook
 * section 8: a veneer is emitted for the CALL, not for the argument list, so
 * restoring the real signature costs nothing and the bytes are indifferent to
 * it. The parameter therefore cannot be evidenced from these three words -- it
 * is `u32 vfSuccess` because include/ActorBase.h and include/Actor.h both
 * declare the slot that way, and the qualified call below is what emits the
 * direct branch.
 */
#include "Actor.h"

void Actor::AfterCleanupResources(u32 vfSuccess)
{
    ActorBase::AfterCleanupResources(vfSuccess);
}
