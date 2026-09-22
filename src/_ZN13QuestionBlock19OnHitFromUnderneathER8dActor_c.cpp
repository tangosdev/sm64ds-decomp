//cpp
// @symbol _ZN13QuestionBlock19OnHitFromUnderneathER8dActor_c
#include "QuestionBlock.h"
#include "dActor_c.h"

extern "C" int func_ov102_02149078(void *self);
extern "C" void func_ov102_02149da8(void *self, int state);

/* QuestionBlock::OnHitFromUnderneath -- vtable slot 28, ov102 0x021496a4.
 * include/dActor_c.h's own slot 28 supplies the signature.
 *
 * mVertAccel/mVertSpeed are dActor_c's own fields (include/dActor_c.h),
 * already named there -- this override resets the block's own vertical
 * physics to a fixed pop before running the shared combat-callback body
 * (see src/_ZN13QuestionBlock15OnGroundPoundedER8dActor_c.cpp for the
 * mState/mHitterParam/helper notes). */
/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
int QuestionBlock::OnHitFromUnderneath(dActor_c &other)
{
    if (mState != 1) {
        mVertAccel = -0x8000;
        mVertSpeed = 0x1e000;
        if (!func_ov102_02149078(this)) {
            mHitterParam = other.param1;
            func_ov102_02149da8(this, 1);
        }
    }
}
