//cpp
/**
 * Item-tag blocks: the invisible actors behind BrickBlock item blocks.
 *
 * Each tag attaches to a physical daObjBlockL_c and, once linked,
 * dispatches one of four spawn actions through the PMF table: One-Up
 * mushroom, Mega mushroom, Koopa shell, or Silver Star. Factories
 * live outside this TU.
 *
 * Leftover: LinkSilverStarAndStarMarker, LoadSilverStarAndNumber and
 *   UnloadSilverStarAndNumber keep linker names (ov002 helpers);
 *   naming belongs there.
 * Leftover: Vec3_Dist keeps its mangled spelling (no header home).
 * Leftover: data_ov002_020ff090 (spawn height offsets) and the
 *   data_ov002_0210dd30 PMF table keep linker names; the table is
 *   also referenced by the ov002 sinit.
 * Leftover: shell + 0x3c6 is an unnamed Koopa-shell byte this spawn
 *   writes.
 */

#include "daObjBlockItemTag_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dActor_c.h"
#include "daObjBlockL_c.h"
#include "Model.h"

/* BSS file homes, one per item variant; Init loads them, Cleanup
 * releases them. */
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov002_0210da18;

/* Spawn heights above the tag, indexed by mActionIndex. */
extern s32 data_ov002_020ff090[];

/* PMF action table, indexed by mActionIndex. */
typedef void (daObjBlockItemTag_c::*ItemTagAction)();
extern "C" {
extern void LinkSilverStarAndStarMarker(void *starMarker, void *silverStar);
extern s32 Vec3_Dist(const void *a, const void *b);
extern void LoadSilverStarAndNumber();
extern "C" ItemTagAction data_ov002_0210dd30[];
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN19daObjBlockItemTag_cD1Ev
// @symbol _ZN19daObjBlockItemTag_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order. D0 additionally returns the object to the actor heap through
 * dActor_c's inline operator delete. */
daObjBlockItemTag_c::~daObjBlockItemTag_c()
{
}

// @symbol _ZN19daObjBlockItemTag_c11GetSpawnPosER7Vector3RS_
void daObjBlockItemTag_c::GetSpawnPos(Vector3 &destination, daObjBlockItemTag_c &tag)
{
    u8 index = tag.mActionIndex;
    s32 z = tag.mPosZ;
    s32 y = tag.mPosY + data_ov002_020ff090[index];
    s32 x = tag.mPosX;

    destination.x = x;
    destination.y = y;
    destination.z = z;
}

// @symbol _ZN19daObjBlockItemTag_c15SpawnKoopaShellEv
void daObjBlockItemTag_c::SpawnKoopaShell()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    dActor_c *shell = Spawn(0x11d, 0, spawnPos, 0, mAreaId, -1);
    if (shell)
        *(u8 *)((char *)shell + 0x3c6) = 0xb4;
}

// @symbol _ZN19daObjBlockItemTag_c17SpawnMegaMushroomEv
void daObjBlockItemTag_c::SpawnMegaMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x115, 0, spawnPos, 0, mAreaId, -1);
}

// @symbol _ZN19daObjBlockItemTag_c18SpawnOneUpMushroomEv
void daObjBlockItemTag_c::SpawnOneUpMushroom()
{
    Vector3 spawnPos;
    GetSpawnPos(spawnPos, *this);
    Spawn(0x114, 0, spawnPos, 0, mAreaId, -1);
}

// @symbol _ZN19daObjBlockItemTag_c15SpawnSilverStarEv
void daObjBlockItemTag_c::SpawnSilverStar()
{
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

    /* Silver Star's marker-owner unique ID is the word at 0x434. */
    *(u32 *)((char *)silverStar + 0x434) = starMarker->uniqueID;
    LinkSilverStarAndStarMarker(starMarker, silverStar);
}

// @symbol _ZN19daObjBlockItemTag_c16CleanupResourcesEv
int daObjBlockItemTag_c::CleanupResources()
{
  switch(actorID){
  case 0x141: data_ov002_0210d9d8.Release(); break;
  case 0x142: data_ov002_0210da30.Release(); break;
  case 0x143: data_ov002_0210da18.Release(); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}

// @symbol _ZN19daObjBlockItemTag_c8BehaviorEv
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

// @symbol _ZN19daObjBlockItemTag_c13InitResourcesEv
int daObjBlockItemTag_c::InitResources()
{
    mStarID = param1 & 0x7f;
    if (mStarID == 0x7f)
        mStarID = 0;

    switch (actorID) {
    case 0x141:
        mActionIndex = 0;
        Model::LoadFile(data_ov002_0210d9d8);
        break;
    case 0x142:
        mActionIndex = 1;
        Model::LoadFile(data_ov002_0210da30);
        break;
    case 0x143:
        mActionIndex = 2;
        Model::LoadFile(data_ov002_0210da18);
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
