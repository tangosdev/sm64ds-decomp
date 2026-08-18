//cpp
// @symbol _ZN13QuestionBlock15OnGroundPoundedER8dActor_c
#include "QuestionBlock.h"
#include "dActor_c.h"

extern "C" int func_ov102_02149078(void *self);
extern "C" void func_ov102_02149da8(void *self, int state);

/* QuestionBlock::OnGroundPounded -- vtable slot 21, ov102 0x02149820.
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`,
 * corrected from `int` by Stump::OnGroundPounded (include/Stump.h);
 * re-verified here rather than assumed.
 *
 * unk_3e8/unk_3f2 are this class's own fields (include/QuestionBlock.h).
 * func_ov102_02149078 and func_ov102_02149da8 are unmigrated helpers shared
 * by all five of this class's combat callbacks; still called by their
 * func_ov102_ names, same as the pre-migration recovery. */
void QuestionBlock::OnGroundPounded(dActor_c &other)
{
    if (unk_3e8 == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    unk_3f2 = other.param1;
    func_ov102_02149da8(this, 1);
}
