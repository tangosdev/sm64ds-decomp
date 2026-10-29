//cpp
/**
 * Item-tag blocks: the invisible actors behind BrickBlock item blocks.
 *
 * Each tag attaches to a physical daObjBlockL_c and, once linked,
 * dispatches one of four spawn actions through the PMF table: One-Up
 * mushroom, Mega mushroom, Koopa shell, or Silver Star. The TU is
 * ov002 0x020b415c..0x020b46a0, fourteen functions; it ends with the
 * four per-profile registry factories, each `new daObjBlockItemTag_c()`.
 *
 * Notes:
 * - LoadSilverStarAndNumber and UnloadSilverStarAndNumber are ov002 C
 *   helpers defined outside this TU.
 * - Vec3_Dist keeps a local `const void *` spelling: its header copies
 *   take Vector3 * and live in unrelated leaf headers.
 * - data_ov002_020ff090 (spawn height offsets) and the
 *   data_ov002_0210dd30 PMF table keep linker names; the table is also
 *   referenced by the ov002 sinit.
 */

#include "daObjBlockItemTag_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dActor_c.h"
#include "daObjBlockL_c.h"
#include "dExtSimpleModel_c.h"
#include "daShl_c.h"
#include "daStar_c.h"
#include "daStarBase_c.h"

/* BSS file homes, one per item variant; Init loads them, Cleanup
 * releases them. */
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov002_0210da18;

/* Spawn heights above the tag, indexed by mActionIndex. */
extern s32 data_ov002_020ff090[];

/* PMF action table, indexed by mActionIndex. */
typedef void (daObjBlockItemTag_c::*ItemTagAction)();
extern ItemTagAction data_ov002_0210dd30[];

/* arm9 / ov002 C helpers with no shared header. */
extern "C" {
extern s32 Vec3_Dist(const void *a, const void *b);
extern void LoadSilverStarAndNumber();
}

/* Actor IDs 0xf, 0x10 and 0x11 are three of the daObjBlockL_c brick
 * blocks a tag can attach to. This int-returning predicate is what makes
 * the ROM materialize 0/1 before testing it. */
static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN19daObjBlockItemTag_cD1Ev
// @symbol _ZN19daObjBlockItemTag_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order. D0 additionally returns the object to the actor heap through
 * dActor_c's inline operator delete. */
daObjBlockItemTag_c::~daObjBlockItemTag_c() {}

// @symbol _ZN19daObjBlockItemTag_c11GetSpawnPosER7Vector3RS_
void daObjBlockItemTag_c::GetSpawnPos(Vector3 &destination, daObjBlockItemTag_c &tag) {
    u8 index = tag.mActionIndex;
    s32 z = tag.mPosZ;
    s32 y = tag.mPosY + data_ov002_020ff090[index];
    s32 x = tag.mPosX;

    destination.x = x;
    destination.y = y;
    destination.z = z;
}

// @symbol _ZN19daObjBlockItemTag_c15SpawnKoopaShellEv
void daObjBlockItemTag_c::SpawnKoopaShell() {
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    dActor_c *shell = Spawn(0x11d, 0, spawnPos, 0, mAreaId, -1);
    if (shell)
        ((daShl_c *)shell)->mDespawnTimer = 0xb4;
}

// @symbol _ZN19daObjBlockItemTag_c17SpawnMegaMushroomEv
void daObjBlockItemTag_c::SpawnMegaMushroom() {
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x115, 0, spawnPos, 0, mAreaId, -1);
}

// @symbol _ZN19daObjBlockItemTag_c18SpawnOneUpMushroomEv
void daObjBlockItemTag_c::SpawnOneUpMushroom() {
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x114, 0, spawnPos, 0, mAreaId, -1);
}

// @symbol _ZN19daObjBlockItemTag_c15SpawnSilverStarEv
void daObjBlockItemTag_c::SpawnSilverStar() {
    Vector3 spawnPos;
    dActor_c *starMarker;
    dActor_c *silverStar;

    UntrackStar(mTrackStarID);
    GetSpawnPos(spawnPos, *this);
    starMarker = Spawn(0xb4, 0x50, spawnPos, 0, mAreaId, -1);
    silverStar = Spawn(0xb3, mStarID | 0x10, spawnPos, 0, mAreaId, -1);
    if (starMarker == 0)
        return;
    if (silverStar == 0)
        return;

    ((daStar_c *)silverStar)->mMarkerID = starMarker->uniqueID;
    ((daStarBase_c *)starMarker)->LinkSilverStarAndStarMarker((char *)silverStar);
}

