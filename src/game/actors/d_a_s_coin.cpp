//cpp
/* Secret Coin (SECRET_COIN 329) -- ov002/daSCoin_c. RTTI ov002:0x0210b000
 * names daSCoin_c. Collecting all five coins of a group spawns POWER_STAR
 * at the STARBASE marker with the same mStarID.
 *
 * Function order is the reverse of the ROM's: mwccarm 2004/b56 emits one
 * .text section per function in reverse source order.
 *
 * deslop leftovers:
 * - dCcAc_c::Init stays a scalar extern: it takes Fix12<int> by value and
 *   the member form size-DIFFs (mwccarm-codegen wall 6az).
 * - SpawnStar copies the marker's position through an int * at its mPosX:
 *   member reads (marker->mPos.x or marker->mPosX, ...) size-DIFF.
 * - LinkToBlock's ID tests go through the int-returning HasActorID: a
 *   direct `actorID == id` in the for/else-if loop size-DIFFs.
 * - data_ov002_0210d9a8 (the ov002 SharedFilePtr handle) and func_02012790
 *   (the no-position jingle player, id 0x25 here) have no recovered names.
 */

#include "daSCoin_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Model.h"
#include "daStar_c.h"
#include "daStarBase_c.h"
#include "daObjHatenaBlock_c.h"
#include "daObjPushblock_c.h"

extern "C" {
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
unsigned char DecIfAbove0_Byte(unsigned char *p);
unsigned int func_02012790(unsigned int);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor,
                                              int radius, int height,
                                              unsigned int flags,
                                              unsigned int vulnFlags);
extern SharedFilePtr data_ov002_0210d9a8;
}

/* int-returning, so the ROM materializes the 0/1 of each ID test before
 * branching on it; a direct `actorID == id` does not. */
static inline int HasActorID(dActor_c *actor, unsigned int id) {
    return actor->actorID == id;
}

// @symbol daSCoin_c_classInit
extern "C" daSCoin_c *daSCoin_c_classInit()
{
    return new daSCoin_c();
}

// @symbol g_profile_SECRET_COIN
extern "C" SpawnInfo<daSCoin_c> g_profile_SECRET_COIN = {
    daSCoin_c_classInit, 0x0149, 0x014a, 0x00000000,
    0x00000000, 0x00320000, 0x01f40000, 0x00050000
};

// @symbol _ZN9daSCoin_c13InitResourcesEv
s32 daSCoin_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000,
                                              0x40000, 0x800002, 0);
    mStarID = param1 & 0xf;
    mGroupId = (param1 >> 8) & 0xf;
    mGroupRole = 0;
    mLeaderUniqueID = 0;
    mCollectedCount = 0;
    mDeathTimer = 0;
    Model::LoadFile(data_ov002_0210d9a8);
    return 1;
}

// @symbol _ZN9daSCoin_c8BehaviorEv
s32 daSCoin_c::Behavior()
{
    if (mDeathTimer) {
        if (DecIfAbove0_Byte(&mDeathTimer) == 0) {
            SpawnStar();
            MarkForDestruction();
        }
        return 1;
    }
    if (mGroupRole == 0) {
        u8 group = mGroupId;
        if (group == 0 || group == 0xf) {
            mGroupRole = 1;
            mLeaderUniqueID = uniqueID;
            daSCoin_c *other = 0;
            for (;;) {
                other = (daSCoin_c *)dActor_c::FindWithActorID(0x149, other);
                if (other == 0) break;
                if (other != this) {
                    other->mGroupRole = 2;
                    other->mLeaderUniqueID = uniqueID;
                }
            }
        }
    }
    if (mGroupRole == 1 && mCollectedCount == 5) {
        MarkForDestruction();
        return 1;
    }
    LinkToBlock();
    if (mdCcAc_c.otherOwner) {
        Collect();
    }
    mdCcAc_c.Clear();
    if (mClsnDisabled == 0) {
        mdCcAc_c.Update();
    }
    return 1;
}

