//cpp
/* Actor::OnTurnIntoEgg(Player&) at 0x02010154, 0xc bytes -- vtable slot 19.
 *
 * A tail-call veneer to Actor::KillAndTrackInDeathTable at 0x0200f9b8:
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
 * `player` is unused, and the bytes could not tell you otherwise: a tail call
 * never touches r0-r3, so the argument list is invisible here (runbook section
 * 8). The signature comes from the mangled name and include/Actor.h, not from
 * these three words. r0 still holds `this` when the branch is taken, which is
 * exactly what the target -- a non-static member taking nothing -- expects.
 *
 * The slot is declared `int` in include/Actor.h while the target returns void,
 * so this cannot `return` its callee. Falling off the end reproduces the
 * veneer: whatever KillAndTrackInDeathTable leaves in r0 becomes the caller's
 * answer, which is what a tail call means.
 */
#include "Actor.h"

int Actor::OnTurnIntoEgg(Player &player)
{
    KillAndTrackInDeathTable();
}
