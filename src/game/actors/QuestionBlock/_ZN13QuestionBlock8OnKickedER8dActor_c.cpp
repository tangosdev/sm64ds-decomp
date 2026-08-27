//cpp
// @symbol _ZN13QuestionBlock8OnKickedER8dActor_c
#include "QuestionBlock.h"
#include "dActor_c.h"

extern "C" int func_ov102_02149078(void *self);
extern "C" void func_ov102_02149da8(void *self, int state);

/* QuestionBlock::OnKicked -- vtable slot 24, ov102 0x02149770.
 *
 * `void`, not `int`: include/dActor_c.h's slot 24 was corrected to `void` by
 * BigBrickBlock::OnKicked (see include/BigBrickBlock.h and
 * src/_ZN8dActor_c8OnKickedERS_.cpp) and every override of it must match.
 *
 * Same body shape as this class's own OnGroundPounded (slot 21) -- see
 * src/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp for the field and
 * helper notes. */
void QuestionBlock::OnKicked(dActor_c &other)
{
    if (mState == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    mHitterParam = other.param1;
    func_ov102_02149da8(this, 1);
}
