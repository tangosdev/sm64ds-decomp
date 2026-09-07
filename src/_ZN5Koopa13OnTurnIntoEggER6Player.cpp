//cpp
// @symbol _ZN5Koopa13OnTurnIntoEggER6Player
// recovered name: Koopa_OnTurnIntoEgg
/* daNknk_c::OnTurnIntoEgg -- vtable slot 19, verified against ov062 relocs.txt:
 * _ZTV5Koopa (0x0211dab4) + 0x4c -> 0x02119628, exactly this placeholder's
 * former address (former name func_ov062_02119628).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov062).
 */
#include "Koopa.h"
#include "Player.h"

#pragma optimize_for_size on
int Koopa::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 3) {
        if (OnYoshiTryEat() == 6 && !player.IsCollectingCap()) {
            player.RegisterEggCoinCount(0, 0, 1);
        } else {
            GivePlayerCoins(player, 1, 2);
        }
    }
    KillAndTrackInDeathTable();
}
