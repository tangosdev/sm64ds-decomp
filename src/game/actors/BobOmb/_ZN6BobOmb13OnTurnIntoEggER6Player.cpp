//cpp
// @symbol _ZN6BobOmb13OnTurnIntoEggER6Player
// recovered name: BobOmb_OnTurnIntoEgg
/* daBmb_c::OnTurnIntoEgg -- vtable slot 19, verified against ov102 relocs.txt:
 * _ZTV6BobOmb (0x0214e558) + 0x4c -> 0x0214adc8, exactly this placeholder's
 * former address (former name func_ov102_0214adc8).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov102).
 */
#include "BobOmb.h"
#include "Player.h"
#include "Sound.h"

extern "C" {
extern void GiveCoins(int who, int count);
extern int func_ov102_0214baa0(BobOmb *thiz);
}

int BobOmb::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 1) {
        Sound::PlayBank3(0x11, *(Vector3 *)&mCamSpacePosX);
        GiveCoins(player.mPlayerNo, 1);
        player.Heal(0x100);
    }
    return func_ov102_0214baa0(this);
}
