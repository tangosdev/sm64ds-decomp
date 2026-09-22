//cpp
// @symbol _ZN13QuestionBlock15OnHitByMegaCharER6Player
#include "QuestionBlock.h"
#include "Player.h"

extern "C" int func_ov102_02149078(void *self);
extern "C" void func_ov102_02149da8(void *self, int state);

/* QuestionBlock::OnHitByMegaChar -- vtable slot 27, ov102 0x02149710.
 * include/dActor_c.h's own slot 27 supplies the signature -- `void`,
 * corrected from `int` by Stump::OnHitByMegaChar (include/Stump.h);
 * re-verified here rather than assumed.
 *
 * `player.param1` is fBase_c's own field, inherited by Player. Same body
 * shape as this class's own OnGroundPounded (slot 21), plus the
 * IncMegaKillCount() call -- see
 * src/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp for the field and
 * helper notes. */
void QuestionBlock::OnHitByMegaChar(Player &player)
{
    if (mState == 1) return;
    if (func_ov102_02149078(this) != 0) return;
    player.IncMegaKillCount();
    mHitterParam = player.param1;
    func_ov102_02149da8(this, 1);
}
