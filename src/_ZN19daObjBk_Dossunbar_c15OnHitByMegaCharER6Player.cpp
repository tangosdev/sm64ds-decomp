//cpp
// @symbol _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player
#include "daObjBk_Dossunbar_c.h"
#include "Player.h"

/* daObjBk_Dossunbar_c::OnHitByMegaChar -- vtable slot 27, ov015 0x02111cb8.
 *
 * Real member function: Player::IncMegaKillCount is a real method
 * (include/Player.h); the trailing unqualified Kill() call reaches this
 * class's own slot-31 override (include/daObjBk_Dossunbar_c.h) through the vtable,
 * same as every other already-migrated daObjBk_Dossunbar_c member. */
void daObjBk_Dossunbar_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
