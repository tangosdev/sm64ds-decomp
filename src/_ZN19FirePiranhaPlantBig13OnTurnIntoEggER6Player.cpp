//cpp
// @symbol _ZN19FirePiranhaPlantBig13OnTurnIntoEggER6Player
// recovered name: FirePiranhaPlantBig_OnTurnIntoEgg
/* daFPkn_c::OnTurnIntoEgg -- vtable slot 19, verified against ov084 relocs.txt:
 * _ZTV19FirePiranhaPlantBig (0x02130b28) + 0x4c -> 0x0212e9d4, exactly this
 * placeholder's former address (former name func_ov084_0212e9d4).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov084).
 */
#include "FirePiranhaPlantBig.h"
#include "Player.h"

int FirePiranhaPlantBig::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, 1, 0);
    KillAndTrackInDeathTable();
}
