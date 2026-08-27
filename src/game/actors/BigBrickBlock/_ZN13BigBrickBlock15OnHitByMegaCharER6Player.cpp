//cpp
// @symbol _ZN13BigBrickBlock15OnHitByMegaCharER6Player
#include "BigBrickBlock.h"
#include "Player.h"

/* BigBrickBlock::OnHitByMegaChar -- vtable slot 27, ov002 0x020b36b4.
 * include/dActor_c.h's own slot 27 supplies the signature, `virtual void
 * OnHitByMegaChar(Player &player)` -- corrected from `int` by
 * Stump::OnHitByMegaChar (include/Stump.h); re-verified here rather than
 * assumed.
 *
 * player.IncMegaKillCount() then this class's own Kill (slot 31), reached
 * through an unqualified virtual call -- the same indirect dispatch the
 * pre-migration `c->m()` vtable-slot trick reproduced. */
void BigBrickBlock::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}
