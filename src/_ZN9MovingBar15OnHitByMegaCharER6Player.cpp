//cpp
// @symbol _ZN9MovingBar15OnHitByMegaCharER6Player
#include "MovingBar.h"
#include "Player.h"

/* MovingBar::OnHitByMegaChar -- vtable slot 27, ov015 0x02111cb8.
 *
 * Real member function: Player::IncMegaKillCount is a real method
 * (include/Player.h); the trailing unqualified Kill() call reaches this
 * class's own slot-31 override (include/MovingBar.h) through the vtable,
 * same as every other already-migrated MovingBar member. */
void MovingBar::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
