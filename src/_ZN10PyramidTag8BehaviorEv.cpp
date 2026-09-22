//cpp
// @symbol _ZN10PyramidTag8BehaviorEv

#include "PyramidTag.h"
#include "PyramidTop.h"

s32 PyramidTag::Behavior()
{
    if (mCylinder.otherOwner != 0) {
        if (mPyramidTopID == 0) {
            MarkForDestruction();
            return 1;
        }

        PyramidTop *top = (PyramidTop *)dActor_c::FindWithID(mPyramidTopID);
        if (top != 0)
            ++top->mNumTagsTriggered;

        MarkForDestruction();
        return 1;
    }

    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}
