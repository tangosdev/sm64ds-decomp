//cpp
/* Abstract falling block. Stood on, it shakes, then drops until mKillY,
 * the ground sampled under it at init. Actor ids are the leaf profiles:
 * FL_KUZURE 0x53 (ov022), KM2_KUZURE 0x8b (ov045). BK_DOWN_B and TH_DOWN_B
 * share this body. No factory and no g_profile: slots 0 and 3 are pure,
 * and the leaves construct the object.
 *
 * common.h stays first. ApplyModelXyz writes Model::mat4x3 as a flat
 * s32 m[12]; math/Matrix.h's {Matrix3x3 r; Vector3 t;} is the other
 * spelling of the same 0x30 bytes, and this TU needs the word stores.
 *
 * Leftover: func_ov098_0213a2cc is shared CleanupResources. Its body
 *   disables mMeshCollider and Release()s the model and collision
 *   SharedFilePtrs (descriptor slots 0 and 1). The four leaves import
 *   that linker name, and the virtual slot stays pure, so it is not
 *   renamed here.
 * Leftover: dBgW_KcMbg::SetFile's definition takes an int scale (this
 *   InitResources passes 0x199). The header method takes Fix12<int> by
 *   value and changes the caller, so the call stays the mangled symbol.
 * Leftover: dBgActor_c::IsClsnInRange's definition takes two ints
 *   (Behavior passes 0, 0). There is no int method on the class.
 * Leftover: Particle::System::NewSimple's definition takes three s32
 *   positions (Kill). A Fix12<int> declaration changes the caller.
 * Leftover: func_020393c4 is `p[7] = v`, the store of OnStoodOn into
 *   dBgW::unk_1c. The ROM calls it; an inlined store would drop the bl.
 *   Naming belongs on dBgW.
 * Leftover: OnStoodOn keeps long_calls. The ROM tail is the pooled
 *   absolute veneer (size 0x14); a near branch to RequestShake is 0xc.
 * Leftover: RequestShake keeps the unused second parameter. The veneer
 *   forwards it, and a 1-arg callee drops `mov r1, r2`.
 * Leftover: Behavior case 2 keeps the masked address of mAngleX
 *   (this+0x8c) and mAngleZ (this+0x90). Named stores change the size.
 *   Case 1 keeps `(long long)sinv * 0x19000`.
 * Leftover: Kill materialises `(actorID == KM2_KUZURE)` into an int
 *   before the branch. Folding the test changes the compare.
 * Leftover: the inline destructor emits D1 then D0; the ROM has D0
 *   below D1, so the pair stays in its own shards.
 */

#include "common.h"
#include "daObjFallBlock_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "PowerStar.h"

#define U64 0xFFFFFFFFFFFFFFFFLL

/* Profile actor ids (symbols/profile_reconstruction_registry.json). */
enum {
    ACTOR_FL_KUZURE = 0x53,
    ACTOR_KM2_KUZURE = 0x8b,
    ACTOR_STAR = 0xb2
};

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
#ifndef SM64DS_PLATFORM_PC
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];
#endif

extern "C" {
s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
void daObjFallBlock_c_LinkGroup(daObjFallBlock_c *c);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
int DecIfAbove0_Byte(u8 *p);
int DecIfAbove0_Short(u16 *p);
int Vec3_HorzDist(void *a, void *b);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int radius, int yOffset);
void daObjFallBlock_c_PollLinkedStar(daObjFallBlock_c *c);
void daObjFallBlock_c_FindLinkedStar(daObjFallBlock_c *c);
void daObjFallBlock_c_ResetMotion(daObjFallBlock_c *c);
void daObjFallBlock_c_ApplyModelXyz(daObjFallBlock_c *c);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const Matrix4x3 *mat, int scale, short angle, void *clps);
void func_020393c4(int *p, int v);
int daObjFallBlock_c_RequestShake(daObjFallBlock_c *block, void *unused);
int daObjFallBlock_c_OnStoodOn(void *collider, daObjFallBlock_c *block, void *unused);
int daObjFallBlock_c_InitResources(daObjFallBlock_c *self, ResourceDescriptor *fp);
extern s16 data_02082214[]; /* sine table */
extern signed char data_0209f2f8; /* current level id */
}

/* Free definitions take ints. SetFile's header method takes Fix12<int>
   by value; IsClsnInRange and NewSimple have no int method. */
#define dBgW_KcMbg_SetFile _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
#define dBgActor_c_IsClsnInRange _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_
#define Particle_System_NewSimple _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_

// @symbol daObjFallBlock_c_OnStoodOn
/* dBgW+0x1c callback veneer. Drops the collider and forwards the actor into
   RequestShake. long_calls is the ROM's pooled absolute tail-call. */
