//cpp
// @symbol _ZN5Stump15OnHitByMegaCharER6Player
#include "Stump.h"
#include "Player.h"

/* Stump::OnHitByMegaChar -- vtable slot 27, ov091 0x021335d4.
 * include/dActor_c.h's own slot 27 supplies the signature -- `void`, the
 * correction this override forced (int gave a 6-word mismatch, all in
 * register choice; see include/dActor_c.h and include/Stump.h).
 *
 * mBusy/mState/mAttackCooldown are this class's own fields
 * (include/Stump.h). PoofDust is dActor_c's own, called unqualified.
 * func_02012694 and func_ov091_021334b8 (this class's own) keep their
 * verified ROM symbols. */
extern "C" int func_02012694(int a, void *b);
extern "C" int func_ov091_021334b8(void *self, int b);

void Stump::OnHitByMegaChar(Player &player)
{
    if (mBusy) return;
    if (&player == 0) return;
    if (mState == 0) return;
    if (mAttackCooldown) return;
    player.IncMegaKillCount();
    PoofDust();
    func_02012694(0x62, &mCamSpacePosX);
    func_ov091_021334b8(this, 1);
}
