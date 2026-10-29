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
 * Notes:
 * - data_ov002_0210da30 is the ov002 SharedFilePtr handle; symbols.txt has
 *   no recovered name, so it is not coined.
 * - Behavior links the nearest PILE (actor ID 0x1b) back to this tag through
 *   daObjPile_c::mLinkedTag.
 */

#include "daObjKinokoTag_c.h"
#include "SpawnInfo.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "daObjPile_c.h"
#include "math/CalcLib.h"

extern SharedFilePtr data_ov002_0210da30;

// @symbol daObjKinokoTag_c_classInit_KINOKO_TAG
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_classInit_KINOKO_TAG() {
    return new daObjKinokoTag_c();
}

/* The actor profile at ov002:0x02108cb4. */
extern "C" SpawnInfo<daObjKinokoTag_c> g_profile_KINOKO_TAG = {
    daObjKinokoTag_c_classInit_KINOKO_TAG,
    0x0140,
    0x013e,
    0x00000003,
    0x00060000,
    0x00200000,
    0x01000000,
    0};

// @symbol daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG
extern "C" daObjKinokoTag_c *daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG() {
    return new daObjKinokoTag_c();
}

/* The actor profile at ov002:0x02108cd0. */
extern "C" SpawnInfo<daObjKinokoTag_c> g_profile_KINOKO_CREATE_TAG = {
    daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG, 0x013f, 0x013d, 0, 0, 0, 0, 0};

// @symbol _ZN16daObjKinokoTag_c13InitResourcesEv
s32 daObjKinokoTag_c::InitResources() {
    mMovingCylinderClsn.Init(this, 0x12c000, 0x32000, 0x800002, 0x20);
    mGroupId = param1;
    dExtSimpleModel_c::LoadFile(data_ov002_0210da30);
    return 1;
}

// @symbol _ZN16daObjKinokoTag_c8BehaviorEv
s32 daObjKinokoTag_c::Behavior() {
    dActor_c *other;
    s32 isKinokoTag = (s32)(actorID == 0x140);
    if (isKinokoTag) {
        if (!mSearchedForPile) {
            other = FindWithActorID(0x1b, 0);
            while (other != 0) {
                if (Vec3_Dist(&mPos, &other->mPos) < 0x96000) {
                    mHasLinkedPile = 1;
                    ((daObjPile_c *)other)->mLinkedTag = this;
                    mSearchedForPile = 1;
                    return 1;
                }
                other = FindWithActorID(0x1b, other);
            }
            mSearchedForPile = 1;
        }
    }

    if (!mHasMatchingTag) {
        daObjKinokoTag_c *tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, 0);
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
s32 daObjKinokoTag_c::CleanupResources() {
    data_ov002_0210da30.Release();
    return 1;
}

// @symbol _ZN16daObjKinokoTag_c20TrySpawnMegaMushroomEv
void daObjKinokoTag_c::TrySpawnMegaMushroom() {
    if (mMovingCylinderClsn.otherOwner == 0)
        return;
    if ((mMovingCylinderClsn.hitFlags & 0x20) == 0)
        return;
    SpawnMegaMushroom();
}

// @symbol _ZN16daObjKinokoTag_c17SpawnMegaMushroomEv
void daObjKinokoTag_c::SpawnMegaMushroom() {
    daObjKinokoTag_c *tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, 0);
    while (tag != 0) {
        if (mGroupId == tag->mGroupId) {
            Vector3 *tagPos = &tag->mPos;
            Vector3 spawnPos;
            spawnPos.x = tagPos->x;
            spawnPos.y = tagPos->y;
            spawnPos.z = tagPos->z;

            LandingDustAt(spawnPos, true);
            dActor_c *mushroom = Spawn(0x115, 0xd, tag->mPos, 0, mAreaId, -1);
            mushroom->mVertSpeed = 0x28000;

            tag->KillAndTrackInDeathTable();
            KillAndTrackInDeathTable();
            return;
        }

        tag = (daObjKinokoTag_c *)FindWithActorID(0x13f, tag);
    }
}
