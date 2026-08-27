//cpp
// @symbol _ZN9PushBlock15OnHitByMegaCharER6Player
#include "PushBlock.h"
#include "Player.h"

/* PushBlock::OnHitByMegaChar -- vtable slot 27, ov002 0x020b8d14. Attributed by
 * the vtable: _ZTV9PushBlock carries 0x020b8d14 at slot 27 -- vtable + 0x6c --
 * where _ZTV10dBgActor_c carries dActor_c's generic 0x02010124. Confirmed with
 * tools/mangle.py: _ZN9PushBlock15OnHitByMegaCharER6Player.
 *
 * Same idiom as SlidingIce::OnHitByMegaChar: Player::IncMegaKillCount is a real
 * method, and the trailing unqualified Kill() dispatches virtually through slot
 * 31. PushBlock does override slot 31 (its own _ZN9PushBlock4KillEv), so this
 * reaches PushBlock::Kill through the vtable rather than the base. */
void PushBlock::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
