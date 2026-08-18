//cpp
// @symbol _ZN8IceSheet15OnHitByMegaCharER6Player
#include "IceSheet.h"
#include "Player.h"

/* IceSheet::OnHitByMegaChar -- vtable slot 27, ov018 0x02112858.
 *
 * Same idiom as MovingBar/TowerStep::OnHitByMegaChar: Player::
 * IncMegaKillCount is a real method, and the trailing unqualified Kill()
 * reaches this class's own slot-31 override (include/IceSheet.h). */
void IceSheet::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
