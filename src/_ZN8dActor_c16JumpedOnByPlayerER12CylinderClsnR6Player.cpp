//cpp
/* Actor::JumpedOnByPlayer(CylinderClsn&, Player&) at 0x0201054c, 0x80 bytes.
 *
 * "Did that player land on me, as opposed to walking into me?" Two different
 * tests, chosen by whether the cylinder reports a side hit.
 *
 *   clsn.flags & 6 set -- the geometry already says the contact was lateral,
 *   so being jumped on has to be argued from state instead: the player must be
 *   falling, must be airborne, and must be more than 0xa000 (10.0 in 20.12)
 *   above this actor's own Y. The height slack is what stops a player scraping
 *   down a wall beside a tall actor from counting as a stomp.
 *
 *   otherwise -- the cylinder's own pushback Y (0x10) decides. The player must
 *   be falling, the pushback must be downward, and it must be SMALLER in
 *   magnitude than the fall speed. That last comparison is `c10 > vel` with
 *   both negative, which reads backwards until you notice both are negative.
 *
 * A member: r0 is `this` and is read at `ldr r2, [r0, #0x60]` for mPosY, with
 * the clsn in r1 and the player in r2.
 *
 * THE `volatile s32 pad[4]` IS LOAD-BEARING and is not a placeholder. The ROM
 * opens `sub sp, sp, #0x10` and never stores anything in that space -- 16
 * bytes of stack frame nothing uses. Measured both ways under 2004/b56: with
 * the array the .text is 0x80, matching the ROM; without it, 0x6c. Twenty
 * bytes, because dropping the frame also removes the matching `add sp` from
 * each of the four returns. `volatile` and the `(void)&pad` are both needed to
 * stop it being optimised away.
 *
 * Unlike the pointer casts removed from BeforeBehavior and MakeVanishLuigiWork
 * -- which turned out to be doing nothing -- this one was tested the same way
 * and the bytes DO depend on it. Same test, opposite answer; that is why it is
 * worth running rather than guessing.
 */
#include "Actor.h"
#include "Player.h"
#include "CylinderClsn.h"

int Actor::JumpedOnByPlayer(CylinderClsn &clsn, Player &player)
{
    volatile s32 pad[4];
    s32 vel = player.mVertSpeed;

    (void)&pad;
    if (clsn.flags & 6) {
        if (vel < 0 && player.mIsAirborne != 0
            && mPosY < player.mPosY - 0xa000)
            return 1;
        return 0;
    } else {
        s32 pushbackY = clsn.pushback.y;
        if (vel < 0 && pushbackY < 0 && pushbackY > vel)
            return 1;
        return 0;
    }
}
