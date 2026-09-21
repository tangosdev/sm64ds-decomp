//cpp
#include "dActor_c.h"
// @symbol _ZN19daObjBlockItemTag_c8BehaviorEv
#include "daObjBlockItemTag_c.h"
#include "daObjBlockL_c.h"

typedef void (daObjBlockItemTag_c::*BrickBlockAction)();
extern "C" BrickBlockAction data_ov002_0210dd30[];
extern "C" {
extern s32 Vec3_Dist(const void *a, const void *b);
}

int daObjBlockItemTag_c::Behavior()
{
    dActor_c *block = 0;
    if (mIsAttached != 0)
        goto attached;

    block = dActor_c::Next(0);
    while (block) {
        u16 type = block->actorID;
        int isBlock;
        isBlock = (type == 0x10);
        if (!isBlock) {
            isBlock = (type == 0x0f);
            if (!isBlock) {
                isBlock = (type == 0x11);
                if (!isBlock)
                    goto next;
            }
        }
        if (Vec3_Dist(&mPosX, &block->mPosX) < 0x32000) {
            ((daObjBlockL_c *)block)->mLinkedActor = this;
            mIsAttached = 1;
            return 1;
        }
    next:
        block = dActor_c::Next(block);
    }

    if (block)
        goto attached;
    MarkForDestruction();
    return 1;

attached:
    if (mActionPending != 0) {
        int index = mActionIndex;
        (this->*data_ov002_0210dd30[index])();
        MarkForDestruction();
    }
    return 1;
}
