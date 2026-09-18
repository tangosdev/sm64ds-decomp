//cpp
// @symbol _ZN14daObjBk_Lift_c15OnHitByMegaCharER6Player
#include "daObjBk_Lift_c.h"
#include "Player.h"

/* daObjBk_Lift_c::OnHitByMegaChar -- vtable slot 27, ov015 0x021123a0.
 *
 * Same idiom as daObjBk_Dossunbar_c::OnHitByMegaChar (this overlay): Player::
 * IncMegaKillCount is a real method, and the trailing unqualified Kill()
 * reaches this class's own slot-31 override (include/daObjBk_Lift_c.h). */
void daObjBk_Lift_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
