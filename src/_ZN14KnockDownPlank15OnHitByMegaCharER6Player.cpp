//cpp
// @symbol _ZN14KnockDownPlank15OnHitByMegaCharER6Player
#include "KnockDownPlank.h"
#include "Player.h"

/* KnockDownPlank::OnHitByMegaChar -- vtable slot 27, ov015 0x021113c0.
 *
 * Real member function on this class's own named fields (unk_397, unk_394 --
 * include/KnockDownPlank.h) and Player::IncMegaKillCount, a real method
 * (include/Player.h). unk_397 is the same hit-stage counter OnAttacked2/
 * OnKicked's shared helper advances; unk_394 is the same cooldown timer that
 * helper sets to 0x640 after a hit registers. */
void KnockDownPlank::OnHitByMegaChar(Player &player)
{
    if (unk_397 >= 2) return;
    player.IncMegaKillCount();
    unk_397 = 2;
    unk_394 = 0x640;
}
