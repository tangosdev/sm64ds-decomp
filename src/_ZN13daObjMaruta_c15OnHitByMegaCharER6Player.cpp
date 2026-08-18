//cpp
// @symbol _ZN13daObjMaruta_c15OnHitByMegaCharER6Player
#include "daObjMaruta_c.h"
#include "Player.h"
/* daObjMaruta_c::OnHitByMegaChar -- vtable slot 27, ov080 0x02127058.
 *
 * Attributed by the vtable, not by a `recovered name:` comment -- the
 * pre-migration file carried none, only two `void *`. daObjMaruta_c's vtable
 * (ov080 0x02128338) supplies exactly four slots of its own -- 9, 16, 17 and
 * 27 -- and this address is slot 27, so the function is this class's and not
 * daObjFlMaruta_c's or daObjHmMaruta_c's; both leaves inherit it.
 * include/daObjMaruta_c.h's banner already says so from the other direction.
 *
 * SIGNATURE FROM include/dActor_c.h's OWN SLOT 27, `virtual void
 * OnHitByMegaChar(Player &player)` -- `int` until Stump::OnHitByMegaChar
 * (include/Stump.h) proved it wrong tree-wide; a no-op re-verify for this
 * file, which has neither locals nor an early return, same reading
 * src/_ZN12dScMgSlot1_c15OnHitByMegaCharER6Player.cpp records for its own
 * slot 27.
 *
 * dBgActor_c::KillByMegaChar is declared non-virtual in include/dBgActor_c.h, so
 * the unqualified call is already the direct `bl` the ROM has. */

extern "C" void func_02012694(int a, void *b);

void daObjMaruta_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    KillByMegaChar(player);
}
