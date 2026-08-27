//cpp
// @symbol _ZN5Swoop13OnTurnIntoEggER6Player
// recovered name: Swoop_OnTurnIntoEgg
/* daBasabasa_c::OnTurnIntoEgg -- vtable slot 19, verified against ov065 relocs.txt:
 * _ZTV5Swoop (0x0211cca0) + 0x4c -> 0x02117eb4, exactly this placeholder's
 * former address (former name func_ov065_02117eb4).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov065).
 */
#include "Swoop.h"
#include "Player.h"

int Swoop::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