extern "C" {
#pragma long_calls on
int daObjFallBlock_c_OnStoodOn(void *collider, daObjFallBlock_c *block, void *unused)
{
    return daObjFallBlock_c_RequestShake(block, unused);
}
#pragma long_calls off
}

// @symbol daObjFallBlock_c_RequestShake
/* Sets mShakeRequested so Behavior case 0 starts the shake. Second parameter
   is the veneer's extra forwarded register -- a 1-arg callee drops mov r1,r2. */
extern "C" {
int daObjFallBlock_c_RequestShake(daObjFallBlock_c *block, void *unused)
{
    block->mShakeRequested = 1;
}
}

// @symbol daObjFallBlock_c_InitResources
/* Shared InitResources body for the four leaves. Slot 0 is Model::LoadFile,
   slot 1 is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile. Ground-raycasts
   mKillY, copies mRestPos, and arms the OnStoodOn mesh callback. */
extern "C" {
int daObjFallBlock_c_InitResources(daObjFallBlock_c *self, ResourceDescriptor *fp)
{
    Vector3 v;
    int on;
    int y;

    self->mModel.SetFile((BMD_File *)Model::LoadFile(*fp->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    dBgW_KcMbg_SetFile(
        &self->mMeshCollider,
        dBgW_Kc::LoadFile(*fp->collision),
        &self->mClsnMat,
        0x199,
        self->mAngleY,
        fp->clps);
    func_020393c4((int *)&self->mMeshCollider, (int)&daObjFallBlock_c_OnStoodOn);
    v.x = self->mPosX;
    y = self->mPosY;
    v.y = y;
    v.z = self->mPosZ;
    v.y = y - 0x64000;
    {
        dBgCh_Gnd rc;
        rc.SetObjAndPos(v, 0);
        self->mKillY = v.y;
        if (rc.DetectClsn())
            self->mKillY = rc.clsnY;
        self->mVertAccel = -0x4000;
        self->mTerminalVelocity = -0xc8000;
        on = 1;
        self->mStateTimer = 4;
        self->mReady = on;
        self->mRestPos.x = self->mPosX;
        self->mRestPos.y = self->mPosY;
        self->mRestPos.z = self->mPosZ;
        self->mLinkedStarID = 0;
        if (self->actorID != ACTOR_FL_KUZURE)
            on = 0;
        if (on != 0)
            self->mSuppressed = 1;
    }
    return 1;
}
}

// @symbol _ZN16daObjFallBlock_c8BehaviorEv
/* daObjFallBlock_c::Behavior - the whole fall-block state machine; see the
   class header for the field-by-field account. Kill() is this class's own
   named virtual (key function). UpdatePos is dActor_c's. dBgW calls go
   through mMeshCollider. IsClsnInRange stays mangled --
   notes/mwccarm-codegen.md 6az. */
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

    is53 = (int)(actorID == ACTOR_FL_KUZURE);
    if (is53 != 0) {
        if (mSuppressed != 0) {
            if (mLinkedStarID == 0)
                daObjFallBlock_c_FindLinkedStar(this);
            else
                daObjFallBlock_c_PollLinkedStar(this);
            return 1;
        }
    }

    daObjFallBlock_c_LinkGroup(this);
    if (mRespawnDelay != 0) {
        if (DecIfAbove0_Byte(&mRespawnDelay) == 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        return 1;
    }

    is53 = (int)(actorID == ACTOR_FL_KUZURE);
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
                    daObjFallBlock_c_ResetMotion(q);
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
            /* (long long) is load-bearing: a plain int mul changes the code
               size. */
            mPosY =
                mRestPos.y
                + (int)(((long long)sinv * 0x19000 + 0x800) >> 12);
            {
                s16 *bobPhase = &mBobPhase;
                *bobPhase = (s16)(*bobPhase + 0x3000);
            }
        }
        break;

    case 2:
        if (mAngleX < 0x400) {
            s16 *shakeX = &mShakeX;
            *shakeX = (s16)(*shakeX + 0x80);
        } else {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        yaw = mAngleZ;
        if (yaw > -0x400) {
            if (yaw < 0x400) {
                s16 *tiltVelZ = &mTiltVelZ;
                *tiltVelZ = (s16)(*tiltVelZ + 0x40);
            }
        }
        {
            /* Integer-cast address form: named mAngleX / mAngleZ stores
               change the code size of Behavior. */
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
        is53 = (int)(actorID == ACTOR_FL_KUZURE);
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

    daObjFallBlock_c_ApplyModelXyz(this);
    if (mState <= 1) {
        if (dBgActor_c_IsClsnInRange(this, 0, 0) != 0)
            UpdateClsnPosAndRot();
    }
    mShakeRequested = 0;
    return 1;
}

// @symbol _ZN16daObjFallBlock_c6RenderEv
s32 daObjFallBlock_c::Render()
{
    if (mSuppressed) return 1;
    if (mState == 3) return 1;
    mModel.Render(0);
    return 1;
}

// @symbol func_ov098_0213a2cc
/* Shared CleanupResources. Linker name stays: the four leaves import it.
   Slot 0 is the model SharedFilePtr, slot 1 the collision file. */
extern "C" {
int func_ov098_0213a2cc(daObjFallBlock_c *t, ResourceDescriptor *files)
{
    if (t->mMeshCollider.IsEnabled())
        t->mMeshCollider.Disable();
    files->model->Release();
    files->collision->Release();
    return 1;
}
}

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

// @symbol daObjFallBlock_c_ApplyModelXyz
/* Full XYZ model matrix. Translation is model space, position >> 3,
   same row UpdateModelPosAndRotY writes for yaw alone. */
extern "C" {
void daObjFallBlock_c_ApplyModelXyz(daObjFallBlock_c *t)
{
    Matrix4x3_FromRotationXYZExt(&t->mModel.mat4x3, t->mAngleX, t->mAngleY, t->mAngleZ);
    t->mModel.mat4x3.m[9] = t->mPosX >> 3;
    t->mModel.mat4x3.m[10] = t->mPosY >> 3;
    t->mModel.mat4x3.m[11] = t->mPosZ >> 3;
}
}

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
 * mangled -- notes/mwccarm-codegen.md 6az. */
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
    int isFallBlockBfs = (actorID == ACTOR_KM2_KUZURE);
    if (isFallBlockBfs) {
        id = 0x49;
    }
    Particle_System_NewSimple(id, pos.x, pos.y, pos.z);
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

// @symbol daObjFallBlock_c_ResetMotion
/* Group re-arm. Clears the shake and the fall, and gives case 0 four
   frames before it will accept another stand. */
extern "C" {
void daObjFallBlock_c_ResetMotion(daObjFallBlock_c *c)
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

// @symbol daObjFallBlock_c_FindLinkedStar
/* STAR whose unk_49d equals param1's low nibble. AddStarMarker passes
   that byte to IsStarCollectedInCurLevel, so it is the star index. */
extern "C" {
void daObjFallBlock_c_FindLinkedStar(daObjFallBlock_c *self)
{
    dActor_c *star;
    star = dActor_c::FindWithActorID(ACTOR_STAR, 0);
    while (star) {
        if (static_cast<PowerStar *>(star)->unk_49d == (self->param1 & 0xf)) {
            self->mLinkedStarID = (s32)star->uniqueID;
        }
        star = dActor_c::FindWithActorID(ACTOR_STAR, star);
    }
}
}

// @symbol daObjFallBlock_c_PollLinkedStar
/* unk_440 is PowerStar's state index into data_ov002_021109d8.
   Slot 4 is func_ov002_020ea420. Reaching it clears mSuppressed so
   this FL_KUZURE block renders and runs. A missing star destroys it. */
extern "C" {
void daObjFallBlock_c_PollLinkedStar(daObjFallBlock_c *c)
{
    dActor_c *a = dActor_c::FindWithID((unsigned int)c->mLinkedStarID);
    if (a == 0) {
        c->MarkForDestruction();
        return;
    }
    if (static_cast<PowerStar *>(a)->unk_440 == 4)
        c->mSuppressed = 0;
}
}

// @symbol daObjFallBlock_c_LinkGroup
/* Once (unk_341). Same-id neighbours closer than 0x96000 become
   mNextInGroup when they sit within a quarter-turn of mAngleY, and
   mPrevInGroup otherwise. Vec3_HorzAngle / Vec3_Dist / AngleDiff. */
void daObjFallBlock_c_LinkGroup(daObjFallBlock_c *c)
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

// @symbol _ZN16daObjFallBlock_cD1Ev
// @symbol _ZN16daObjFallBlock_cD0Ev
/* The real C++ destructor pair -- NO SOURCE TEXT OF THEIR OWN.
 *
 * daObjFallBlock_c.h defines `~daObjFallBlock_c() {}` in the class body, and
 * the ROM carries the out-of-line D1 (0x02139fc8) and D0 (0x02139f70) anyway
 * because THIS TU owns the key function. The destructor is inline, so the key
 * function is the first non-inline, non-pure virtual -- Kill(), ROM ordinal 6
 * above -- and the TU that defines it emits _ZTV16daObjFallBlock_c together
 * with both destructor variants. The pair is held out of the licensed run:
 * the ROM orders D0 below D1, and the in-class body emits D1 then D0. */
