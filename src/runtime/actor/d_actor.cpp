//cpp
/* dActor_c translation unit -- arm9.  Nintendo's dActor_c.
 *
 * This file owns 0x0200f658..0x02011654 (97 functions); see the worked
 * example in notes/tu-reconstruction-pilot-report.md and the boundary
 * evidence in config/tu_manifest.d/arm9/Actor.json.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST -- the constructor tops the
 * file and the leaf helpers sit at the bottom, the natural shape of a
 * hand-written .cpp.
 *
 * THE TAIL IS IN THIS FILE. One out-of-line ~dActor_c(), written after the
 * constructor, emits D2, D0, D1 under default codegen (defer_codegen left
 * on). That is the cartridge order 0x020112c8, 0x02011314, 0x02011374.
 * ~dActor_c is still the key function, so this object also emits
 * _ZTV8dActor_c, _ZTI8dActor_c and _ZTS8dActor_c; those stay the ROM's
 * copies (deadstrip-data at their symbols.txt homes), not a second table.
 * One dActor_c::dActor_c() emits C1 then C2. The ROM keeps Vector3s::~Vector3s
 * (four bytes, bx lr) between them. An unreachable Vector3s array in the
 * constructor odr-uses that destructor into the slot and adds no
 * instructions; the union and base dtors mwcc drops in the same slot are
 * compiler-only and are deadstripped, so the kept run is C1, Vector3sD1, C2.
 *
 * Declarations are reconciled IN PLACE: each member carries whatever it needs
 * that no earlier member already supplied. Nothing is hoisted but the includes.
 */

/* RECONCILIATION -- what merging 91 files into one TU forced, and nothing else.
 *
 * Every item below exists only BECAUSE these are now one translation unit: two
 * files describing the same global with different types, a local shadow struct
 * standing in for a type the merged TU really has, or a declaration whose
 * parameter list disagrees with the definition that used to live in another
 * file. Not one of them changes what a function does, and every one was held to
 * the byte gate: 91/91 members reproduce the cartridge.
 *
 *  - Shadow types dropped for the real header's: dActor_c, Vector3, Vector3_16,
 *    Matrix4x3, dCc_c, Player, Sound, Heap, and `typedef int Fix12`
 *    (types.h has a Fix12<T> TEMPLATE, so that typedef is not even legal here).
 *    Field names follow: deathTableId -> mDeathTableID, flags -> mFlags, camSpacePos
 *    -> mCamSpacePosX, pos -> mPosX, speed -> mSpeedX, areaID -> mAreaId.
 *  - One spelling per global, chosen for what the ROM's use of it supports:
 *    data_0209b468, the global actor list, is defined below; the list
 *    helpers take its address, and Next() reads its first link.
 *  - One prototype per function, taken from whichever file had recovered the
 *    most: func_02010e78's caller knew (const Vector3*, const Vector3_16*, s8,
 *    s16) where its own file only knew (int, int, uchar, short);
 *    Particle::System::NewSimple returns int, because the two callers that USE
 *    the result cannot both be wrong about it while six that discard it are
 *    right. dActor_c::ClosestPlayer is DEFINED here, so the three disagreeing
 *    extern "C" guesses at its prototype are simply gone.
 *  - Ordinals 12, 14, 16 and 18 use the real dBgCh_Gnd header and its
 *    collision-height field; their automatic query objects own the normal
 *    C++ lifetime (12 and 18 used to spell the C1/D1 calls by hand on a
 *    stand-in struct). Ordinal 0 takes the real dBgCh_Lin instead: its
 *    ctor/dtor are out-of-line declarations, so the member spelling emits
 *    the same calls the retired shard proved.
 *
 * include/dActor_c.h gained three declarations for this file: a `struct Matrix4x3;`
 * forward declaration, `void UntrackInDeathTable();` and
 * `Matrix4x3 *UpdateCarry(Player &, const Vector3 &);`. All three are
 * NON-VIRTUAL, so the class layout, its 0xd0 size assertion and its 31 vtable
 * slots are untouched, and ~dActor_c() is still the key function. */

/* Includes: union of the admitted legacy files', first-seen in emission
 * order. Header-guarded, so the union is safe; ordering constraints are
 * not otherwise checked. daFPknBall_c.h, daObjNumber_c.h and daCoin_c.h
 * came later, for the classes SpawnFireball, SpawnNumber and
 * GivePlayerCoins spawn. */
#include "types.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "dActor_c.h"
#include "Player.h"
#include "common.h"
#include "dCc_c.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"
#include "daFPknBall_c.h"
#include "daObjNumber_c.h"
#include "daCoin_c.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "dClipper.h"
#include "nitro/fx/fx.h"
#include "daObjNumber_c.h"
#include "daCoin_c.h"
#include "daFPknBall_c.h"
namespace cstd {
s16 atan2(s32, s32);
}

/* Spawn globals the constructor reads. The s16* spellings are the ones
   func_02010e78 already uses; a second void* declaration in this TU is
   rejected. */
extern "C" {
extern s16 *data_0209b460;
extern s16 *data_0209b45c;
extern s16 data_0208e378;
/* local extern: no header declares it. List link, same call the shard proved. */
int func_0203b244(void *list, void *node);
/* local extern: no header declares it. Actor info table, indexed by actorID. */
extern void *data_020a4bb8;
/* local extern: no header declares it. Game mode; 2 pushes the clip distance. */
extern unsigned char data_0209f2d8;
/* Defined below as extern "C". Fix12-by-value, so it is not a method. */
}

#ifdef _MSC_VER
/* The host has one ~dActor_c(). These are the ROM's flat D2 and D0 names. */
extern "C" dActor_c *_ZN8dActor_cD2Ev(dActor_c *thiz) {
    thiz->dActor_c::~dActor_c();
    return thiz;
}
extern "C" dActor_c *_ZN8dActor_cD0Ev(dActor_c *thiz) {
    thiz->dActor_c::~dActor_c();
    dActor_c::operator delete(thiz);
    return thiz;
}
#endif

/* The global actor list every dActor_c links its mActorListNode into: the
   first and last nodes. Its inline constructor empties it, so mwcc emits
   __sinit_d_actor.cpp to run that (was the handwritten __sinit_02074d90).
   The list helpers (func_0203b244 links, func_0203b27c unlinks, the
   ActorBase.cpp searches) take its address. */
struct ActorList {
    fLiNdBa_c *first;
    fLiNdBa_c *last;

    ActorList() : first(0), last(0) {}
};
ActorList data_0209b468;

/* Constructor first in source so reverse emission puts C1/C2 above the
   destructor. Both variants are this one body. */
