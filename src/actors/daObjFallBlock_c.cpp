//cpp
/* ov098/daObjFallBlock_c -- abstract falling-block base.
 *
 * ov098 is mixed (CRATE / CANNON / WATER_BOMB / ARROW_SIGN). This class is
 * none of those: RTTI `_ZTS16daObjFallBlock_c` lives in ov015 at 0x0211488c,
 * the vtable lives here at 0x0213c5bc, and the four leaf profiles are
 * FALL_BLOCK_WF (ov015), FALL_BLOCK_LLL (ov022), FALL_BLOCK_BFS (ov045),
 * FALL_BLOCK_BBH (ov063). ABSTRACT: slots 0 and 3 are pure, so there is no
 * classInit to fold.
 *
 * common.h FIRST: func_ov098_0213a23c writes Model::mat4x3 (this+0xf0) as a
 * rotation plus translation row. common.h's flat s32 m[12] is the spelling
 * that keeps those stores as word writes; math/Matrix.h's nested
 * {Matrix3x3 r; Vector3 t;} is the other 0x30-byte claim.
 *
 * deslop leftovers:
 * - no factory: abstract, InitResources / CleanupResources = 0. Leaves own
 *   classInit (daObjBk_Fall_Block_c_classInit / daObjFl_Fall_Block_c_classInit
 *   / daObjKm2_Fall_Block_c_classInit / daObjTh_Fall_Block_c_classInit).
 * - dBgActor_c::IsClsnInRange 6az: Behavior passes Fix12<int> by value; the
 *   header method form is refused by the bytes (include/dBgActor_c.h).
 * - Particle::System::NewSimple 6az: Kill's three by-value Fix12<int>;
 *   declaring the true types changes how the caller passes them.
 * - MarkForDestruction stays mangled in func_ov098_0213a0a8: fBase_c.h spells
 *   it void, and this helper returns the callee's r0.
 * - Behavior case 2 keeps `((int)this + 0x8c) & U64` / `+ 0x90` for mAngleX /
 *   mAngleZ: named stores size-DIFF (999 words).
 * - (long long)sinv * 0x19000 in case 1: a plain int mul size-DIFFs.
 * - common.h first: func_ov098_0213a23c's mat4x3 stores stay word writes
 *   against the flat s32 m[12] spelling.
 * - S14: no g_profile for this abstract class; leaves own their records.
 * - D0 below D1: in-class inline destructor (cartridge form for the four
 *   descendants) emits D1 then D0; the pair stays in its own shards.
 */

#include "common.h"
#include "daObjFallBlock_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"

#define U64 0xFFFFFFFFFFFFFFFFLL

