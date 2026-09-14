//cpp
/* Secret Coin (SECRET_COIN 329) -- ov002/daSCoin_c.
 *
 * RTTI ov002:0x0210b000 names daSCoin_c; the debug table names SECRET_COIN.
 * overlay_actors.md lists the same slot as INVISIBLE_SECRET(329). SILVER_STAR
 * (179) is daStar_c, not this class. ov002 also carries POWER_STAR(178) and
 * STAR_MARKER(180), which 020f05f4 matches and spawns.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one `.text` section per function in the reverse of source order.
 *
 * deslop leftovers:
 * - dCcAc_c::Init 6az: InitResources passes Fix12<int> by value; the header
 *   method form size-DIFFs (notes/mwccarm-codegen.md 6az).
 * - dActor_c::Spawn as a real method size-DIFFs in 020f05f4 (s8/s16
 *   areaID/deathTableID vs the scalar ABI the ROM calls with).
 * - Named mPosX/Y/Z on the STAR_MARKER in 020f05f4 size-DIFFs; keep the
 *   int* +0x5c copy.
 * - data_ov002_0210d9a8 is the ov002 SharedFilePtr handle; symbols.txt has
 *   no recovered name, so it is not coined. SharedFilePtr.h has no fields.
 * - func_02012790 collection jingle 0x25 (020f0438).
 */

#include "daSCoin_c.h"
#include "SharedFilePtr.h"
#include "Model.h"

extern "C" {
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_02012790(int);
void _ZN9PowerStar13AddStarMarkerEv(void *thiz);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a,
        unsigned int b, void *v, void *w, int e, int f);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor,
                                              int radius, int height,
                                              unsigned int flags,
                                              unsigned int vulnFlags);
void func_ov002_020f0438(void *self);
void func_ov002_020f051c(void *self);
void func_ov002_020f05f4(void *self);

extern char data_ov002_0210d9a8;
}

/* The typed 0x1c actor profile: fBase_c reads the halfwords at +4/+6 as
 * behavior/render priorities. dActor_c reads actor flags at +8 and passes
 * the words at +0xc/+0x10/+0x14/+0x18 to SetRanges. */
struct SCoinSpawnInfo {
    daSCoin_c *(*classInit)();
    s16 behaviorPriority;   /* 0x0149 */
    s16 renderPriority;     /* 0x014a */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char SCoinSpawnInfo_size_must_be_0x1c[
    sizeof(SCoinSpawnInfo) == 0x1c ? 1 : -1];

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daSCoin_c_classInit
extern "C" daSCoin_c *daSCoin_c_classInit()
{
    return new daSCoin_c();
}

// @symbol g_profile_SECRET_COIN
extern "C" SCoinSpawnInfo g_profile_SECRET_COIN = {
    daSCoin_c_classInit, 0x0149, 0x014a, 0x00000000,
    0x00000000, 0x00320000, 0x01f40000, 0x00050000
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c13InitResourcesEv
s32 daSCoin_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000,
                                              0x40000, 0x800002, 0);
    unk_10d = param1 & 0xf;
    mGroupId = (param1 >> 8) & 0xf;
    mGroupRole = 0;
    mLeaderUniqueID = 0;
    mCollectedCount = 0;
    mDeathTimer = 0;
    Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9a8);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c8BehaviorEv
