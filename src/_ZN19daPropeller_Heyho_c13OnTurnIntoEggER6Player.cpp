//cpp
// @symbol _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player
// recovered name: FlyGuy_OnTurnIntoEgg
/* daPropeller_Heyho_c::OnTurnIntoEgg -- vtable slot 19, verified against ov070
 * relocs.txt: _ZTV19daPropeller_Heyho_c (0x02123168) + 0x4c -> 0x021204ec, exactly this
 * placeholder's former address (former name func_ov070_021204ec).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov070).
 */
#include "daPropeller_Heyho_c.h"
#include "Player.h"

int daPropeller_Heyho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