// @symbol _ZN9daSCoin_c16CleanupResourcesEv
s32 daSCoin_c::CleanupResources()
{
    data_ov002_0210d9a8.Release();
    return 1;
}

// @symbol _ZN9daSCoin_c9SpawnStarEv
/* Finds the STARBASE whose mStarID equals this group's and spawns POWER_STAR
 * (0xb2) 0x12c000 above its position, then calls AddStarMarker on it.
 * Returns without spawning when there is no such marker. */
void daSCoin_c::SpawnStar()
{
    daStarBase_c *marker = 0;
    for (;;) {
        marker = (daStarBase_c *)dActor_c::FindWithActorID(0xb4, marker);
        if (marker == 0) return;
        if (mStarID == marker->mStarID) {
            int *base = &marker->mPosX;
            Vector3 pos;
            pos.x = base[0];
            pos.y = base[1];
            pos.z = base[2];
            pos.y += 0x12c000;
            daStar_c *star = (daStar_c *)dActor_c::Spawn(
                0xb2, mStarID | 0x40, pos, 0, mAreaId, -1);
            if (star != 0) {
                star->AddStarMarker();
            }
            return;
        }
    }
}

// @symbol _ZN9daSCoin_c11LinkToBlockEv
/* Runs once (mBlockScanDone guards it). Looks through the actor list for a
 * HATENA_BLOCK (0x14), ITEM_BLOCK (0x15) or PUSHBLOCK (0xc0) closer than
 * 0xc8000 to this coin; on a hit it disables the touch collider and stores
 * itself in the block's held/linked slot. The block collects the coin when
 * it pops. */
void daSCoin_c::LinkToBlock()
{
    if (mBlockScanDone != 0) return;
    for (dActor_c *other = dActor_c::Next(0); other != 0; other = dActor_c::Next(other)) {
        if (HasActorID(other, 0x14) || HasActorID(other, 0x15)) {
            if (Vec3_Dist(&mPos, &other->mPos) < 0xc8000) {
                mClsnDisabled = 1;
                ((daObjHatenaBlock_c *)other)->mHeldActor = this;
                break;
            }
        } else if (HasActorID(other, 0xc0)) {
            if (Vec3_Dist(&mPos, &other->mPos) < 0xc8000) {
                mClsnDisabled = 1;
                ((daObjPushblock_c *)other)->mLinkedActor = this;
                break;
            }
        }
    }
    mBlockScanDone = 1;
}

// @symbol _ZN9daSCoin_c7CollectEv
/* Collection handler. Finds the group leader by mLeaderUniqueID; unless this
 * coin is mGroupId 0xf, its mGroupId must equal the leader's mCollectedCount.
 * Plays func_02012790(0x25), bumps the leader's count, spawns the count as a
 * number, and marks the collider flags. The fifth collection starts this
 * coin's mDeathTimer at 0x1e frames; otherwise a mGroupRole 2 coin destroys
 * itself. */
void daSCoin_c::Collect()
{
    daSCoin_c *leader = (daSCoin_c *)dActor_c::FindWithID(mLeaderUniqueID);
    if (!leader) return;
    if (mGroupId != 0xf && mGroupId != leader->mCollectedCount) return;
    func_02012790(0x25);
    leader->mCollectedCount++;
    {
        Vector3 v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        SpawnNumber(v, leader->mCollectedCount, false, 0, 0);
    }
    mdCcAc_c.flags |= 1;
    if (leader->mCollectedCount == 5) { mDeathTimer = 0x1e; return; }
    if (mGroupRole != 2) return;
    MarkForDestruction();
}

/*   _ZN9daSCoin_cD1Ev  0x020f03c4  size 0x30  (complete-object destructor)   */
/*   _ZN9daSCoin_cD0Ev  0x020f03f4  size 0x44  (deleting destructor)          */

// @symbol _ZN9daSCoin_cD0Ev
// @symbol _ZN9daSCoin_cD1Ev
