//cpp
// @symbol _ZN7Skeeter13OnTurnIntoEggER6Player
// recovered name: Skeeter_OnTurnIntoEgg
/* daMenbo_c::OnTurnIntoEgg -- vtable slot 19, verified against ov090 relocs.txt:
 * _ZTV7Skeeter (0x02134168) + 0x4c -> 0x02132620, exactly this placeholder's
 * former address (former name func_ov090_02132620).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov090).
 */
#include "Skeeter.h"
#include "Player.h"

int Skeeter::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}
