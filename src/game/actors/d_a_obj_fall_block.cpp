//cpp
/* Abstract falling block. Stood on, it shakes, then drops until mKillY,
 * the ground sampled under it at init. Actor ids are the leaf profiles:
 * FL_KUZURE 0x53 (ov022), KM2_KUZURE 0x8b (ov045). BK_DOWN_B and TH_DOWN_B
 * share this body. No factory and no g_profile: slots 0 and 3 are pure,
 * and the leaves construct the object.
 *
 * common.h stays first. ApplyModelXyz writes dExtSimpleModel_c::mat4x3 as a flat
 * s32 m[12]; math/Matrix.h's {Matrix3x3 r; Vector3 t;} is the other
 * spelling of the same 0x30 bytes, and this TU needs the word stores.
 * The file is in ROM order under `#pragma defer_codegen off`, with the two
 * destructors first (D0 0x02139f70, D1 0x02139fc8): the inline destructor
 * emits D1 then D0 under deferred codegen, and the ROM has D0 below D1.
 */

// Notes (each measured with mwccarm 2004/b56):
// - InitResourcesFrom and CleanupResourcesFrom are the shared bodies of
//   the four leaves' InitResources and CleanupResources. Each leaf passes
//   its own ResourceDescriptor; the virtual slots stay pure here.
// - func_020393c4 is `p[7] = v`, the store of StoodOnCallback into
//   dBgW::stoodOnCallback. The ROM calls it; an inlined store would drop
//   the bl. d_bg_w.cpp defines it and no header declares it.
// - StoodOnCallback keeps long_calls. The ROM tail is the pooled absolute
//   veneer (size 0x14); a near branch to RequestShake is 0xc.
// - RequestShake keeps the unused second parameter. The veneer forwards
//   it, and a 1-arg callee drops `mov r1, r2`.
// - Behavior case 2 keeps the masked address of mAngleX (this+0x8c) and
//   mAngleZ (this+0x90). Named stores change the size. Case 1 keeps
//   `(long long)sinv * 0x19000`.
// - Kill materialises `(actorID == KM2_KUZURE)` into an int before the
//   branch. Folding the test changes the compare.

#pragma defer_codegen off

#include "common.h"
#include "daObjFallBlock_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "daStar_c.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"

#define U64 0xFFFFFFFFFFFFFFFFLL

/* Profile actor ids (symbols/profile_reconstruction_registry.json). */
enum { ACTOR_FL_KUZURE = 0x53, ACTOR_KM2_KUZURE = 0x8b, ACTOR_STAR = 0xb2 };

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
#ifndef SM64DS_PLATFORM_PC
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];
#endif

extern "C" {
s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
int DecIfAbove0_Byte(u8 *p);
int DecIfAbove0_Short(u16 *p);
int Vec3_HorzDist(void *a, void *b);

/* local extern: d_bg_w.cpp defines it and no header declares it. */
void func_020393c4(int *p, int v);
}

extern s16 data_02082214[];       /* sine table */
extern signed char data_0209f2f8; /* current level id */

// @symbol _ZN16daObjFallBlock_cD0Ev
// @symbol _ZN16daObjFallBlock_cD1Ev
/* ROM ordinals 0 and 1 -- ov098 0x02139f70 (D0, 0x58) and 0x02139fc8 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjFallBlock_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjFallBlock_c_EmitDeletingDestructor(daObjFallBlock_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjFallBlock_c_EmitDestructor(daObjFallBlock_c *p) {
    p->~daObjFallBlock_c();
}

// @symbol _ZN16daObjFallBlock_c9LinkGroupEv
/* Once (mGroupLinked). Same-id neighbours closer than 0x96000 become
   mNextInGroup when they sit within a quarter-turn of mAngleY, and
   mPrevInGroup otherwise. Vec3_HorzAngle / Vec3_Dist / AngleDiff. */
