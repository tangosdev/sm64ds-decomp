//cpp
// @symbol _ZN21MegaMushroomCreateTag8BehaviorEv

#include "MegaMushroomCreateTag.h"

extern "C" Fix12i Vec3_Dist(const Vector3 *, const Vector3 *);

s32 MegaMushroomCreateTag::Behavior()
{
    dActor_c *other;
    s32 isCreateTag = (s32)(actorID == 0x140);
    if (isCreateTag) {
        if (!mSearchedForMushroom) {
            other = FindWithActorID(0x1b, 0);
            while (other != 0) {
                if (Vec3_Dist((Vector3 *)&mPosX,
                              (Vector3 *)&other->mPosX) < 0x96000) {
                    mHasLinkedMushroom = 1;
                    *(dActor_c **)((u8 *)other + 0x32c) = this;
                    mSearchedForMushroom = 1;
                    return 1;
                }
                other = FindWithActorID(0x1b, other);
            }
            mSearchedForMushroom = 1;
        }
    }

    if (!mHasMatchingTag) {
        MegaMushroomCreateTag *tag =
            (MegaMushroomCreateTag *)FindWithActorID(0x13f, 0);
        while (tag != 0) {
            if (mGroupId == tag->mGroupId)
                mHasMatchingTag = 1;
            tag = (MegaMushroomCreateTag *)FindWithActorID(0x13f, tag);
        }
        if (!mHasMatchingTag)
            MarkForDestruction();
    }

    isCreateTag = (s32)(actorID == 0x140);
    if (isCreateTag) {
        if (mHasLinkedMushroom == 1) {
            if (mLinkedMushroomGone)
                SpawnMegaMushroom();
        } else {
            TrySpawnMegaMushroom();
        }
    }

    mMovingCylinderClsn.Clear();
    mMovingCylinderClsn.Update();
    return 1;
}
