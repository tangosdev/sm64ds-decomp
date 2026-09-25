//cpp
// @symbol _ZN17daObjBk_Botaosi_c15OnHitByMegaCharER6Player
#include "daObjBk_Botaosi_c.h"
#include "Player.h"

/* daObjBk_Botaosi_c::OnHitByMegaChar -- vtable slot 27, ov015 0x021113c0.
 *
 * Real member function on this class's own named fields (mState, mWobbleTimer --
 * include/daObjBk_Botaosi_c.h) and Player::IncMegaKillCount, a real method
 * (include/Player.h). mState is the same hit-stage counter OnAttacked2/
 * OnKicked's shared helper advances; mWobbleTimer is the same cooldown timer that
 * helper sets to 0x640 after a hit registers. */
void daObjBk_Botaosi_c::OnHitByMegaChar(Player &player)
{
    if (mState >= 2) return;
    player.IncMegaKillCount();
    mState = 2;
    mWobbleTimer = 0x640;
}
