//cpp
/* Genuine production translation unit for ov002/daObjKinokoTag_c.
 *
 * RTTI ov002:0x02108c94 / 0x02108ca0 names daObjKinokoTag_c.
 * overlay_actors.md maps MEGA_MUSHROOM_CREATE_TAG(319) and
 * MEGA_MUSHROOM_TAG(320); the ROM debug table names those profiles
 * KINOKO_CREATE_TAG and KINOKO_TAG. Both factories construct this class.
 * Historical aliases: MegaMushroomTag_Spawn / MegaMushroomTag_SpawnInfo
 * and daObjKinokoTag_c_Spawn / MegaMushroomCreateTag_SpawnInfo.
 * Private helper spellings are inferred; their class ownership, bodies,
 * calls, and ordering are byte/relocation proven.
 *
 * mwccarm emits ordinary text sections in reverse source order, so the
 * ROM-high factory is defined first. InitResources is the key function;
 * with the inline destructor in the real header, the compiler naturally owns
 * retail D1/D0 plus the class RTTI, type name, and vtable.
 *
 * deslop leftovers:
 * - dCcAc_c::Init 6az: InitResources passes Fix12<int> by value; the header
 *   method form size-DIFFs (notes/mwccarm-codegen.md 6az).
 * - KinokoPositionWords: a local Vector3 would emit vague-linkage ~Vector3.
 * - *(Vector3 *)&mPosX addressing shape (a Vector3 member at 0x05c is a
 *   dActor_c campaign, not this leaf).
 * - data_ov002_0210da30 is the ov002 SharedFilePtr handle; symbols.txt has
 *   no recovered name, so it is not coined. SharedFilePtr.h has no fields.
 * - PILE / Stump.h +0x32c backlink (actor ID 0x1b).
 * - Vec3_Dist: no shared header this TU can take without a campaign.
 * - no Player.h / Camera.h.
 * - leaf operator new until #2570.
 */

#include "daObjKinokoTag_c.h"
#include "Model.h"
#include "SharedFilePtr.h"

/* POD view used only to preserve the three-word Vector3 call ABI without
 * emitting Vector3's vague-linkage destructor. */
struct KinokoPositionWords {
    Fix12i x;
    Fix12i y;
    Fix12i z;
};

typedef char KinokoPositionWords_size_must_be_0xc[
    sizeof(KinokoPositionWords) == 0xc ? 1 : -1];

/* Typed owner for the two 0x1c actor/process profile descriptors at
 * ov002:0x02108cb4 and 0x02108cd0. Field roles are recovered from
 * fBase_c/dActor_c consumers; exact original member spellings are not
 * preserved. */
struct KinokoTagSpawnInfo {
    daObjKinokoTag_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char KinokoTagSpawnInfo_size_must_be_0x1c[
    sizeof(KinokoTagSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern SharedFilePtr data_ov002_0210da30;
extern Fix12i Vec3_Dist(const Vector3 *, const Vector3 *);

/* dCcAc_c::Init takes Fix12<int> values by value. Spelling those parameters
 * naturally makes mwccarm home the register arguments and grows this caller,
 * so this one measured scalar declaration remains a narrow ABI seam. */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *, dActor_c *, Fix12i, Fix12i, u32, u32);
}

// @symbol daObjKinokoTag_c_classInit_KINOKO_TAG
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_classInit_KINOKO_TAG()
{
    return new daObjKinokoTag_c();
}

extern "C" KinokoTagSpawnInfo g_profile_KINOKO_TAG = {
    daObjKinokoTag_c_classInit_KINOKO_TAG,
    0x0140,
    0x013e,
    0x00000003,
    0x00060000,
    0x00200000,
    0x01000000,
    0
};

// @symbol daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG()
{
    return new daObjKinokoTag_c();
}

extern "C" KinokoTagSpawnInfo g_profile_KINOKO_CREATE_TAG = {
    daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG,
    0x013f,
    0x013d,
    0,
    0,
    0,
    0,
    0
};

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

// @symbol _ZN16daObjKinokoTag_c8BehaviorEv
s32 daObjKinokoTag_c::Behavior()
{
    dActor_c *other;
    s32 isKinokoTag = (s32)(actorID == 0x140);
    if (isKinokoTag) {
        if (!mSearchedForPile) {
            other = FindWithActorID(0x1b, 0);
            while (other != 0) {
                if (Vec3_Dist((Vector3 *)&mPosX,
                              (Vector3 *)&other->mPosX) < 0x96000) {
                    mHasLinkedPile = 1;
                    /* PILE / Stump.h +0x32c */
                    *(dActor_c **)((u8 *)other + 0x32c) = this;
                    mSearchedForPile = 1;
                    return 1;
                }
                other = FindWithActorID(0x1b, other);
            }
            mSearchedForPile = 1;
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

    isKinokoTag = (s32)(actorID == 0x140);
    if (isKinokoTag) {
        if (mHasLinkedPile == 1) {
            if (mLinkedPileGone)
                SpawnMegaMushroom();
        } else {
            TrySpawnMegaMushroom();
        }
    }

    mMovingCylinderClsn.Clear();
    mMovingCylinderClsn.Update();
    return 1;
}

// @symbol _ZN16daObjKinokoTag_c16CleanupResourcesEv
s32 daObjKinokoTag_c::CleanupResources()
{
    data_ov002_0210da30.Release();
    return 1;
}

// @symbol _ZN16daObjKinokoTag_c20TrySpawnMegaMushroomEv
void daObjKinokoTag_c::TrySpawnMegaMushroom()
{
    if (mMovingCylinderClsn.otherOwner == 0)
        return;
    if ((mMovingCylinderClsn.hitFlags & 0x20) == 0)
        return;
    SpawnMegaMushroom();
}

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