extern "C" {
s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
void func_ov098_0213a00c(daObjFallBlock_c *c);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
int DecIfAbove0_Byte(u8 *p);
int DecIfAbove0_Short(u16 *p);
int Vec3_HorzDist(void *a, void *b);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
int func_ov098_0213a0a8(daObjFallBlock_c *c);
void func_ov098_0213a0e8(daObjFallBlock_c *c);
void func_ov098_0213a148(daObjFallBlock_c *c);
void func_ov098_0213a23c(daObjFallBlock_c *c);
int _ZN7fBase_c18MarkForDestructionEv(void *);
extern s16 data_02082214[];
extern signed char data_0209f2f8;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFallBlock_c8BehaviorEv
/* daObjFallBlock_c::Behavior - the whole fall-block state machine; see the
   class header for the field-by-field account. Kill() is this class's own
   named virtual (key function). UpdatePos is dActor_c's. dBgW calls go
   through mMeshCollider. IsClsnInRange stays mangled -- leftover 6az. */
s32 daObjFallBlock_c::Behavior()
{
    char *c = (char *)this;
    int all;
    daObjFallBlock_c *p;
    daObjFallBlock_c *q;
    int is53;
    s16 yaw;
    int dist;
    int thresh;
    int flag;
    s16 sinv;
    u8 *st;
    void *pl;

    is53 = (int)(actorID == 0x53);
    if (is53 != 0) {
        if (mSuppressed != 0) {
            if (mLinkedStarID == 0)
                func_ov098_0213a0e8(this);
            else
                func_ov098_0213a0a8(this);
            return 1;
        }
    }

    func_ov098_0213a00c(this);
    if (mRespawnDelay != 0) {
        if (DecIfAbove0_Byte(&mRespawnDelay) == 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        return 1;
    }

    is53 = (int)(actorID == 0x53);
    if (is53 == 0) {
        if (mPrevInGroup == 0) {
            p = this;
            all = 1;
            while (1) {
                if (p->mReady == 0) {
                    all = 0;
                    break;
                }
                p = p->mNextInGroup;
                if (p == 0)
                    break;
            }
            if (all != 0) {
                q = this;
                while (1) {
                    if (q == 0)
                        break;
                    func_ov098_0213a148(q);
                    q = q->mNextInGroup;
                }
            }
        }
    }

    switch (mState) {
    case 0:
        if (mShakeRequested != 0) {
            mReady = 0;
            mStateTimer = 4;
            st = &mState;
            *st = (u8)(*st + 1);
        }
        break;

    case 1:
        if (DecIfAbove0_Short(&mStateTimer) == 0) {
            st = &mState;
            *st = (u8)(*st + 1);
            Sound::PlayBank3(0x2d, *(Vector3 *)&mCamSpacePosX);
            mStateTimer = 0x5a;
        } else {
            sinv = data_02082214[(*(u16 *)&mBobPhase >> 4) << 1];
            /* (long long) is load-bearing: a plain int mul size-DIFFs. */
            mPosY =
                mRestPos.y
                + (int)(((long long)sinv * 0x19000 + 0x800) >> 12);
            {
                s16 *p338 = &mBobPhase;
                *p338 = (s16)(*p338 + 0x3000);
            }
        }
        break;

    case 2:
        if (mAngleX < 0x400) {
            s16 *p334 = &mShakeX;
            *p334 = (s16)(*p334 + 0x80);
        } else {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        yaw = mAngleZ;
        if (yaw > -0x400) {
            if (yaw < 0x400) {
                s16 *p336 = &mTiltVelZ;
                *p336 = (s16)(*p336 + 0x40);
            }
        }
        {
            /* Integer-cast address form: named mAngleX / mAngleZ stores
               size-DIFF Behavior (999 words). */
            s16 t = *(s16 *)(((int)c + 0x8c) & U64);
            t = (s16)(t + mShakeX);
            *(s16 *)(((int)c + 0x8c) & U64) = t;
            t = (s16)(*(s16 *)(((int)c + 0x90) & U64) + mTiltVelZ);
            *(s16 *)(((int)c + 0x90) & U64) = t;
            UpdatePos(0);
        }
        if (DecIfAbove0_Short(&mStateTimer) == 0) {
            mPosX = mRestPos.x;
            mPosY = mRestPos.y;
            mPosZ = mRestPos.z;
            st = &mState;
            *st = (u8)(*st + 1);
        } else {
            if (mPosY < mKillY) {
                Kill();
                mPosX = mRestPos.x;
                mPosY = mRestPos.y;
                mPosZ = mRestPos.z;
                st = &mState;
                *st = (u8)(*st + 1);
            }
        }
        break;

    case 3:
        mPosX = mRestPos.x;
        mPosY = mRestPos.y;
        mPosZ = mRestPos.z;
        is53 = (int)(actorID == 0x53);
        if (is53 == 0) {
            pl = ClosestPlayer();
            dist = Vec3_HorzDist(&mRestPos, (char *)pl + 0x5c);
            thresh = 0x28a000;
            if (data_0209f2f8 == 0x2e)
                thresh = 0xa000;
            flag = (mFlags & 8) ? 1 : 0;
            if (flag != 0 && dist > thresh)
                mReady = 1;
            else
                mReady = 0;
        }
        break;

    default:
        break;
    }

    func_ov098_0213a23c(this);
    if (mState <= 1) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
            UpdateClsnPosAndRot();
    }
    mShakeRequested = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFallBlock_c6RenderEv
s32 daObjFallBlock_c::Render()
{
    if (mSuppressed) return 1;
    if (mState == 3) return 1;
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a2cc
/* Shared CleanupResources body for the four leaves. Keeps the C name because
   they call it as a free function. */
extern "C" {
int func_ov098_0213a2cc(daObjFallBlock_c *t, SharedFilePtr **f)
{
    if (t->mMeshCollider.IsEnabled())
        t->mMeshCollider.Disable();
    f[0]->Release();
    f[1]->Release();
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFallBlock_c15OnHitByMegaCharER6Player
/* `player` is never read, matching the ROM body, which only ever takes the
   one (`this`) argument. */
void daObjFallBlock_c::OnHitByMegaChar(Player &player)
{
    if (mRespawnDelay != 0) return;
    Kill();
    mRespawnDelay = 0xa;
    mState = 3;
    mStateTimer = 0x3c;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a23c
extern "C" {
void func_ov098_0213a23c(daObjFallBlock_c *t)
{
    Matrix4x3_FromRotationXYZExt(&t->mModel.mat4x3, t->mAngleX, t->mAngleY, t->mAngleZ);
    t->mModel.mat4x3.m[9] = t->mPosX >> 3;
    t->mModel.mat4x3.m[10] = t->mPosY >> 3;
    t->mModel.mat4x3.m[11] = t->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFallBlock_c4KillEv
/* daObjFallBlock_c::Kill() at ov098 0x0213a17c, 0xc0 bytes -- vtable slot 31.
 *
 * The block plays its poof where it stands, 0x32000 -- fifty 20.12 units --
 * above itself, then does NOT destroy itself: it teleports back to mRestPos,
 * clears mReady and sets mStateTimer to 0x1e. That is the respawn.
 *
 * dActor_c id 0x8b (FALL_BLOCK_BFS) gets particle 0x49 instead of 0x48.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays
 * mangled -- leftover 6az. */
void daObjFallBlock_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x32000;
    u32 id = 0x48;
    /* THE INTERMEDIATE IS LOAD-BEARING. `if (actorID == 0x8b)` folds the test
       into the conditional move -- `cmp r1,#0x8b; moveq r0,#0x49`, two
       instructions. The ROM materialises the comparison into a register first
       and then tests THAT: cmp/moveq #1/movne #0/cmp #0/movne, five. Writing
       the int is what asks for the second shape. */
    int isFallBlockBfs = (actorID == 0x8b);
    if (isFallBlockBfs) {
        id = 0x49;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(id, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    mStateTimer = 0x1e;
    mPosX = mRestPos.x;
    mPosY = mRestPos.y;
    mPosZ = mRestPos.z;
    mReady = 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a148
extern "C" {
void func_ov098_0213a148(daObjFallBlock_c *c)
{
    c->mState = 0;
    c->mAngleX = 0;
    c->mAngleZ = 0;
    c->mShakeX = 0;
    c->mTiltVelZ = 0;
    c->unk_0a4 = 0;
    c->mVertSpeed = 0;
    c->unk_0ac = 0;
    c->mStateTimer = 4;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a0e8
extern "C" {
void func_ov098_0213a0e8(daObjFallBlock_c *r5)
{
    dActor_c *r1;
    r1 = dActor_c::FindWithActorID(0xb2, 0); /* POWER_STAR */
    while (r1) {
        /* POWER_STAR +0x49d */
        if (*(unsigned char *)((char *)r1 + 0x49d) == (r5->param1 & 0xf)) {
            r5->mLinkedStarID = (s32)r1->uniqueID;
        }
        r1 = dActor_c::FindWithActorID(0xb2, r1);
    }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a0a8
extern "C" {
int func_ov098_0213a0a8(daObjFallBlock_c *c)
{
    dActor_c *a = dActor_c::FindWithID((unsigned int)c->mLinkedStarID);
    if (a == 0) return _ZN7fBase_c18MarkForDestructionEv(c);
    int v = *(int *)((char *)a + 0x440); /* POWER_STAR +0x440 */
    if (v == 4) { v = 0; c->mSuppressed = 0; }
    return v;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a00c
void func_ov098_0213a00c(daObjFallBlock_c *c)
{
    if (c->unk_341 != 0) return;
    dActor_c *r = dActor_c::FindWithActorID(c->actorID, 0);
    while (r) {
        if (r != (dActor_c *)c) {
            Vector3 *b = (Vector3 *)&r->mPosX;
            Vector3 *a = (Vector3 *)&c->mPosX;
            int ang = Vec3_HorzAngle(a, b);
            if (Vec3_Dist(a, b) < 0x96000) {
                if (AngleDiff(ang, c->mAngleY) < 0x4000)
                    c->mNextInGroup = (daObjFallBlock_c *)r;
                else
                    c->mPrevInGroup = (daObjFallBlock_c *)r;
            }
        }
        r = dActor_c::FindWithActorID(c->actorID, r);
    }
    c->unk_341 = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFallBlock_cD1Ev
// @symbol _ZN16daObjFallBlock_cD0Ev
/* recovered: real C++ destructor pair -- NO SOURCE TEXT OF THEIR OWN.
 *
 * daObjFallBlock_c.h defines `~daObjFallBlock_c() {}` in the class body, and
 * the ROM carries the out-of-line D1 (0x02139fc8) and D0 (0x02139f70) anyway
 * because THIS TU owns the key function. The destructor is inline, so the key
 * function is the first non-inline, non-pure virtual -- Kill(), ROM ordinal 6
 * above -- and the TU that defines it emits _ZTV16daObjFallBlock_c together
 * with both destructor variants. The pair is held out of the licensed run:
 * the ROM orders D0 below D1, and the in-class body emits D1 then D0. */
