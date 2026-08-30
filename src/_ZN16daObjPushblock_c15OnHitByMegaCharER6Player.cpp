//cpp
// @symbol _ZN16daObjPushblock_c15OnHitByMegaCharER6Player
#include "daObjPushblock_c.h"
#include "Player.h"

/* daObjPushblock_c::OnHitByMegaChar -- vtable slot 27, ov002 0x020b8d14. Attributed by
 * the vtable: _ZTV16daObjPushblock_c carries 0x020b8d14 at slot 27 -- vtable + 0x6c --
 * where _ZTV10dBgActor_c carries dActor_c's generic 0x02010124. Confirmed with
 * tools/mangle.py: _ZN16daObjPushblock_c15OnHitByMegaCharER6Player.
 *
 * Same idiom as SlidingIce::OnHitByMegaChar: Player::IncMegaKillCount is a real
 * method, and the trailing unqualified Kill() dispatches virtually through slot
 * 31. daObjPushblock_c does override slot 31 (its own _ZN16daObjPushblock_c4KillEv), so this
 * reaches daObjPushblock_c::Kill through the vtable rather than the base. */
void daObjPushblock_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