s32 daSCoin_c::Behavior()
{
    if (mDeathTimer) {
        if (DecIfAbove0_Byte(&mDeathTimer) == 0) {
            func_ov002_020f05f4(this);
            MarkForDestruction();
        }
        return 1;
    }
    if (mGroupRole == 0) {
        unsigned char st = mGroupId;
        if (st == 0 || st == 0xf) {
            {
                dActor_c *o = 0;
                mGroupRole = 1;
                mLeaderUniqueID = uniqueID;
                for (;;) {
                    o = dActor_c::FindWithActorID(0x149, o);
                    if (o == 0) break;
                    if (o != this) {
                        ((daSCoin_c *)o)->mGroupRole = 2;
                        ((daSCoin_c *)o)->mLeaderUniqueID = uniqueID;
                    }
                }
            }
        }
    }
    if (mGroupRole == 1 && mCollectedCount == 5) {
        MarkForDestruction();
        return 1;
    }
    func_ov002_020f051c(this);
    if (mdCcAc_c.otherOwner) {
        func_ov002_020f0438(this);
    }
    mdCcAc_c.Clear();
    if (mClsnDisabled == 0) {
        mdCcAc_c.Update();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c16CleanupResourcesEv
s32 daSCoin_c::CleanupResources()
{
    (*(SharedFilePtr *)&data_ov002_0210d9a8).Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov002_020f05f4(void *self)
{
    /* MATCH form: dActor_c::Spawn as a real method size-DIFFs (s8/s16
     * areaID/deathTableID vs the scalar ABI). Named mPosX/Y/Z on the
     * STAR_MARKER also size-DIFFs; keep the int* +0x5c copy. */
    daSCoin_c *c = (daSCoin_c *)self;
    char *a = 0;
    for (;;) {
        a = (char *)dActor_c::FindWithActorID(0xb4, (dActor_c *)a);
        if (a == 0) return;
        if (c->unk_10d == *(unsigned char *)(a + 0x1d9)) {
            int *base = (int *)(a + 0x5c);
            Vector3 pos;
            pos.x = base[0];
            pos.y = base[1];
            pos.z = base[2];
            pos.y += 0x12c000;
            {
                char *p = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xb2, c->unk_10d | 0x40, &pos, 0, c->mAreaId, -1);
                if (p != 0) {
                    _ZN9PowerStar13AddStarMarkerEv(p);
                }
            }
            return;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov002_020f051c(void *self)
{
    daSCoin_c *c = (daSCoin_c *)self;
    dActor_c *a;
    u32 t;
    int b;
    if (c->unk_112 != 0) return;
    a = dActor_c::Next(0);
    if (a == 0) goto done;
    do {
        t = a->actorID;
        b = (t == 0x14);
        if (b == 0) {
            b = (t == 0x15);
            if (b == 0) goto chk2;
        }
        if (Vec3_Dist((Vector3 *)&c->mPosX, (Vector3 *)&a->mPosX) < 0xc8000) {
            c->mClsnDisabled = 1;
            *(int *)((char *)a + 0x3f4) = (int)c;
            goto done;
        }
        goto next;
      chk2:
        b = (t == 0xc0);
        if (b == 0) goto next;
        if (Vec3_Dist((Vector3 *)&c->mPosX, (Vector3 *)&a->mPosX) < 0xc8000) {
            c->mClsnDisabled = 1;
            *(int *)((char *)a + 0x4f0) = (int)c;
            goto done;
        }
      next:
        a = dActor_c::Next(a);
    } while (a != 0);
  done:
    c->unk_112 = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f0438
extern "C" void func_ov002_020f0438(void *self)
{
    daSCoin_c *thiz = (daSCoin_c *)self;
    daSCoin_c *o = (daSCoin_c *)dActor_c::FindWithID(thiz->mLeaderUniqueID);
    if (!o) return;
    if (thiz->mGroupId != 0xf && thiz->mGroupId != o->mCollectedCount) return;
    func_02012790(0x25);
    o->mCollectedCount++;
    {
        Vector3 v;
        v.x = thiz->mPosX;
        v.y = thiz->mPosY;
        v.z = thiz->mPosZ;
        thiz->SpawnNumber(v, o->mCollectedCount, false, 0, 0);
    }
    thiz->mdCcAc_c.flags |= 1;
    if (o->mCollectedCount == 5) { thiz->mDeathTimer = 0x1e; return; }
    if (thiz->mGroupRole != 2) return;
    thiz->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/*   _ZN9daSCoin_cD1Ev  0x020f03c4  size 0x30  (complete-object destructor)   */
/*   _ZN9daSCoin_cD0Ev  0x020f03f4  size 0x44  (deleting destructor)          */
/* -------------------------------------------------------------------------- */

// @symbol _ZN9daSCoin_cD0Ev
// @symbol _ZN9daSCoin_cD1Ev
