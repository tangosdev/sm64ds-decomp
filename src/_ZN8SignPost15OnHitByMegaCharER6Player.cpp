//cpp
// @symbol _ZN8SignPost15OnHitByMegaCharER6Player
#include "SignPost.h"
#include "Player.h"

/* SignPost::OnHitByMegaChar -- vtable slot 27, ov002 0x020bb374.
 * reloc: _ZTV8SignPost+0x6c -> 0x020bb374, _ZTV10dBgActor_c+0x6c ->
 * 0x02010130 (different, real override).
 *
 * SIGNATURE FROM include/dActor_c.h's OWN SLOT 27, `virtual void
 * OnHitByMegaChar(Player &player)` -- `int` until Stump::OnHitByMegaChar
 * proved it wrong tree-wide (a64045669). Same body shape
 * src/_ZN13daObjMaruta_c15OnHitByMegaCharER6Player.cpp records for its own
 * slot 27: dBgActor_c::KillByMegaChar is non-virtual, so the unqualified
 * call is already the direct `bl` the ROM has. mAngleY = mPrevAngleY is
 * dActor_c's own field pair (include/dActor_c.h, 0x08e/0x094). */

extern "C" void func_02012694(int a, void *b);

void SignPost::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1d, &mCamSpacePosX);
    KillByMegaChar(player);
    mAngleY = mPrevAngleY;
}
