//cpp
// @symbol _ZN10SlidingIce15OnHitByMegaCharER6Player
#include "SlidingIce.h"
#include "Player.h"

/* SlidingIce::OnHitByMegaChar -- vtable slot 27, ov027 0x0211123c.
 *
 * Same idiom as MovingBar/TowerStep/daObjIceBoard_c::OnHitByMegaChar: Player::
 * IncMegaKillCount is a real method, and the trailing unqualified Kill()
 * call dispatches virtually -- SlidingIce does NOT override slot 31
 * itself (relocs.txt: _ZTV10SlidingIce+0x7c still relocates to
 * dBgActor_c::Kill, ov002 0x020ee55c), so this reaches the inherited
 * base implementation through the vtable, same mechanism as an override
 * would. */
void SlidingIce::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
