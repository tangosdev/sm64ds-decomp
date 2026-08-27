//cpp
// @symbol _ZN6FlyGuy13OnTurnIntoEggER6Player
// recovered name: FlyGuy_OnTurnIntoEgg
/* daPropeller_Heyho_c::OnTurnIntoEgg -- vtable slot 19, verified against ov070
 * relocs.txt: _ZTV6FlyGuy (0x02123168) + 0x4c -> 0x021204ec, exactly this
 * placeholder's former address (former name func_ov070_021204ec).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov070).
 */
#include "FlyGuy.h"
#include "Player.h"

int FlyGuy::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