// @symbol _ZN8dActor_cC1Ev
// @symbol _ZN8dActor_cC2Ev
dActor_c::dActor_c() : mActorListNode(this) {
    func_0203b244(&data_0209b468, &mActorListNode);
    {
        int *spawnPos = (int *)data_0209b460;
        if (spawnPos) {
            mPosX = spawnPos[0];
            mPosY = spawnPos[1];
            mPosZ = spawnPos[2];
        }
    }
    {
        short *spawnRot = (short *)data_0209b45c;
        if (spawnRot) {
            mAngleX = spawnRot[0];
            mAngleY = spawnRot[1];
            mAngleZ = spawnRot[2];
            /* The ROM reloads the global. Folding the two reads drops an ldr. */
            spawnRot = (short *)data_0209b45c;
            mPrevAngleX = spawnRot[0];
            mPrevAngleY = spawnRot[1];
            mPrevAngleZ = spawnRot[2];
        }
    }
    mAreaId = data_0209b44c;
    mDeathTableID = data_0208e378;
    {
        void **table = *(void ***)&data_020a4bb8;
        int actorID = this->actorID;
        SpawnInfo<void> *info = (SpawnInfo<void> *)table[actorID];
        mFlags = info->actorFlags;
        {
            int isMode2 = (data_0209f2d8 == 2);
            int clipDistance;
            int farDistance = info->farDistance;
            if (isMode2)
                clipDistance = info->clipDistance + 0x7d0000;
            else
                clipDistance = info->clipDistance;
            SetRanges(info->clipOffsetY, info->clipRadius, clipDistance, farDistance);
        }
    }
    /* Unreachable. Vector3s's empty destructor inlines at a direct call, but
       an array cleanup has to pass its address to __cxa_vec_cleanup, so mwcc
       emits the out-of-line Vector3s::~Vector3s() (D1) in the constructor's helper slot, between C1 and
       C2. -O4 deletes the cleanup itself: both variants stay 0x148. The marker
       names that emitted symbol; its body is the inline destructor in
       include/types.h. */
    // @symbol _ZN8Vector3sD1Ev
    if (0) {
        Vector3s tmp[2];
        (void)tmp;
    }
}