void daObjFallBlock_c::LinkGroup() {
    if (mGroupLinked != 0)
        return;
    dActor_c *r = dActor_c::FindWithActorID(actorID, 0);
    while (r) {
        if (r != this) {
            Vector3 *b = &r->mPos;
            Vector3 *a = &mPos;
            int ang = Vec3_HorzAngle(a, b);
            if (Vec3_Dist(a, b) < 0x96000) {
                if (AngleDiff(ang, mAngleY) < 0x4000)
                    mNextInGroup = (daObjFallBlock_c *)r;
                else
                    mPrevInGroup = (daObjFallBlock_c *)r;
            }
        }
        r = dActor_c::FindWithActorID(actorID, r);
    }
    mGroupLinked = 1;
}

// @symbol _ZN16daObjFallBlock_c14PollLinkedStarEv
/* unk_440 is daStar_c's state index into data_ov002_021109d8.
   Slot 4 is func_ov002_020ea420. Reaching it clears mSuppressed so
   this FL_KUZURE block renders and runs. A missing star destroys it. */
void daObjFallBlock_c::PollLinkedStar() {
    dActor_c *a = dActor_c::FindWithID((unsigned int)mLinkedStarID);
    if (a == 0) {
        MarkForDestruction();
        return;
    }
    if (static_cast<daStar_c *>(a)->unk_440 == 4)
        mSuppressed = 0;
}

// @symbol _ZN16daObjFallBlock_c14FindLinkedStarEv
/* STAR whose unk_49d equals param1's low nibble. AddStarMarker passes
   that byte to IsStarCollectedInCurLevel, so it is the star index. */
void daObjFallBlock_c::FindLinkedStar() {
    dActor_c *star;
    star = dActor_c::FindWithActorID(ACTOR_STAR, 0);
    while (star) {
        if (static_cast<daStar_c *>(star)->unk_49d == (param1 & 0xf)) {
            mLinkedStarID = (s32)star->uniqueID;
        }
        star = dActor_c::FindWithActorID(ACTOR_STAR, star);
    }
}

// @symbol _ZN16daObjFallBlock_c11ResetMotionEv
/* Group re-arm. Clears the shake and the fall, and gives case 0 four
   frames before it will accept another stand. */
void daObjFallBlock_c::ResetMotion() {
    mState = 0;
    mAngleX = 0;
    mAngleZ = 0;
    mShakeX = 0;
    mTiltVelZ = 0;
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    mStateTimer = 4;
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
 * instructions where the ROM has six. */
void daObjFallBlock_c::Kill() {
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
    Particle::System::NewSimple(id, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, mCamSpacePos);
    mStateTimer = 0x1e;
    mPosX = mRestPos.x;
    mPosY = mRestPos.y;
    mPosZ = mRestPos.z;
    mReady = 0;
}

// @symbol _ZN16daObjFallBlock_c13ApplyModelXyzEv
/* Full XYZ model matrix. Translation is model space, position >> 3,
   same row UpdateModelPosAndRotY writes for yaw alone. */
void daObjFallBlock_c::ApplyModelXyz() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN16daObjFallBlock_c15OnHitByMegaCharER6Player
/* `player` is never read, matching the ROM body, which only ever takes the
   one (`this`) argument. */
void daObjFallBlock_c::OnHitByMegaChar(Player &player) {
    if (mRespawnDelay != 0)
        return;
    Kill();
    mRespawnDelay = 0xa;
    mState = 3;
    mStateTimer = 0x3c;
}

// @symbol _ZN16daObjFallBlock_c20CleanupResourcesFromEP18ResourceDescriptor
/* Shared CleanupResources for the four leaves. Slot 0 is the model
   SharedFilePtr, slot 1 the collision file. */
s32 daObjFallBlock_c::CleanupResourcesFrom(ResourceDescriptor *files) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    files->model->Release();
    files->collision->Release();
    return 1;
}

