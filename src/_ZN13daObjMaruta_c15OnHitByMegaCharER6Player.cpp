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
 * SIGNATURE FROM include/Actor.h's OWN SLOT 27, `virtual int
 * OnHitByMegaChar(Player &player)`, which is where the name comes from. The
 * pre-migration stub returned void and took the player as `void *`; the ROM
 * sets no r0 here either, so the declared `int` return compiles to the same
 * bytes -- the same reading src/_ZN12dScMgSlot1_c15OnHitByMegaCharER6Player.cpp
 * records for its own slot 27.
 *
 * Platform::KillByMegaChar is declared non-virtual in include/Platform.h, so
 * the unqualified call is already the direct `bl` the ROM has. */

extern "C" void func_02012694(int a, void *b);

int daObjMaruta_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    KillByMegaChar(player);
}
