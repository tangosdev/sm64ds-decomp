//cpp
// @symbol _ZN6Player18St_Grabbed_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * Clears bit 1 of the body-collision flags, then if something is holding the
 * player at +0x35c and its actorID is 0xbf, tells it to drop and clears the
 * slot. Note the callee is _ZN6Player9DropActorEv -- Player::DropActor() --
 * invoked with THAT actor as its this, not the player's: whatever occupies
 * 0x35c is itself a Player. Kept as an extern "C" call rather than a method
 * call so the source does not assert a type this function cannot evidence.
 *
 * 0x35c is unk_35c, NOT mHeldObj at 0x358 -- those are different slots; the
 * object the player holds and the actor holding the player.
 */
#include "Player.h"
extern "C" {
extern void _ZN6Player9DropActorEv(void* a);
}

int Player::St_Grabbed_Cleanup()
{
    void* a;
    *(u32*)&mMovingCylinderClsnWithPos.flags &= ~2;
    a = (void*)unk_35c;
    if (a) {
        int isBob = *(u16*)((char*)a + 0xc) == 0xbf;
        if (isBob) {
            _ZN6Player9DropActorEv(a);
            unk_35c = 0;
        }
    }
    return 1;
}
