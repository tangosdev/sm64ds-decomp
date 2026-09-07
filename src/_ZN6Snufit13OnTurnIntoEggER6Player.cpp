//cpp
// @symbol _ZN6Snufit13OnTurnIntoEggER6Player
// recovered name: Snufit_OnTurnIntoEgg
/* daYurei_Mucho_c::OnTurnIntoEgg -- vtable slot 19, verified against ov065
 * relocs.txt: _ZTV6Snufit (0x0211cba4) + 0x4c -> 0x02116f14, exactly this
 * placeholder's former address (former name func_ov065_02116f14).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov065).
 */
#include "Snufit.h"
#include "Player.h"

int Snufit::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
