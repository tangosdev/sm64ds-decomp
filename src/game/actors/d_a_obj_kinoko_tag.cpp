//cpp
/* Genuine production translation unit for ov002/daObjKinokoTag_c.
 *
 * RTTI and the contiguous linker run prove that both tag factories belong to
 * this class TU. mwccarm emits ordinary text sections in reverse source order,
 * so the ROM-high factory is defined first. InitResources is the key function;
 * with the inline destructor in the real header, the compiler naturally owns
 * retail D1/D0 plus the class RTTI, type name, and vtable.
 *
 * Both factories construct the same ROM-proven RTTI class. A single
 * daObjKinokoTag_c_classInit spelling would therefore collide, so their
 * evidence-bounded C ABI aliases remain unresolved. Both factories keep
 * retail's measured allocator/base/member/vptr seam because a natural new
 * expression targets the wrong global allocator relocation.
 */

#include "daObjKinokoTag_c.h"
#include "decl_common.h"
#include "Model.h"
#include "SharedFilePtr.h"

extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(dActor_c *actor);
extern void _ZN7dCcAc_cC1Ev(dCcAc_c *clsn);

extern SharedFilePtr data_ov002_0210da30;
extern Fix12i Vec3_Dist(const Vector3 *, const Vector3 *);

/* dCcAc_c::Init takes Fix12<int> values by value. Spelling those parameters
 * naturally makes mwccarm home the register arguments and grows this caller,
 * so this one measured scalar declaration remains a narrow ABI seam. */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *, dActor_c *, Fix12i, Fix12i, u32, u32);
}

extern "C" daObjKinokoTag_c *MegaMushroomTag_Spawn();
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_Spawn();

/* ROM ordinal 8 -- evidence-bounded actor-table C ABI factory. */
// @symbol MegaMushroomTag_Spawn
extern "C" daObjKinokoTag_c *MegaMushroomTag_Spawn()
{
    daObjKinokoTag_c *tag =
        (daObjKinokoTag_c *)_ZN7fBase_cnwEj(sizeof(daObjKinokoTag_c));
    if (tag) {
        _ZN8dActor_cC2Ev(tag);
        *(int *)tag = (int)&_ZTV16daObjKinokoTag_c[2];
        _ZN7dCcAc_cC1Ev(&tag->mMovingCylinderClsn);
    }
    return tag;
}

/* Reconstructed profile-global spelling from the literal ROM registry ID.
 * Exact original SM64DS spelling is not preserved. Historical project alias:
 * MegaMushroomTag_SpawnInfo. */
extern "C" KinokoTagSpawnInfo g_profile_KINOKO_TAG = {
    MegaMushroomTag_Spawn,
    0x0140,
    0x013e,
    0x00000003,
    0x00060000,
    0x00200000,
    0x01000000,
    0
};

/* ROM ordinal 7 -- class-anchored inferred actor-table C ABI factory. */
// @symbol daObjKinokoTag_c_Spawn
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_Spawn()
{
    daObjKinokoTag_c *tag =
        (daObjKinokoTag_c *)_ZN7fBase_cnwEj(sizeof(daObjKinokoTag_c));
    if (tag) {
        _ZN8dActor_cC2Ev(tag);
        *(int *)tag = (int)&_ZTV16daObjKinokoTag_c[2];
        _ZN7dCcAc_cC1Ev(&tag->mMovingCylinderClsn);
    }
    return tag;
}

/* Reconstructed profile-global spelling from the literal ROM registry ID.
 * Exact original SM64DS spelling is not preserved. Historical project alias:
 * MegaMushroomCreateTag_SpawnInfo. */
extern "C" KinokoTagSpawnInfo g_profile_KINOKO_CREATE_TAG = {
    daObjKinokoTag_c_Spawn,
    0x013f,
    0x013d,
    0,
    0,
    0,
    0,
    0
};

/* ROM ordinal 6 -- key function and class-data owner. */
// @symbol _ZN16daObjKinokoTag_c13InitResourcesEv
s32 daObjKinokoTag_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mMovingCylinderClsn, this,
        0x12c000, 0x32000, 0x800002, 0x20);
    mGroupId = param1;
    Model::LoadFile(data_ov002_0210da30);
    return 1;
}

/* ROM ordinal 5. */
// @symbol _ZN16daObjKinokoTag_c8BehaviorEv
s32 daObjKinokoTag_c::Behavior()
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
                    /* The mushroom class header is not trustworthy on this
                     * base. Keep its independently witnessed backlink at
                     * external-object offset 0x32c as the narrow seam. */
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
        daObjKinokoTag_c *tag =
            (daObjKinokoTag_c *)FindWithActorID(0x13f, 0);
        while (tag != 0) {
            if (mGroupId == tag->mGroupId)
                mHasMatchingTag = 1;
            tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, tag);
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

/* ROM ordinal 4. */
// @symbol _ZN16daObjKinokoTag_c16CleanupResourcesEv
s32 daObjKinokoTag_c::CleanupResources()
{
    data_ov002_0210da30.Release();
    return 1;
}

/* ROM ordinal 3 -- inferred private helper spelling. */
// @symbol _ZN16daObjKinokoTag_c20TrySpawnMegaMushroomEv
void daObjKinokoTag_c::TrySpawnMegaMushroom()
{
    if (mMovingCylinderClsn.otherOwner == 0)
        return;
    if ((mMovingCylinderClsn.hitFlags & 0x20) == 0)
        return;
    SpawnMegaMushroom();
}

/* ROM ordinal 2 -- inferred private helper spelling. */
// @symbol _ZN16daObjKinokoTag_c17SpawnMegaMushroomEv
void daObjKinokoTag_c::SpawnMegaMushroom()
{
    daObjKinokoTag_c *tag =
        (daObjKinokoTag_c *)FindWithActorID(0x13f, 0);
    while (tag != 0) {
        if (mGroupId == tag->mGroupId) {
            KinokoPositionWords *tagPos =
                (KinokoPositionWords *)&tag->mPosX;
            KinokoPositionWords spawnPos;
            spawnPos.x = tagPos->x;
            spawnPos.y = tagPos->y;
            spawnPos.z = tagPos->z;

            LandingDustAt(*(Vector3 *)&spawnPos, true);
            dActor_c *mushroom = Spawn(
                0x115, 0xd, *(Vector3 *)&tag->mPosX,
                0, mAreaId, -1);
            mushroom->mVertSpeed = 0x28000;

            tag->KillAndTrackInDeathTable();
            KillAndTrackInDeathTable();
            return;
        }

        tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, tag);
    }
}
