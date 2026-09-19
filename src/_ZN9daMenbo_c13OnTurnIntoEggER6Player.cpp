//cpp
// @symbol _ZN9daMenbo_c13OnTurnIntoEggER6Player
// recovered name: daMenbo_c_OnTurnIntoEgg
/* daMenbo_c::OnTurnIntoEgg -- vtable slot 19, verified against ov090 relocs.txt:
 * _ZTV9daMenbo_c (0x02134168) + 0x4c -> 0x02132620, exactly this placeholder's
 * former address (former name func_ov090_02132620).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov090).
 */
#include "daMenbo_c.h"
#include "Player.h"

void daMenbo_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