// @symbol _ZN8dActor_cD2Ev
// @symbol _ZN8dActor_cD0Ev
// @symbol _ZN8dActor_cD1Ev
dActor_c::~dActor_c() {
    func_0203b27c((int)&data_0209b468, (int)&mActorListNode);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19BeforeInitResourcesEv
/* ROM ordinal 90 -- dActor_c::BeforeInitResources()
 * 0x02011268  size 0x60 */
bool dActor_c::BeforeInitResources()
{
    int spawnEvenIfKilledBefore = (mFlags & 0x8000000) ? 1 : 0;

    if (spawnEvenIfKilledBefore == 0 && GetBitInDeathTable() != 0) {
        MarkForDestruction();
        return true;
    }
    if (!fBase_c::BeforeInitResources())
        return false;
    return true;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18AfterInitResourcesEj
/* ROM ordinal 89 -- dActor_c::AfterInitResources(unsigned int)
 * 0x02011244  size 0x24 */
void dActor_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
    /* The store through `*(u32 *)&mFlags` is a codegen launder, not a type
       conversion: the ROM materialises the address into a register first
       (`add r1,r4,#0xb0`) and then does the read-modify-write through it.
       Writing `mFlags |= 0x38` folds the offset into the ldr/str and comes
       out 4 bytes short. */
    *(u32 *)&mFlags |= 0x38;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c22BeforeCleanupResourcesEv
/* ROM ordinal 88 -- dActor_c::BeforeCleanupResources()
 * 0x02011220  size 0x24 */
int dActor_c::BeforeCleanupResources()
{
    if (fBase_c::BeforeCleanupResources() != 0)
        return 1;
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c21AfterCleanupResourcesEj
/* ROM ordinal 87 -- dActor_c::AfterCleanupResources(unsigned int)
 * 0x02011214  size 0xc */
void dActor_c::AfterCleanupResources(u32 vfSuccess)
{
    fBase_c::AfterCleanupResources(vfSuccess);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c14BeforeBehaviorEv
/* ROM ordinal 86 -- dActor_c::BeforeBehavior()
 * 0x02010fd4  size 0x240 */
extern "C" {
u8 IsAreaShowing(int areaId);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern int data_0209fc68; /* nonzero forces the actor to think anyway */
extern Matrix4x3 data_0209b3ec; /* world -> camera */
extern dClipper data_0209f43c;  /* the dClipper instance */
extern u8 data_0209f274; /* doubles the far-away threshold when set */
extern u8 data_0209f2c4;
extern u8 data_0209f20c;
extern u8 data_0209f294;
extern u32 data_0209b464; /* flag mask that forces thinking; 0 means "any" */
}
int dActor_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    data_0209b458 = 0;

    signed char areaId = mAreaId;
    if (areaId >= 0 && !IsAreaShowing(areaId)) {
        mFlags |= 0x38;
        mCamSpacePosX = 0x7fffffff;
        mCamSpacePosY = 0x7fffffff;
        mCamSpacePosZ = 0x7fffffff;
        if (data_0209fc68 == 0 || (mFlags & 0x10000))
            return 0;
    } else {
        if (mClipRadius != 0) {
            Vector3 tmp;
            tmp.x = mPosX >> 3;
            tmp.y = (mPosY + mClipOffsetY) >> 3;
            tmp.z = mPosZ >> 3;
            MulVec3Mat4x3(&tmp, &data_0209b3ec, &mCamSpacePos);
        }
        if (mFlags & 0x10003) {
            int r = data_0209f43c.ClipSphere(mCamSpacePos, mClipRadius, &mClipResult);
            int thresh = mClipDistance;
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                mFlags |= 0x18;
                if (mFlags & 0x10000)
                    return 0;
            } else {
                u32 *p = &mFlags;
                *p &= ~0x38;
                if (r > mFarDistance)
                    *p |= 0x10;
            }
        } else {
            mFlags &= ~0x38;
        }
    }

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) != 0 ||
        (((mFlags & 9) == 9 || (data_0209b464 != 0 && (mFlags & data_0209b464) == 0)) &&
         data_0209fc68 == 0))
        return 0;

    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13AfterBehaviorEj
/* ROM ordinal 85 -- dActor_c::AfterBehavior(unsigned int)
 * 0x02010fc8  size 0xc */
void dActor_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c12BeforeRenderEv
/* ROM ordinal 84 -- dActor_c::BeforeRender()
 * 0x02010f78  size 0x50 */
int dActor_c::BeforeRender()
{
    u32 flags;

    if (!fBase_c::BeforeRender())
        return 0;

    flags = mFlags;
    if ((flags & 0x20) || ((flags & 8) && (flags & 2)))
        return 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11AfterRenderEj
/* ROM ordinal 83 -- dActor_c::AfterRender(unsigned int)
 * 0x02010f6c  size 0xc */
void dActor_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c10FindWithIDEj
/* ROM ordinal 82 -- dActor_c::FindWithID(unsigned int)
 * 0x02010f3c  size 0x30 */
extern "C" {
/* local extern: defined in ActorBase.cpp, which has no header. */
extern fLiNdBa_c *func_02043f98(fLiNdBa_c **head, unsigned int key);
}
dActor_c *dActor_c::FindWithID(u32 id)
{
    fLiNdBa_c *node = func_02043f98((fLiNdBa_c **)&data_0209b468, id);
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15FindWithActorIDEjPS_
/* ROM ordinal 81 -- dActor_c::FindWithActorID(unsigned int, dActor_c *)
 * 0x02010ef0  size 0x4c */
/* func_02043f4c really takes and returns fLiNdBa_c * (ActorBase.cpp), but
   decl_common.h spells it with ints and this TU includes that. */
dActor_c *dActor_c::FindWithActorID(u32 actorID, dActor_c *after) {
    fLiNdBa_c *node;
    if (after)
        node =
            (fLiNdBa_c *)func_02043f4c((int *)&data_0209b468, actorID, (int)&after->mActorListNode);
    else
        node = (fLiNdBa_c *)func_02043f4c((int *)&data_0209b468, actorID, 0);
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c4NextEPKS_
/* ROM ordinal 80 -- dActor_c::Next(dActor_c const *)
 * 0x02010ecc  size 0x24 */
dActor_c *dActor_c::Next(const dActor_c *after)
{
    fLiNdBa_c *node;
    if (after)
        node = after->mActorListNode.next;
    else
        node = *(fLiNdBa_c **)&data_0209b468;
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol func_02010e78
/* ROM ordinal 79 -- func_02010e78
 * 0x02010e78  size 0x54 */
extern "C" {
extern s16 *data_0209b460;          /* spawn position, or null to leave it at 0 */
extern s16 *data_0209b45c;          /* spawn rotation, or null */
extern s16 data_0208e378;           /* spawn death-table id */
}
/* Stages Spawn's arguments for the constructor to pick up. */
extern "C" void func_02010e78(const Vector3 *pos, const Vector3_16 *rot, s8 areaID,
                              s16 deathTableID) {
    data_0209b460 = (s16 *)pos;
    data_0209b45c = (s16 *)rot;
    data_0209b44c = areaID;
    data_0208e378 = deathTableID;
    data_0209b458 = 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as
/* ROM ordinal 78 -- dActor_c::Spawn(unsigned int, unsigned int, Vector3 const &, Vector3_16 const *, signed char, short)
 * 0x02010e2c  size 0x4c */
extern "C" {
extern fBase_c *data_0209f5c0;
void func_02010e78(const Vector3 *pos, const Vector3_16 *rot, s8 areaID, s16 deathTableID);
}
dActor_c *dActor_c::Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                    const Vector3_16 *rot, s8 areaID, s16 deathTableID)
{
    func_02010e78(&pos, rot, areaID, deathTableID);
    return (dActor_c *)dBase_c::Spawn(actorID, data_0209f5c0, spawnParam, 2);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c9SetRangesEiiii
/* ROM ordinal 77 -- dActor_c::SetRanges(int, int, int, int)
 * 0x02010e08  size 0x24 */
/* The three ranges are stored at 1/8 scale, the scale BeforeBehavior projects
   into. */
void dActor_c::SetRanges(Fix12i clipOffsetY, Fix12i clipRadius, Fix12i clipDistance,
                         Fix12i farDistance) {
    mClipOffsetY = clipOffsetY;
    mClipRadius = clipRadius >> 3;
    mClipDistance = clipDistance >> 3;
    mFarDistance = farDistance >> 3;
}

/* -------------------------------------------------------------------------- */
// @symbol func_02010da4
/* ROM ordinal 76 -- func_02010da4
 * 0x02010da4  size 0x64 */
/* Clamps each axis of mPos to +-32000.0, the edge of the world. */
extern "C" void func_02010da4(dActor_c *actor) {
    s32 v;
    v = actor->mPosX;
    if (v < -0x7d00000)
        actor->mPosX = -0x7d00000;
    else if (v > 0x7d00000)
        actor->mPosX = 0x7d00000;
    v = actor->mPosY;
    if (v < -0x7d00000)
        actor->mPosY = -0x7d00000;
    else if (v > 0x7d00000)
        actor->mPosY = 0x7d00000;
    v = actor->mPosZ;
    if (v < -0x7d00000)
        actor->mPosZ = -0x7d00000;
    else if (v > 0x7d00000)
        actor->mPosZ = 0x7d00000;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c
/* ROM ordinal 75 -- dActor_c::UpdatePosWithOnlySpeed(dCc_c *)
 * 0x02010d40  size 0x64 */
extern "C" {
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void func_02010da4(dActor_c *actor);
}
/* Adds the velocity (mSpeedX, mVertSpeed, mSpeedZ) to mPos, then the
   cylinder's horizontal push-out, then clamps to the world bounds. */
void dActor_c::UpdatePosWithOnlySpeed(dCc_c *clsn) {
    AddVec3(&mPos, (Vector3 *)&mSpeedX, &mPos);
    if (clsn != 0) {
        mPosX += clsn->pushback.x;
        mPosZ += clsn->pushback.z;
    }
    func_02010da4(this);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv
/* ROM ordinal 74 -- dActor_c::UpdatePosWithHorzSpeedAndAng()
 * 0x02010c5c  size 0xe4 */
extern "C" {
extern s16 data_02082214[]; /* NitroSDK FX_SinCosTable_: {sin, cos} per angle >> 4 */
}
/* Splits mHorzSpeed along mPrevAngleY (the heading the velocity follows) into
   the x/z velocity words, x by sin and z by cos, and applies mVertAccel to
   mVertSpeed, clamped at mTerminalVelocity. */
void dActor_c::UpdatePosWithHorzSpeedAndAng() {
    s32 horzSpeed = mHorzSpeed;
    if (horzSpeed == 0) {
        s32 vertSpeed = mVertSpeed + mVertAccel;
        s32 terminal = mTerminalVelocity;
        if (vertSpeed >= terminal)
            terminal = vertSpeed;
        mSpeedX = 0;
        mVertSpeed = terminal;
        mSpeedZ = 0;
    } else {
        s32 vertSpeed;
        s32 accel;
        s32 terminal;
        s32 speedZ;
        s32 idx;
        s16 cosv;
        s32 next;
        s16 sinv;
        s32 speedX;
        idx = ((u16)mPrevAngleY >> 4) * 2;
        cosv = data_02082214[idx + 1];
        vertSpeed = mVertSpeed;
        accel = mVertAccel;
        terminal = mTerminalVelocity;
        speedZ = FX_MUL(horzSpeed, cosv);
        next = vertSpeed + accel;
        if (next >= terminal)
            terminal = next;
        sinv = data_02082214[idx];
        speedX = FX_MUL(horzSpeed, sinv);
        mSpeedX = speedX;
        mVertSpeed = terminal;
        mSpeedZ = speedZ;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c9UpdatePosEP5dCc_c
/* ROM ordinal 73 -- dActor_c::UpdatePos(dCc_c *)
 * 0x02010c30  size 0x2c */
void dActor_c::UpdatePos(dCc_c *clsn)
{
    UpdatePosWithHorzSpeedAndAng();
    UpdatePosWithOnlySpeed(clsn);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x3iih
/* ROM ordinal 72 -- dActor_c::DropShadowRadHeight(dExtShadowModel_c &, Matrix4x3 &, int, int, unsigned char)
 * 0x02010be8  size 0x48 */
void dActor_c::DropShadowRadHeight(dExtShadowModel_c &shadow, Matrix4x3 &matrix, Fix12i radius,
                                   Fix12i depth, u8 opacity) {
    if (mFlags & 0x10) /* beyond mFarDistance: no drop shadow */
        return;
    shadow.InitModel(&matrix, radius, depth, radius, opacity);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x3iiih
/* ROM ordinal 71 -- dActor_c::DropShadowScaleXYZ(dExtShadowModel_c &, Matrix4x3 &, int, int, int, unsigned char)
 * 0x02010b9c  size 0x4c */
void dActor_c::DropShadowScaleXYZ(dExtShadowModel_c &shadow, Matrix4x3 &matrix, Fix12i scaleX,
                                  Fix12i scaleY, Fix12i scaleZ, u8 opacity) {
    if (mFlags & 0x10)
        return;
    shadow.InitModel(&matrix, scaleX, scaleY, scaleZ, opacity);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13ClosestPlayerEv
/* ROM ordinal 70 -- dActor_c::ClosestPlayer()
 * 0x02010ad8  size 0xc4 */
extern "C" {
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern int data_0208e380;           /* distance to the cached closest player */
extern int data_0208e37c;           /* distance to the cached farthest player */
extern unsigned char data_0209f21c; /* number of players */
extern void *data_0209f394[];       /* the player table */
}
Player *dActor_c::ClosestPlayer()
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            Player *player = (Player *)data_0209f394[i];
            if (!player)
                continue;
            int dist = Vec3_Dist(&mPos, &player->mPos);
            if (dist < data_0208e380) {
                data_0208e380 = dist;
                data_0209b458 = player;
            }
            if (dist > data_0208e37c) {
                data_0208e37c = dist;
                data_0209b450 = player;
            }
        }
    }
    return (Player *)data_0209b458;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c22ClosestNonVanishPlayerEv
/* ROM ordinal 69 -- dActor_c::ClosestNonVanishPlayer()
 * 0x02010a08  size 0xd0 */
Player *dActor_c::ClosestNonVanishPlayer()
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            Player *player = (Player *)data_0209f394[i];
            if (!player)
                continue;
            if (player->mIsVanish)
                continue;
            int dist = Vec3_Dist(&mPos, &player->mPos);
            if (dist < data_0208e380) {
                data_0208e380 = dist;
                data_0209b458 = player;
            }
            if (dist > data_0208e37c) {
                data_0208e37c = dist;
                data_0209b450 = player;
            }
        }
    }
    return (Player *)data_0209b458;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13DistToCPlayerEv
/* ROM ordinal 68 -- dActor_c::DistToCPlayer()
 * 0x020109e4  size 0x24 */
s32 dActor_c::DistToCPlayer()
{
    ClosestPlayer();
    return data_0208e380;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18HorzAngleToCPlayerEv
/* ROM ordinal 67 -- dActor_c::HorzAngleToCPlayer()
 * 0x020109b8  size 0x2c */
extern "C" {
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}
s16 dActor_c::HorzAngleToCPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle(&mPos, &((dActor_c *)data_0209b458)->mPos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c23HorzAngleToCPlayerOrAngEv
/* ROM ordinal 66 -- dActor_c::HorzAngleToCPlayerOrAng()
 * 0x0201097c  size 0x3c */
s16 dActor_c::HorzAngleToCPlayerOrAng()
{
    dActor_c *player;

    ClosestPlayer();
    player = (dActor_c *)data_0209b458;
    if (player == 0)
        return mAngleY;

    return Vec3_HorzAngle(&mPos, &player->mPos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c14FarthestPlayerEv
/* ROM ordinal 65 -- dActor_c::FarthestPlayer()
 * 0x02010958  size 0x24 */
Player *dActor_c::FarthestPlayer()
{
    ClosestPlayer();
    return (Player *)data_0209b450;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18HorzAngleToFPlayerEv
/* ROM ordinal 64 -- dActor_c::HorzAngleToFPlayer()
 * 0x0201092c  size 0x2c */
s16 dActor_c::HorzAngleToFPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle(&mPos, &((dActor_c *)data_0209b450)->mPos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18ClosestWithActorIDEj
/* ROM ordinal 63 -- dActor_c::ClosestWithActorID(unsigned int)
 * 0x020108c4  size 0x68 */
dActor_c *dActor_c::ClosestWithActorID(u32 actorID)
{
    dActor_c *actor = 0;
    dActor_c *closest = 0;
    Fix12i closestDist = 0x7fffffff;

    for (;;) {
        actor = FindWithActorID(actorID, actor);
        if (!actor)
            break;

        if (actor != this) {
            Fix12i dist = Vec3_Dist(&mPos, &actor->mPos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
            }
        }
    }
    return closest;
}

/* -------------------------------------------------------------------------- */
// @symbol func_02010844
/* ROM ordinal 62 -- func_02010844
 * 0x02010844  size 0x80 */
extern "C" {
extern s16 atan2_hold_d_actor_1(Fix12i y, Fix12i x);
extern s32 Vec3_HorzLen(Vector3 *v);
}
/* atan2 of v's horizontal length, projected onto heading `angle`, against
   v->y. The leading actor argument is unused. */
extern "C" s32 func_02010844(void *actor, Vector3 *v, s16 angle) {
    s32 dir;
    s32 horzLen;

    dir = cstd::atan2(v->x, v->z);
    angle = (s16)(dir - angle);
    horzLen = Vec3_HorzLen(v);
    return cstd::atan2(FX_MUL(horzLen, data_02082214[((u16)angle >> 4) * 2 + 1]), v->y);
}

/* -------------------------------------------------------------------------- */
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650[];
}
// @symbol _ZN8dActor_c10SpawnCoinsERK7Vector3iis
/* ROM ordinal 61 -- dActor_c::SpawnCoins(Vector3 const &, int, int, short)
 * 0x02010714  size 0x130 */
/* count is SIGNED, although every local extern of this symbol and the old
   mangled name claimed unsigned: 0x02010734 branches with `ble` and the loop
   closes with `blt`, which an unsigned count assembles as `bls`/`blo`. */
void dActor_c::SpawnCoins(const Vector3 &pos, s32 count, Fix12i spread, s16 angle) {
    dActor_c *actor;
    s32 ang;
    s32 prev;
    s32 i;
    u32 r;
    s16 ang0;
    Vector3_16 *rot;
    s32 deathId;
    u32 id;
    u32 param;

    ang0 = angle;
    rot = 0;
    id = 0x120;
    param = 2;
    deathId = -1;
    prev = 0xff;

    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    }

    for (i = 0; i < count; i++) {
        actor = dActor_c::Spawn(id, param, pos, rot, mAreaId, deathId);

        if (actor != 0) {
            do {
                ang = (s32)(((u32)RandomIntInternal(data_0209e650) >> 16) << 27) >> 16;
            } while (ang == prev);

            r = (u32)RandomIntInternal(data_0209e650);
            r >>= 16;

            actor->mPrevAngleX = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            actor->mPrevAngleY = (s16)(ang0 + ang);
            actor->mPrevAngleZ = 0;
            actor->mHorzSpeed = spread;
        }
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15GivePlayerCoinsER6Playerhj
/* ROM ordinal 60 -- dActor_c::GivePlayerCoins(Player &, unsigned char, unsigned int)
 * 0x0201061c  size 0xf8 */
extern "C" {
extern u32 data_02075238[]; /* coins each kind is worth */
extern u16 data_02075230[]; /* actor ID of the coin each kind spawns */
void GiveCoins(s32 playerNo, s32 amount); /* 0x0202a7b8 */
}
void dActor_c::GivePlayerCoins(Player &player, u8 count, u32 coinKind)
{
    if (&player == 0)
        return;
    if (count == 0)
        return;
    if (coinKind >= 3)
        return;

    if (player.mIsUnderwater)
        Sound::PlayBank3(0x12, mCamSpacePos);
    else
        Sound::PlayBank3(0x11, mCamSpacePos);

    s32 amount = count * data_02075238[coinKind];
    GiveCoins(player.mPlayerNo, amount);
    player.Heal(amount << 8);

    /* Through a base pointer, not three field reads: the ROM materialises
       `add r2, r5, #0x5c` once and loads [r2], [r2,#4], [r2,#8] off it.
       Reading player.mPosX/Y/Z directly costs one instruction less (0xf4, not
       0xf8) because the offsets fold into r5. */
    const Vector3 *src = &player.mPos;
    Vector3 spawnPos;
    spawnPos.x = src->x;
    spawnPos.y = src->y;
    spawnPos.z = src->z;
    spawnPos.y = spawnPos.y + 0x96000;

    /* A coin of the same kind (actor 0x120, 0x121 or 0x122, all daCoin_c),
       spawned above the player; it then tracks mFollowTarget every frame. */
    daCoin_c *coin =
        static_cast<daCoin_c *>(Spawn(data_02075230[coinKind], 8, spawnPos, 0, player.mAreaId, -1));
    if (coin != 0)
        coin->mFollowTarget = &player;
}

/* -------------------------------------------------------------------------- */
// @symbol func_020105cc
/* ROM ordinal 59 -- func_020105cc
 * 0x020105cc  size 0x50 */
extern "C" void func_020105cc(dActor_c *actor, u32 flags) {
    if (flags & 0x380) {
        Sound::PlayBank0(0xa, actor->mCamSpacePos);
        return;
    }
    if (flags & 0x40) {
        Sound::PlayBank0(9, actor->mCamSpacePos);
        return;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player
/* ROM ordinal 58 -- dActor_c::JumpedOnByPlayer(dCc_c &, Player &)
 * 0x0201054c  size 0x80 */
int dActor_c::JumpedOnByPlayer(dCc_c &clsn, Player &player)
{
    volatile s32 pad[4];
    s32 vel = player.mVertSpeed;

    (void)&pad;
    if (clsn.flags & 6) {
        if (vel < 0 && player.mIsAirborne != 0
            && mPosY < player.mPosY - 0xa000)
            return 1;
        return 0;
    } else {
        s32 pushbackY = clsn.pushback.y;
        if (vel < 0 && pushbackY < 0 && pushbackY > vel)
            return 1;
        return 0;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c24BumpedUnderneathByPlayerER6Player
/* ROM ordinal 57 -- dActor_c::BumpedUnderneathByPlayer(Player &)
 * 0x02010518  size 0x34 */
int dActor_c::BumpedUnderneathByPlayer(Player &player)
{
    if (player.mIsAirborne != 0 && player.mVertSpeed > 0
        && player.mPosY < mPosY)
        return 1;

    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15IsPlayerInRangeEi
/* ROM ordinal 56 -- dActor_c::IsPlayerInRange(int)
 * 0x020104dc  size 0x3c */
bool dActor_c::IsPlayerInRange(s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist(&mPos, &closest->mPos) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15IsPlayerInRangeEiiii
/* ROM ordinal 55 -- dActor_c::IsPlayerInRange(int, int, int, int)
 * 0x02010498  size 0x44 */
bool dActor_c::IsPlayerInRange(Fix12i posX, Fix12i posY, Fix12i posZ, s32 maxDist) {
    Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    Player *closest = ClosestPlayer();
    return Vec3_Dist(&pos, &closest->mPos) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15IsPlayerInRangeERK7Vector3i
/* ROM ordinal 54 -- dActor_c::IsPlayerInRange(Vector3 const &, int)
 * 0x0201045c  size 0x3c */
bool dActor_c::IsPlayerInRange(const Vector3 &pos, s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist(&pos, &closest->mPos) < (maxDist << 12);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c12ReflectAngleEiis
/* ROM ordinal 53 -- dActor_c::ReflectAngle(int, int, short)
 * 0x02010428  size 0x34 */
/* Mirrors angToReflect, turned around, about the normal's heading. */
s16 dActor_c::ReflectAngle(Fix12i normalX, Fix12i normalZ, s16 angToReflect) {
    s16 normalAngle = cstd::atan2(normalX, normalZ);
    s16 delta = (s16)((angToReflect + 0x8000) - normalAngle);
    return (s16)(normalAngle - delta);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c14GetSubtractionEss
/* ROM ordinal 52 -- dActor_c::GetSubtraction(short, short)
 * 0x020103f4  size 0x34 */
/* The magnitude of the shortest turn from angle a to angle b. */
int dActor_c::GetSubtraction(short a, short b)
{
    int d = (short)(b - a);
    if (d == -0x8000)
        d = -0x7fff;
    if (d < 0)
        d = (short)(-d);
    return d;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c7FindEggER5dCc_c
/* ROM ordinal 51 -- dActor_c::FindEgg(dCc_c &)
 * 0x020103b4  size 0x40 */
dActor_c *dActor_c::FindEgg(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x2000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18FindExplosionActorER5dCc_c
/* ROM ordinal 50 -- dActor_c::FindExplosionActor(dCc_c &)
 * 0x02010374  size 0x40 */
dActor_c *dActor_c::FindExplosionActor(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x4000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol func_02010304
/* ROM ordinal 49 -- func_02010304
 * 0x02010304  size 0x70 */
/* Returns the player behind `clsn`'s last hit if that hit carried hitFlags
   0x1000 and the player then grabs `target`; otherwise null. 0xbf is the
   player's actor ID. */
extern "C" void *func_02010304(dActor_c *target, dCc_c *clsn) {
    u32 id = clsn->otherOwner;
    dActor_c *found;
    if (id != 0 && (found = dActor_c::FindWithID(id)) != 0 && (clsn->hitFlags & 0x1000)) {
        int isPlayer = found->actorID == 0xbf;
        if (isPlayer && ((Player *)found)->TryGrab(*target))
            return found;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_16iij
/* ROM ordinal 48 -- dActor_c::SpawnFireball(Vector3 const &, Vector3_16 const *, int, int, unsigned int)
 * 0x020102b0  size 0x54 */
/* Spawns actor 0xfe, daFPknBall_c (the fire piranha plant's fireball), with
   its starting speed and the speed it then approaches. RETURNS THE BALL:
   r0 holds it from the Spawn call to the `bx lr`, and daTrs_c reads it back
   after the call, so the void the old local externs mostly spelled was wrong. */
dActor_c *dActor_c::SpawnFireball(const Vector3 &pos, const Vector3_16 *rot, Fix12i horzSpeed,
                                  Fix12i targetSpeed, u32 param1) {
    dActor_c *ball = dActor_c::Spawn(0xfe, param1, pos, rot, mAreaId, -1);
    if (ball != 0) {
        ball->mHorzSpeed = horzSpeed;
        static_cast<daFPknBall_c *>(ball)->mTargetSpeed = targetSpeed;
    }
    return ball;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3
/* ROM ordinal 47 -- dActor_c::UpdateCarry(Player &, Vector3 const &)
 * 0x02010180  size 0x130 */
extern "C" {
void *func_ov002_020e496c(Player *p); /* the player's current mBodyModels entry */
void MulMat4x3Mat4x3(Matrix4x3 *m1, Matrix4x3 *m0, Matrix4x3 *mF);
void Matrix4x3_FromTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *mF, short x, short y, short z);
void Vec3_Asr(Vector3 *vF, Vector3 *v, int amount);
void Vec3_Lsl(Vector3 *vF, const Vector3 *v, int amount);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
extern Matrix4x3 data_020a0e68;
}
Matrix4x3 *dActor_c::UpdateCarry(Player &player, const Vector3 &vec)
{
    Vector3 t;
    t.x = 0;
    t.y = 0;
    t.z = 0;
    /* Bone 14 of the body model, carried into world space by the model's
       own matrix; only its translation survives. */
    dExtSimpleModel_c *body = (dExtSimpleModel_c *)func_ov002_020e496c(&player);
    Matrix4x3 *bones = body->data.transforms;
    data_020a0e68 = body->mat4x3;
    MulMat4x3Mat4x3(&bones[14], &data_020a0e68, &data_020a0e68);
    t = *(Vector3 *)&data_020a0e68.m[9];
    mPrevAngleY = player.mAngleY;
    mAngleY = mPrevAngleY;
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    Vector3 asr;
    /* vec is a const reference; Vec3_Asr's definition is not const-qualified. */
    Vec3_Asr(&asr, (Vector3 *)&vec, 3);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    mPosX = data_020a0e68.m[9];
    mPosY = data_020a0e68.m[10];
    mPosZ = data_020a0e68.m[11];
    Vector3 lsl;
    Vec3_Lsl(&lsl, &mPos, 3);
    mPosX = lsl.x;
    mPosY = lsl.y;
    mPosZ = lsl.z;
    return &data_020a0e68;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c10EarthquakeERK7Vector3i
/* ROM ordinal 46 -- dActor_c::Earthquake(Vector3 const &, int)
 * 0x02010168  size 0x18 */
extern "C" {
extern void func_0200d8c8(void *, const Vector3 *, int);
extern void *data_0209f318; /* the camera */
}
/* r1 is the position and r2 the intensity; both are forwarded untouched. */
void dActor_c::Earthquake(const Vector3 &pos, Fix12i magnitude) {
    func_0200d8c8(data_0209f318, &pos, magnitude);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13OnYoshiTryEatEv
/* ROM ordinal 45 -- dActor_c::OnYoshiTryEat()
 * 0x02010160  size 0x8 */
int dActor_c::OnYoshiTryEat()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13OnTurnIntoEggER6Player
/* ROM ordinal 44 -- dActor_c::OnTurnIntoEgg(Player &)
 * 0x02010154  size 0xc */
void dActor_c::OnTurnIntoEgg(Player &player)
{
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c9Virtual50Ev
/* ROM ordinal 43 -- dActor_c::Virtual50()
 * 0x0201014c  size 0x8 */
int dActor_c::Virtual50()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15OnGroundPoundedERS_
/* ROM ordinal 42 -- dActor_c::OnGroundPounded(dActor_c &)
 * 0x02010148  size 0x4 */
void dActor_c::OnGroundPounded(dActor_c &pounder)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11OnAttacked1ERS_
/* ROM ordinal 41 -- dActor_c::OnAttacked1(dActor_c &)
 * 0x02010144  size 0x4 */
int dActor_c::OnAttacked1(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11OnAttacked2ERS_
/* ROM ordinal 40 -- dActor_c::OnAttacked2(dActor_c &)
 * 0x02010140  size 0x4 */
int dActor_c::OnAttacked2(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c8OnKickedERS_
/* ROM ordinal 39 -- dActor_c::OnKicked(dActor_c &)
 * 0x0201013c  size 0x4 */
void dActor_c::OnKicked(dActor_c &kicker)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c8OnPushedERS_
/* ROM ordinal 38 -- dActor_c::OnPushed(dActor_c &)
 * 0x02010138  size 0x4 */
int dActor_c::OnPushed(dActor_c &pusher)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c24OnHitByCannonBlastedCharERS_
/* ROM ordinal 37 -- dActor_c::OnHitByCannonBlastedChar(dActor_c &)
 * 0x02010134  size 0x4 */
int dActor_c::OnHitByCannonBlastedChar(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15OnHitByMegaCharER6Player
/* ROM ordinal 36 -- dActor_c::OnHitByMegaChar(Player &)
 * 0x02010130  size 0x4 */
void dActor_c::OnHitByMegaChar(Player &player)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19OnHitFromUnderneathERS_
/* ROM ordinal 35 -- dActor_c::OnHitFromUnderneath(dActor_c &)
 * 0x0201012c  size 0x4 */
int dActor_c::OnHitFromUnderneath(dActor_c &attacker)
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c16OnAimedAtWithEggEv
/* ROM ordinal 34 -- dActor_c::OnAimedAtWithEgg()
 * 0x02010124  size 0x8 */
int dActor_c::OnAimedAtWithEgg()
{
    return 0x14000;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c25OnAimedAtWithEggReturnVecEv
/* ROM ordinal 33 -- dActor_c::OnAimedAtWithEggReturnVec()
 * 0x020100dc  size 0x48 */
Vector3 dActor_c::OnAimedAtWithEggReturnVec()
{
    Vector3 result;
    result.x = mPosX;
    result.y = mPosY;
    result.z = mPosZ;
    result.y += OnAimedAtWithEgg();
    return result;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_
/* ROM ordinal 32 -- dActor_c::SpawnNumber(Vector3 const &, unsigned int, bool, unsigned short, dActor_c *)
 * 0x02010044  size 0x98 */
/* local extern: defined in d_a_obj_number.cpp, which has no header for it. It
   returns void there; the ROM tail-calls it, so SpawnNumber returns whatever
   it leaves in r0. */
extern "C" dActor_c *func_ov002_020f0918(daObjNumber_c *number, dActor_c *owner);
/* Spawns actor 0x14a, daObjNumber_c, the number that pops up over an actor. */
dActor_c *dActor_c::SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble, u16 delay,
                                dActor_c *owner) {
    daObjNumber_c *number;

    if (packLowNibble)
        number =
            static_cast<daObjNumber_c *>(Spawn(0x14a, (value & 0xf) | 0x10, pos, 0, mAreaId, -1));
    else
        number = static_cast<daObjNumber_c *>(Spawn(0x14a, value, pos, 0, mAreaId, -1));

    if (number == 0)
        return number;

    number->mDelay = delay;
    return func_ov002_020f0918(number, owner);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c9TrackStarEjj
/* ROM ordinal 31 -- dActor_c::TrackStar(unsigned int, unsigned int)
 * 0x0200ff94  size 0xb0 */
extern "C" {
extern u8 data_0209f220[]; /* the star index this mission was entered for */
extern s8 data_0209f2f8[]; /* the current sublevel id */
extern dActor_c *STAR_MARKERS[12]; /* 0x0209f40c */
void SetStarMarker(s32 slot, dActor_c *actor, s32 markerType);
int IsStarCollectedInCurLevel(u32 starIdx);
}
s32 dActor_c::TrackStar(u32 starIdx, u32 markerType)
{
    s32 type = markerType;

    if (markerType == 2) {
        if (starIdx != data_0209f220[0]) {
            if (SublevelToLevel(data_0209f2f8[0]) <= 0xe)
                return -1;
        }
        if (IsStarCollectedInCurLevel(starIdx))
            type = 3;
    }

    s8 slot;
    for (slot = 0; slot < 0xc; slot++) {
        if (STAR_MARKERS[slot] == 0) {
            SetStarMarker(slot, this, type);
            return slot;
        }
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11UntrackStarERa
/* ROM ordinal 30 -- dActor_c::UntrackStar(signed char &)
 * 0x0200ff60  size 0x34 */
void dActor_c::UntrackStar(s8 &starID)
{
    s8 slot = starID;
    if (slot < 0)
        return;

    SetStarMarker(slot, 0, 2);
    starID = -1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h
/* ROM ordinal 29 -- dActor_c::UntrackAndSpawnStar(signed char &, unsigned int, Vector3 const &, unsigned char)
 * 0x0200ff14  size 0x4c */
dActor_c *dActor_c::UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                                  const Vector3 &spawnPos, u8 howToSpawnStar)
{
    UntrackStar(trackStarID);

    return Spawn(0xb2, starID | (howToSpawnStar << 4), spawnPos, 0,
                 mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c17GetWaterHeightWDWEv
/* ROM ordinal 28 -- dActor_c::GetWaterHeightWDW()
 * 0x0200fec8  size 0x4c */
extern "C" {
extern unsigned char data_0209f2c0[]; /* WDW's water level, 0..2 */
extern int data_02075244[];           /* the water height for each level */
}
/* In area 0 of sublevel 0x15 (WDW), the current water height; anywhere else,
   the actor's own height. */
s32 dActor_c::GetWaterHeightWDW()
{
    s32 height = mPosY;
    if (data_0209f2f8[0] == 0x15 && mAreaId == 0) {
        unsigned int level = data_0209f2c0[0];
        if (level >= 3)
            level = 2;
        height = data_02075244[level];
    }
    return height;
}

/* -------------------------------------------------------------------------- */
// @symbol LoadBlueCoinModel
/* ROM ordinal 27 -- LoadBlueCoinModel
 * 0x0200fe9c  size 0x2c */
extern "C" {
extern SharedFilePtr data_ov002_0210da00; /* 0x0210da00 */
extern SharedFilePtr data_ov002_0210d9c8; /* 0x0210d9c8 */
}
extern "C" void LoadBlueCoinModel(void)
{
    dExtSimpleModel_c::LoadFile(data_ov002_0210da00);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c8);
}

/* -------------------------------------------------------------------------- */
// @symbol UnloadBlueCoinModel
/* ROM ordinal 26 -- UnloadBlueCoinModel
 * 0x0200fe70  size 0x2c */
extern "C" void UnloadBlueCoinModel(void)
{
    data_ov002_0210da00.Release();
    data_ov002_0210d9c8.Release();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c8PoofDustEv
/* ROM ordinal 25 -- dActor_c::PoofDust()
 * 0x0200fe3c  size 0x34 */
void dActor_c::PoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    PoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c10PoofDustAtERK7Vector3
/* ROM ordinal 24 -- dActor_c::PoofDustAt(Vector3 const &)
 * 0x0200fdfc  size 0x40 */
extern "C" {
/* local extern: Particle::System::NewSimple takes Fix12<int> by value, so it
   is called by its symbol (the Fix12 wall). */
namespace Particle {
struct System {
    static int NewSimple(unsigned int id, int x, int y, int z);
};
}
}
void dActor_c::PoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(0x122, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x123, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c14TriplePoofDustEv
/* ROM ordinal 23 -- dActor_c::TriplePoofDust()
 * 0x0200fdc8  size 0x34 */
void dActor_c::TriplePoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    TriplePoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c16TriplePoofDustAtERK7Vector3
/* ROM ordinal 22 -- dActor_c::TriplePoofDustAt(Vector3 const &)
 * 0x0200fd74  size 0x54 */
void dActor_c::TriplePoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(0x124, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x125, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x126, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13SmallPoofDustEv
/* ROM ordinal 21 -- dActor_c::SmallPoofDust()
 * 0x0200fd40  size 0x34 */
void dActor_c::SmallPoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    DisappearPoofDustAt(pos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19DisappearPoofDustAtERK7Vector3
/* ROM ordinal 20 -- dActor_c::DisappearPoofDustAt(Vector3 const &)
 * 0x0200fd04  size 0x3c */
void dActor_c::DisappearPoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(0x127, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(0x128, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200fccc
/* ROM ordinal 19 -- func_0200fccc
 * 0x0200fccc  size 0x38 */
extern "C" {
extern int func_0200fc44(int actor, Vector3 *pos, int doRaycast);
}
/* func_0200fc44 at this actor's position. The parameter keeps the char *
   every declaration of this symbol spells. */
extern "C" int func_0200fccc(char *actorp, int doRaycast) {
    dActor_c *actor = (dActor_c *)actorp;
    Vector3 pos;
    pos.x = actor->mPosX;
    pos.y = actor->mPosY;
    pos.z = actor->mPosZ;
    return func_0200fc44((int)actor, &pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200fc44
/* ROM ordinal 18 -- func_0200fc44
 * 0x0200fc44  size 0x88 */
/* Particle 0xb3 25.0 above pos, optionally first dropping pos onto the
   ground below it; pos is moved. The leading actor argument is unused. */
extern "C" int func_0200fc44(int actor, Vector3 *pos, int doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos->y += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = rc.clsnY;
    }
    pos->y += 0x19000;
    return Particle::System::NewSimple(0xb3, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c11LandingDustEb
/* ROM ordinal 17 -- dActor_c::LandingDust(bool)
 * 0x0200fc0c  size 0x38 */
void dActor_c::LandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    HugeLandingDustAt(pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c17HugeLandingDustAtER7Vector3b
/* ROM ordinal 16 -- dActor_c::HugeLandingDustAt(Vector3 &, bool)
 * 0x0200fb84  size 0x88 */
/* Particle 0xb2 40.0 above pos, optionally first dropping pos onto the
   ground below it; pos is moved. */
void dActor_c::HugeLandingDustAt(Vector3 &pos, bool doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos.y += 0x32000;
        rc.SetObjAndPos(pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
    }
    pos.y += 0x28000;
    Particle::System::NewSimple(0xb2, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c15HugeLandingDustEb
/* ROM ordinal 15 -- dActor_c::HugeLandingDust(bool)
 * 0x0200fb4c  size 0x38 */
void dActor_c::HugeLandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    LandingDustAt(pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13LandingDustAtER7Vector3b
/* ROM ordinal 14 -- dActor_c::LandingDustAt(Vector3 &, bool)
 * 0x0200fac4  size 0x88 */
/* As HugeLandingDustAt, with particle 0xb1 raised 90.0. */
void dActor_c::LandingDustAt(Vector3 &pos, bool doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos.y += 0x32000;
        rc.SetObjAndPos(pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
    }
    pos.y += 0x5a000;
    Particle::System::NewSimple(0xb1, pos.x, pos.y, pos.z);
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200fa8c
/* ROM ordinal 13 -- func_0200fa8c
 * 0x0200fa8c  size 0x38 */
extern "C" {
extern int func_0200fa04(int actor, Vector3 *pos, int doRaycast);
}
/* func_0200fa04 at this actor's position; void * as decl_common.h spells it. */
extern "C" void func_0200fa8c(void *actorp, int doRaycast) {
    dActor_c *actor = (dActor_c *)actorp;
    Vector3 pos;
    pos.x = actor->mPosX;
    pos.y = actor->mPosY;
    pos.z = actor->mPosZ;
    func_0200fa04((int)actor, &pos, doRaycast);
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200fa04
/* ROM ordinal 12 -- func_0200fa04
 * 0x0200fa04  size 0x88 */
/* As func_0200fc44, with particle 0x1b raised 120.0 instead of 25.0. */
extern "C" int func_0200fa04(int actor, Vector3 *pos, int doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos->y += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = rc.clsnY;
    }
    pos->y += 0x78000;
    return Particle::System::NewSimple(0x1b, pos->x, pos->y, pos->z);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c18GetBitInDeathTableEv
/* ROM ordinal 11 -- dActor_c::GetBitInDeathTable()
 * 0x0200f9f4  size 0x10 */
extern "C" {
int DeathTable_GetBit(int id);
}
int dActor_c::GetBitInDeathTable()
{
    return DeathTable_GetBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c17TrackInDeathTableEv
/* ROM ordinal 10 -- dActor_c::TrackInDeathTable()
 * 0x0200f9e4  size 0x10 */
extern "C" {
void DeathTable_SetBit(int id);
}
void dActor_c::TrackInDeathTable()
{
    DeathTable_SetBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19UntrackInDeathTableEv
/* ROM ordinal 9 -- dActor_c::UntrackInDeathTable()
 * 0x0200f9d4  size 0x10 */
extern "C" {
void DeathTable_ClearBit(int id);
}
void dActor_c::UntrackInDeathTable()
{
    DeathTable_ClearBit(mDeathTableID);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c24KillAndTrackInDeathTableEv
/* ROM ordinal 8 -- dActor_c::KillAndTrackInDeathTable()
 * 0x0200f9b8  size 0x1c */
void dActor_c::KillAndTrackInDeathTable()
{
    TrackInDeathTable();
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c13SpawnSoundObjEj
/* ROM ordinal 7 -- dActor_c::SpawnSoundObj(unsigned int)
 * 0x0200f97c  size 0x3c */
dActor_c *dActor_c::SpawnSoundObj(u32 soundObjParam) {
    return Spawn(0x167, soundObjParam, mPos, 0, mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN5Sound15PlaySecretSoundEP8dActor_cPt
/* ROM ordinal 6 -- Sound::PlaySecretSound(dActor_c *, unsigned short *)
 * 0x0200f8f8  size 0x84 */
extern "C" {
/* local extern: its five named parameters hold this TU's @NNN count. */
int d_actorPlaySubHold(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist,
                       int loop);
static const int kPoolDist = 0x8777;
}
namespace Sound {

int PlaySecretSound(dActor_c *actor, u16 *counter) {
    if (*counter < 0x4b) {
        Sound::PlaySub(0x20, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x20, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN5Sound20PlaySmallSecretSoundEP8dActor_cPt
/* ROM ordinal 5 -- Sound::PlaySmallSecretSound(dActor_c *, unsigned short *)
 * 0x0200f874  size 0x84 */
namespace Sound {

int PlaySmallSecretSound(dActor_c *actor, u16 *counter) {
    if (*counter < 0x19) {
        Sound::PlaySub(0x29, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x29, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200f7f0
/* ROM ordinal 4 -- func_0200f7f0
 * 0x0200f7f0  size 0x84 */
extern "C" int func_0200f7f0(dActor_c *actor, u16 *counter) {
    if (*counter < 0x69) {
        Sound::PlaySub(0x21, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x21, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c
/* ROM ordinal 3 -- dActor_c::MakeVanishLuigiWork(dCc_c &)
 * 0x0200f7a8  size 0x48 */
void dActor_c::MakeVanishLuigiWork(dCc_c &clsn)
{
    Player *player;

    clsn.flags &= ~2;

    player = ClosestPlayer();
    if (player == 0)
        return;

    if (player->mIsVanish)
        clsn.flags |= 2;
}

/* -------------------------------------------------------------------------- */
// @symbol func_0200f760
/* ROM ordinal 2 -- func_0200f760
 * 0x0200f760  size 0x48 */
/* MakeVanishLuigiWork again, as a free function taking the cylinder by
   pointer; the ROM keeps both copies. */
extern "C" void func_0200f760(dActor_c *actor, dCc_c *clsn) {
    Player *player;

    clsn->flags &= ~2;
    player = actor->ClosestPlayer();
    if (player == 0)
        return;
    if (player->mIsVanish)
        clsn->flags |= 2;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c22IsTooFarAwayFromPlayerEi
/* ROM ordinal 1 -- dActor_c::IsTooFarAwayFromPlayer(int)
 * 0x0200f70c  size 0x54 */
/* True only while the actor is off screen (mFlags 0x08), nothing forces it to
   think, and the closest player is at least threshold away. */
int dActor_c::IsTooFarAwayFromPlayer(Fix12i threshold) {
    if (data_0209fc68 == 0) {
        int offScreen = (mFlags & 8) != 0;
        if (offScreen != 0) {
            if (DistToCPlayer() >= threshold)
                return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b
/* ROM ordinal 0 -- dActor_c::DetectRaycastClsn(Vector3 &, Vector3 &, bool)
 * 0x0200f658  size 0xb4 */
extern "C" Vector3 *func_02037dc4(SurfaceInfo *surface);

bool dActor_c::DetectRaycastClsn(Vector3 &start, Vector3 &end, bool storeHitPos)
{
    dBgCh_Lin ray;
    ray.SetObjAndLine(start, end, 0);
    if (ray.DetectClsn()) {
        if (storeHitPos) {
            Vector3 *hitPos = &ray.lineEnd;
            Vector3 *normal = func_02037dc4(&ray.surface);
            int x = hitPos->x + normal->x;
            int z = hitPos->z + normal->z;
            int y = hitPos->y + normal->y;
            end.x = x;
            end.y = y;
            end.z = z;
        }
        return true;
    }
    return false;
}
