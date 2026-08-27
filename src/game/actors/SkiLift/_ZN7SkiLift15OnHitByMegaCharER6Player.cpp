//cpp
// @symbol _ZN7SkiLift15OnHitByMegaCharER6Player
#include "SkiLift.h"
#include "Player.h"

/* SkiLift::OnHitByMegaChar -- vtable slot 27, ov018 0x0211123c.
 *
 * Real member function: Player::IncMegaKillCount and dBgActor_c::
 * KillByMegaChar are both real methods (include/Player.h,
 * include/dBgActor_c.h). unk_094/unk_08e are dActor_c's own mPrevAngleY/
 * mAngleY, real named fields (include/dActor_c.h); mAngleY is bumped by
 * 0x4000 (a quarter turn) off the previous frame's angle. */
void SkiLift::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY + 0x4000;
}
