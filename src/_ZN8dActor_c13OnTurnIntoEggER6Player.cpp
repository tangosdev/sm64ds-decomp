//cpp
/* dActor_c::OnTurnIntoEgg(Player&) at 0x02010154, 0xc bytes -- vtable slot 19.
 *
 * A tail-call veneer to dActor_c::KillAndTrackInDeathTable at 0x0200f9b8:
 *
 *     ldr ip, [pc]
 *     bx  ip
 *     .word 0x0200f9b8
 *
 * The behaviour reads straight off the target once it is named: when Yoshi
 * swallows an actor and turns it into an egg, the actor removes itself the same
 * way it would if killed -- record it in the death table so it does not respawn,
 * then mark it for destruction. Turning into an egg IS dying, as far as the
 * actor list is concerned. Leaf classes override the slot when they want
 * something else to happen.
 *
 * `player` is unused, and the bytes could not tell you otherwise: this veneer
 * does not change r0-r3, so the argument list is invisible here (runbook section
 * 8). Player& is the reconstructed interface recorded in include/dActor_c.h;
 * the project-assigned mangled name does not independently prove it from ROM.
 * r0 still holds `this` when the branch is taken, which is
 * exactly what the target -- a non-static member taking nothing -- expects.
 *
 * The shared slot returns void, like KillAndTrackInDeathTable. The void
 * forwarding body reproduces the veneer without inventing a return value.
 * Actor override bodies and the observed Player call sites support this
 * reconstruction; the veneer alone cannot establish the original return type.
 */
#include "dActor_c.h"

void dActor_c::OnTurnIntoEgg(Player &player)
{
    KillAndTrackInDeathTable();
}
