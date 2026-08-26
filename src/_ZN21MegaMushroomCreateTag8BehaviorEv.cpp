//cpp
// @symbol _ZN21MegaMushroomCreateTag8BehaviorEv

#include "MegaMushroomCreateTag.h"

extern "C" Fix12i Vec3_Dist(const Vector3 *, const Vector3 *);

s32 MegaMushroomCreateTag::Behavior()
{
    dActor_c *other;
    s32 isCreateTag = (s32)(actorID == 0x140);
    if (isCreateTag) {
        if (!unk_10b) {
            other = FindWithActorID(0x1b, 0);
            while (other != 0) {
                if (Vec3_Dist((Vector3 *)&mPosX,
                              (Vector3 *)&other->mPosX) < 0x96000) {
                    unk_108 = 1;
                    *(dActor_c **)((u8 *)other + 0x32c) = this;
                    unk_10b = 1;
                    return 1;
                }
                other = FindWithActorID(0x1b, other);
            }
            unk_10b = 1;
        }
    }

    if (!unk_10a) {
        MegaMushroomCreateTag *tag =
            (MegaMushroomCreateTag *)FindWithActorID(0x13f, 0);
        while (tag != 0) {
            if (unk_109 == tag->unk_109)
                unk_10a = 1;
            tag = (MegaMushroomCreateTag *)FindWithActorID(0x13f, tag);
        }
        if (!unk_10a)
            MarkForDestruction();
    }

    isCreateTag = (s32)(actorID == 0x140);
    if (isCreateTag) {
        if (unk_108 == 1) {
            if (unk_10c)
                SpawnMegaMushroom();
        } else {
            TrySpawnMegaMushroom();
        }
    }

    mMovingCylinderClsn.Clear();
    mMovingCylinderClsn.Update();
    return 1;
}
