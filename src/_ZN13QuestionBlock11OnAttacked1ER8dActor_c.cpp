//cpp
// @symbol _ZN13QuestionBlock11OnAttacked1ER8dActor_c
#include "QuestionBlock.h"
#include "dActor_c.h"

extern "C" int func_ov102_02149078(void *self);
extern "C" void func_ov102_02149da8(void *self, int state);

/* QuestionBlock::OnAttacked1 -- vtable slot 22, ov102 0x021497c8.
 * include/dActor_c.h's own slot 22 supplies the signature. Same body shape
 * as this class's own OnGroundPounded (slot 21) -- see
 * src/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp for the field and
 * helper notes. */
int QuestionBlock::OnAttacked1(dActor_c &other)
{
    int v = mState;
    if (v == 1) return;
    if (func_ov102_02149078(this)) return;
    int val = other.param1;
    mHitterParam = val;
    func_ov102_02149da8(this, 1);
}