// @symbol _ZN19daObjBlockItemTag_c16CleanupResourcesEv
int daObjBlockItemTag_c::CleanupResources() {
    switch (actorID) {
    case 0x141:
        data_ov002_0210d9d8.Release();
        break;
    case 0x142:
        data_ov002_0210da30.Release();
        break;
    case 0x143:
        data_ov002_0210da18.Release();
        break;
    case 0x144:
        UnloadSilverStarAndNumber();
        break;
    }
    return 1;
}

// @symbol _ZN19daObjBlockItemTag_c8BehaviorEv
int daObjBlockItemTag_c::Behavior() {
    if (mIsAttached == 0) {
        dActor_c *block;
        for (block = dActor_c::Next(0); block != 0; block = dActor_c::Next(block)) {
            if (IsActorID(block, 0x10) || IsActorID(block, 0x0f) || IsActorID(block, 0x11)) {
                if (Vec3_Dist(&mPos, &block->mPos) < 0x32000) {
                    ((daObjBlockL_c *)block)->mLinkedActor = this;
                    mIsAttached = 1;
                    return 1;
                }
            }
        }

        /* Always true after the loop, but the ROM keeps the test. */
        if (block == 0) {
            MarkForDestruction();
            return 1;
        }
    }

    if (mActionPending != 0) {
        int index = mActionIndex;
        (this->*data_ov002_0210dd30[index])();
        MarkForDestruction();
    }
    return 1;
}

// @symbol _ZN19daObjBlockItemTag_c13InitResourcesEv
int daObjBlockItemTag_c::InitResources() {
    mStarID = param1 & 0x7f;
    if (mStarID == 0x7f)
        mStarID = 0;

    switch (actorID) {
    case 0x141:
        mActionIndex = 0;
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9d8);
        break;
    case 0x142:
        mActionIndex = 1;
        dExtSimpleModel_c::LoadFile(data_ov002_0210da30);
        break;
    case 0x143:
        mActionIndex = 2;
        dExtSimpleModel_c::LoadFile(data_ov002_0210da18);
        break;
    case 0x144:
        mActionIndex = 3;
        /* TrackStar's star index is unsigned even though this stored byte is
         * signed at the other ROM load sites. The cast preserves the ldrb. */
        mTrackStarID = TrackStar((u8)mStarID, 1);
        LoadSilverStarAndNumber();
        break;
    }
    return 1;
}

/* The four registry factories, one per spawn profile, in ROM order.
 * Reconstructed source-style names: SM64DS proves daObjBlockItemTag_c
 * through RTTI, allocation size, vtable identity, and the BLK_SLVSTAR_TAG,
 * BLK_GNSHELL_TAG, BLK_SKINOKO_TAG and BLK_OKINOKO_TAG registry profiles;
 * later EAD lineage supplies classInit. Exact original spellings are not
 * preserved. Historical aliases: SilverStarBlockTag_Spawn,
 * GreenShellBlockTag_Spawn, MegaMushroomBlockTag_Spawn,
 * OneUpMushroomBlockTag_Spawn. */
// @symbol daObjBlockItemTag_c_classInit_BLK_SLVSTAR_TAG
extern "C" daObjBlockItemTag_c *daObjBlockItemTag_c_classInit_BLK_SLVSTAR_TAG() {
    return new daObjBlockItemTag_c();
}

// @symbol daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG
extern "C" daObjBlockItemTag_c *daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG() {
    return new daObjBlockItemTag_c();
}

// @symbol daObjBlockItemTag_c_classInit_BLK_SKINOKO_TAG
extern "C" daObjBlockItemTag_c *daObjBlockItemTag_c_classInit_BLK_SKINOKO_TAG() {
    return new daObjBlockItemTag_c();
}

// @symbol daObjBlockItemTag_c_classInit_BLK_OKINOKO_TAG
extern "C" daObjBlockItemTag_c *daObjBlockItemTag_c_classInit_BLK_OKINOKO_TAG() {
    return new daObjBlockItemTag_c();
}

// @symbol data_ov002_0210dd30
ItemTagAction data_ov002_0210dd30[4] = {
    &daObjBlockItemTag_c::SpawnOneUpMushroom, &daObjBlockItemTag_c::SpawnMegaMushroom,
    &daObjBlockItemTag_c::SpawnKoopaShell, &daObjBlockItemTag_c::SpawnSilverStar};