// @symbol _ZN16daObjFallBlock_c6RenderEv
s32 daObjFallBlock_c::Render() {
    if (mSuppressed)
        return 1;
    if (mState == 3)
        return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjFallBlock_c8BehaviorEv
/* daObjFallBlock_c::Behavior - the whole fall-block state machine; see the
   class header for the field-by-field account. Kill() is this class's own
   named virtual (key function). UpdatePos is dActor_c's. dBgW calls go
   through mMeshCollider. */
s32 daObjFallBlock_c::Behavior() {
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
                FindLinkedStar();
            else
                PollLinkedStar();
            return 1;
        }
    }

    LinkGroup();
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
                    q->ResetMotion();
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
            Sound::PlayBank3(0x2d, mCamSpacePos);
            mStateTimer = 0x5a;
        } else {
            sinv = data_02082214[(*(u16 *)&mBobPhase >> 4) << 1];
            /* (long long) is load-bearing: a plain int mul changes the code
               size. */
            mPosY = mRestPos.y + FX_MUL(sinv, 0x19000);
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
            /* Integer-cast address form: plain mAngleX / mAngleZ
               read-modify-writes shrink Behavior by 8 bytes. */
            s16 t = *(s16 *)((int)&mAngleX & U64);
            t = (s16)(t + mShakeX);
            *(s16 *)((int)&mAngleX & U64) = t;
            t = (s16)(*(s16 *)((int)&mAngleZ & U64) + mTiltVelZ);
            *(s16 *)((int)&mAngleZ & U64) = t;
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
            dist = Vec3_HorzDist(&mRestPos, &((dActor_c *)pl)->mPos);
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

    ApplyModelXyz();
    if (mState <= 1) {
        if (IsClsnInRange(0, 0) != 0)
            UpdateClsnPosAndRot();
    }
    mShakeRequested = 0;
    return 1;
}

// @symbol _ZN16daObjFallBlock_c17InitResourcesFromEP18ResourceDescriptor
/* Shared InitResources body for the four leaves. Slot 0 is dExtSimpleModel_c::LoadFile,
   slot 1 is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile. Ground-raycasts
   mKillY, copies mRestPos, and arms the StoodOnCallback mesh callback. */
s32 daObjFallBlock_c::InitResourcesFrom(ResourceDescriptor *fp) {
    Vector3 v;
    int on;
    int y;

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*fp->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)(dBgW_Kc::LoadFile(*fp->collision)), mClsnMat, 0x199,
                              mAngleY, *fp->clps);
    func_020393c4((int *)&mMeshCollider, (int)&StoodOnCallback);
    v.x = mPosX;
    y = mPosY;
    v.y = y;
    v.z = mPosZ;
    v.y = y - 0x64000;
    {
        dBgCh_Gnd rc;
        rc.SetObjAndPos(v, 0);
        mKillY = v.y;
        if (rc.DetectClsn())
            mKillY = rc.clsnY;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0xc8000;
        on = 1;
        mStateTimer = 4;
        mReady = on;
        mRestPos.x = mPosX;
        mRestPos.y = mPosY;
        mRestPos.z = mPosZ;
        mLinkedStarID = 0;
        if (actorID != ACTOR_FL_KUZURE)
            on = 0;
        if (on != 0)
            mSuppressed = 1;
    }
    return 1;
}

// @symbol _ZN16daObjFallBlock_c12RequestShakeEP8dActor_c
/* Sets mShakeRequested so Behavior case 0 starts the shake. The actor
   parameter is the veneer's extra forwarded register -- a 1-arg callee
   drops mov r1,r2. */
int daObjFallBlock_c::RequestShake(dActor_c *other) {
    mShakeRequested = 1;
}

// @symbol _ZN16daObjFallBlock_c15StoodOnCallbackEP4dBgWPS_P8dActor_c
/* dBgW+0x1c callback veneer. Drops the collider and forwards the actor into
   RequestShake. long_calls is the ROM's pooled absolute tail-call. */
#pragma long_calls on
int daObjFallBlock_c::StoodOnCallback(dBgW *collider, daObjFallBlock_c *block, dActor_c *other) {
    return block->RequestShake(other);
}
#pragma long_calls off
