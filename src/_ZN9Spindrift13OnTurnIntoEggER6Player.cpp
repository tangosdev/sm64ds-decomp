//cpp
// @symbol _ZN9Spindrift13OnTurnIntoEggER6Player
// recovered name: Spindrift_OnTurnIntoEgg
/* daHuwa_c::OnTurnIntoEgg -- vtable slot 19, verified against ov081 relocs.txt:
 * _ZTV9Spindrift (0x0212887c) + 0x4c -> 0x02123fd8, exactly this placeholder's
 * former address (former name func_ov081_02123fd8).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov081).
 */
#include "Spindrift.h"
#include "Player.h"

int Spindrift::OnTurnIntoEgg(Player &player)
{
    if (!player.IsCollectingCap())
        player.RegisterEggCoinCount(3, 0, 0);
    else
        GivePlayerCoins(player, 3, 0);
    KillAndTrackInDeathTable();
}
