//cpp
// @symbol _ZN9TowerStep15OnHitByMegaCharER6Player
#include "TowerStep.h"
#include "Player.h"

/* TowerStep::OnHitByMegaChar -- vtable slot 27, ov015 0x021123a0.
 *
 * Same idiom as MovingBar::OnHitByMegaChar (this overlay): Player::
 * IncMegaKillCount is a real method, and the trailing unqualified Kill()
 * reaches this class's own slot-31 override (include/TowerStep.h). */
void TowerStep::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
