//cpp
// @symbol _ZN10BrickBlock8BehaviorEv
#include "BrickBlock.h"
#include "BigBrickBlock.h"

typedef void (BrickBlock::*BrickBlockAction)();
extern "C" BrickBlockAction data_ov002_0210dd30[];
extern "C" {
extern dActor_c *_ZN8dActor_c4NextEPKS_(const dActor_c *previous);
extern s32 Vec3_Dist(const void *a, const void *b);
}

int BrickBlock::Behavior()
{
    dActor_c *block = 0;
    if (mIsAttached != 0)
        goto attached;

    block = _ZN8dActor_c4NextEPKS_(0);
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
            ((BigBrickBlock *)block)->mLinkedActor = this;
            mIsAttached = 1;
            return 1;
        }
    next:
        block = _ZN8dActor_c4NextEPKS_(block);
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
