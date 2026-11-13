//cpp
/**
 * dPathLiftActor_c -- the path-following lift base.
 *
 * Production consolidation: the two destructors (D0 0x020ef320, D1 0x020ef390),
 * 19 class methods and the existing C bridge occupy ov002:0x020ef320..0x020effb8.
 * This contiguous range is packaging evidence, not proof of the original TU.
 *
 * The state table at 0x0210af2c is the file-scope PathLiftState array defined
 * at the bottom of this file; mwcc emits __sinit_d_path_lift_actor.cpp to copy
 * its six pointer-to-member descriptors. The scale triple data_ov002_0210af00
 * and the class metadata emit with it.
 *
 * Function order is the ROM's: the file compiles under `#pragma defer_codegen
 * off`, where mwccarm 2004/b56 emits ordinary functions at parse time in source
 * order, which is the only measured way to get the destructor pair out D0
 * before D1. Do not reorder. The "// address (size)" line above each definition
 * is its ROM location.
 *
 * Known limits:
 * - data_ov002_0210d9f0 is a SharedFilePtr, read here through a local
 *   PathStuff struct: including SharedFilePtr.h renumbers the @NNN labels.
 * - IsUnk42cSet keeps its symbol-bound name; the field it reads is
 *   mRespawnAfterFall (see include/PathLift.h).
 * - StateFall keeps two views of mFallAngle: a signed read and an
 *   (int)-laundered unsigned read-modify-write. Collapsing both into
 *   mFallAngle makes mwcc reuse the signed load and drops 12 bytes.
 */

#pragma defer_codegen off

// Header order preserves the measured mwccarm 2004/b56 output.
#include "PathLift.h"
#include "types.h"
#include "decl_PathPtr.h"
#include "decl_common.h"
#include "common.h"
#include "math/Approach.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"

typedef void (dPathLiftActor_c::*PathLiftStateFn)();

struct PathLiftState {
    PathLiftStateFn init;
    PathLiftStateFn behavior;
    const char *name;
};

/* POD spelling of Vector3's 0x0c bytes: with a user-declared constructor on
   Vector3, copy-init lowers memberwise; the ROM copy is the block move, so the
   per-frame local in RenderPathModels is a VecFx32. */
/* The shared lift model file, in SharedFilePtr's layout. This TU does not
   include SharedFilePtr.h: the header's declarations take one compiler
   label number, and every @NNN of the state table would move up by one. */
struct BMD_File;
struct PathStuff {
    u16 fileID;
    u8 numRefs;
    void *filePtr;
};
extern PathStuff data_ov002_0210d9f0;

/* ROM symbols with no header of their own. */
extern "C" {

extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

extern short Vec3_VertAngle(const Vector3 *from, const Vector3 *to);
extern "C" {

extern void Vec3_MulScalar(Vector3 *out, const Vector3 *v, int s);
}

/* Sound.cpp: plays sound `id' at a camera-space position. */
void func_02012694(unsigned int id, const Vector3 *camSpacePos);
extern "C" {

/* dBgW.cpp: sets a collider's stoodOnCallback. */

/* Defined last: the ROM-ascending order puts the collider callback after
   BaseInitResources, which stores its address. */
void func_ov002_020eff90(dBgW *collider, dActor_c *lift, dActor_c *rider);
}


extern PathLiftState data_ov002_0210af2c[];

/* Which lift this is. An int-returning inline: the ROM materialises the 0/1
   result before it branches on it. */
static inline int IsActorID(const dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* ROM ordinals 0 and 1 -- ov002 0x020ef320 (D0, 0x70) and 0x020ef390 (D1,
 * 0x5c). No source here: both destructor variants come from the ONE inline
 * body in include/PathLift.h, which the class's descendants need visible to
 * inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. */

#ifndef _MSC_VER
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void dPathLiftActor_c_EmitDeletingDestructor(dPathLiftActor_c *p)
{
    delete p;
}
#endif

/* Not called. Forces the out-of-line copy of the inline destructor. */
void dPathLiftActor_c_EmitDestructor(dPathLiftActor_c *p)
{
    p->~dPathLiftActor_c();
}

// 0x020ef3ec (0x4)
void dPathLiftActor_c::StateWait()
{
}

// 0x020ef3f0 (0x18)
void dPathLiftActor_c::StateWaitInit()
{
    mTriggerDelay = 24;
    mWaitTimer = 300;
}

// 0x020ef408 (0x174)
void dPathLiftActor_c::StateFall()
{
    s32 raw;
    s16 v;

    if (Param08ModeIs2() != 0) {
        if (mAfterClsnRan == 0) {
            if (DecIfAbove0_Short(&mWaitTimer) == 0) {
                ResetPath();
                return;
            }
        } else {
            mWaitTimer = 0x12c;
        }
    }
    if (DecIfAbove0_Byte(&mFallDelay) != 0)
        return;
    if (DecIfAbove0_Byte(&mFallBounceTimer) != 0) {
        /* Retain the mixed signed/unsigned access tree: collapsing both views
           into mFallAngle makes mwcc reuse the signed load and drops 12 bytes. */
        raw = mFallAngle;
        v = FX_SinIdx((u16)(int)(long long)raw >> 4);
        mPosY = mFallStartY + (s32)(u32)((((long long)v << 15) + 0x800) >> 12);
        *(u16 *)((int)&mFallAngle) = (u16)(*(u16 *)((int)&mFallAngle) + 0x3000);
        return;
    }
    UpdatePos(0);
    if (HasNonzeroAngleZ() != 0)
        ApproachLinear(mAngleX, 0x3000, 0x100);
    if (DecIfAbove0_Byte(&mTriggerDelay) != 0)
        return;
    if (IsUnk42cSet() != 0) {
        ResetPath();
        return;
    }
    MarkForDestruction();
}

// 0x020ef57c (0xf4)
void dPathLiftActor_c::StateFallInit() {
    if (IsActorID(this, 0x82)) {
        mVertAccel = -0x1000;
        mTerminalVelocity = -0x1e000;
    } else {
        mVertAccel = -0x2000;
        mTerminalVelocity = -0x3c000;
    }
    mTriggerDelay = 0xfa;
    mPrevAngleX = mAngleX;
    mPrevAngleY = mAngleY;
    mPrevAngleZ = mAngleZ;
    mVertSpeed = 0;
    if (HasNonzeroAngleZ() == 0) {
        mFallDelay = 0x3c;
        mFallBounceTimer = 0;
    } else {
        mFallDelay = 0x1e;
        mFallBounceTimer = 0x18;
        mFallAngle = 0;
        mFallStartY = mPosY;
    }
    if (IsActorID(this, 0x82))
        mHorzSpeed = Vec3_HorzDist(&mPos, &mPrevPos);
}

// 0x020ef670 (0x3d4)
void dPathLiftActor_c::StatePath()
{
    Vector3 destNode, srcNode, fromDest, segment, stepA, stepB;
    int arrived;
    int pitchDiff;
    int yawDiff;
    int horzDist;
    int segFrames;
    int segLen;
    int srcIdx;
    short targetYaw;
    short targetPitch;
    short pitchStep;
    int destDist;

    if (Param08ModeIs2() != 0) {
        if (mAfterClsnRan == 0) {
            if (DecIfAbove0_Short(&mWaitTimer) == 0) {
                ResetPath();
                return;
            }
        } else {
            mWaitTimer = 0x12c;
        }
    }

    srcIdx = mCurrPathNode - mPathDirection;
    arrived = 0;
    if (mPath.Loops()) {
        if (mPathDirection > 0) {
            if (srcIdx < 0) srcIdx = mPath.NumNodes() - 1;
        } else {
            if (srcIdx >= (int)mPath.NumNodes()) srcIdx = 0;
        }
        mPath.GetNode(srcNode, srcIdx);
    } else {
        if (srcIdx < 0 || srcIdx >= (int)mPath.NumNodes()) {
            srcNode.x = mPosX;
            srcNode.y = mPosY;
            srcNode.z = mPosZ;
        } else {
            mPath.GetNode(srcNode, srcIdx);
        }
    }
    mPath.GetNode(destNode, mCurrPathNode);

    Vec3_Sub(&fromDest, &mPos, &destNode);
    destDist = VEC_Mag(&fromDest);
    Vec3_Sub(&segment, &srcNode, &destNode);
    segLen = VEC_Mag(&segment);
    segFrames = FX_Div(segLen, mHorzSpeed) / 0x1000;

    horzDist = Vec3_HorzDist(&srcNode, &destNode);
    if (horzDist != 0) {
        if (mPathDirection > 0) {
            targetYaw = Vec3_HorzAngle(&srcNode, &destNode);
            targetPitch = Vec3_VertAngle(&srcNode, &destNode);
        } else {
            targetYaw = Vec3_HorzAngle(&destNode, &srcNode);
            targetPitch = Vec3_VertAngle(&destNode, &srcNode);
        }
    } else {
        targetYaw = mAngleY;
        targetPitch = mAngleX;
    }

    yawDiff = AngleDiff(mPrevPathAngle.y, targetYaw);
    pitchDiff = AngleDiff(mPrevPathAngle.x, targetPitch);
    pitchStep = (short)(pitchDiff / segFrames);
    ApproachLinear(mAngleY, targetYaw, (short)(yawDiff / segFrames));
    if (HasNonzeroAngleZ() != 0) {
        ApproachLinear(mAngleX, targetPitch, pitchStep);
    }

    if (destDist == 0 || destDist <= mHorzSpeed) {
        Vec3_MulScalar(&stepA, &fromDest, FX_Div(mHorzSpeed, destDist));
        VEC_Subtract(&mPos, &stepA, &mPos);
        arrived = 1;
    } else {
        Vec3_MulScalar(&stepB, &fromDest, FX_Div(mHorzSpeed, destDist));
        VEC_Subtract(&mPos, &stepB, &mPos);
    }

    if (arrived == 0) return;

    mCurrPathNode += mPathDirection;
    if (mCurrPathNode < 0) {
        if (mPath.Loops()) {
            mCurrPathNode = mPath.NumNodes() - 1;
        } else {
            mTriggerDelay = 0x14;
            mPathDirection = 1;
            mCurrPathNode += mPathDirection * 2;
        }
    }
    if (mCurrPathNode >= (int)mPath.NumNodes()) {
        if (mPath.Loops()) {
            mCurrPathNode = 0;
        } else if (Param10ModeIs1() != 0) {
            SetState(2);
        } else {
            mTriggerDelay = 0x14;
            mPathDirection = -1;
            mCurrPathNode += mPathDirection * 2;
        }
    }

    mPrevPathAngle.x = mAngleX;
    mPrevPathAngle.y = mAngleY;
    mPrevPathAngle.z = mAngleZ;
}

// 0x020efa44 (0x10)
void dPathLiftActor_c::StatePathInit()
{
    mWaitTimer = 300;
}

// 0x020efa54 (0x4c)
void dPathLiftActor_c::SetState(int state) {
    mState = state;
    int next = mState;
    (this->*data_ov002_0210af2c[next].init)();
}

// 0x020efaa0 (0x50)
void dPathLiftActor_c::BaseBehavior()
{
    PathLiftState &state = data_ov002_0210af2c[mState];
    (this->*state.behavior)();
    mAfterClsnRan = 0;
}

// 0x020efaf0 (0xec)
void dPathLiftActor_c::BaseInitResources()
{
    for (int i = 0; i < 3; i++)
        mModels[i].SetFile((BMD_File *)data_ov002_0210d9f0.filePtr, 1, -1);

    mPath.FromID(param1 & 0xff);
    if (Param08ModeIs1Or2()) SetState(0);
    else SetState(1);

    mPrevPathAngle.x = mAngleX;
    mPrevPathAngle.y = mAngleY;
    mPrevPathAngle.z = mAngleZ;
    mInitialPos.x = mPosX;
    mInitialPos.y = mPosY;
    mInitialPos.z = mPosZ;
    mInitialAngle.x = mAngleX;
    mInitialAngle.y = mAngleY;
    mInitialAngle.z = mAngleZ;
    mMeshCollider.SetStoodOnCallback(&func_ov002_020eff90);
}

// 0x020efbdc (0x98)
void dPathLiftActor_c::ResetPath()
{
    mPosX = mInitialPos.x;
    mPosY = mInitialPos.y;
    mPosZ = mInitialPos.z;
    mAngleX = mInitialAngle.x;
    mAngleY = mInitialAngle.y;
    mAngleZ = mInitialAngle.z;
    mPrevAngleX = mInitialAngle.x;
    mPrevAngleY = mInitialAngle.y;
    mPrevAngleZ = mInitialAngle.z;
    mCurrPathNode = 0;
    mHorzSpeed = mPathSpeed;
    if (Param08ModeIs1Or2()) SetState(0);
    else SetState(1);
}

// 0x020efc74 (0x80)
void dPathLiftActor_c::RenderPathModels()
{
    if (Param12ModeIs1() == 0) return;
    if (mState == 2) return;
    int i = 0;
    dExtSimpleModel_c *model = mModels;
    do {
        VecFx32 local = *(VecFx32 *)&data_ov002_0210af00;
        model->Render((Vector3 *)&local);
        i++;
        model++;
    } while (i < 3);
}

// 0x020efcf4 (0x174)
void dPathLiftActor_c::UpdatePathModels()
{
    int idx;
    int found;
    int i;
    int modelNode;
    Vector3 node;
    int one;

    if (Param12ModeIs1() == 0)
        return;

    found = 0;
    idx = mCurrPathNode;
    i = found;
    one = 1;

    for (; i < 3; i++) {
        node.x = 0;
        node.y = 0;
        node.z = 0;

        if (i == 0) {
            if (idx < (int)mPath.NumNodes() && idx >= 0) {
                Vector3 diff;
                mPath.GetNode(node, idx);
                Vec3_Sub(&diff, &node, &mPos);
                if (VEC_Mag(&diff) < 0xc8000) {
                    if (found == 0) {
                        found = one;
                        idx += 1;
                    }
                }
            }
        }

        modelNode = i * mPathDirection + idx;
        if (mPath.Loops()) {
            if (modelNode >= (int)mPath.NumNodes()) {
                modelNode -= mPath.NumNodes();
            }
            if (modelNode < 0) {
                modelNode += mPath.NumNodes();
            }
        } else if (modelNode >= (int)mPath.NumNodes()) {
            continue;
        }
        if (modelNode < (int)mPath.NumNodes() && modelNode >= 0) {
            mPath.GetNode(node, modelNode);
            mModels[i].mat4x3.t.x = node.x >> 3;
            mModels[i].mat4x3.t.y = node.y >> 3;
            mModels[i].mat4x3.t.z = node.z >> 3;
        }
    }
}

// 0x020efe68 (0x14)
int dPathLiftActor_c::HasNonzeroAngleZ() const {
    return mAngleZ != 0;
}

// 0x020efe7c (0x20)
int dPathLiftActor_c::Param10ModeIs1() const {
    return (unsigned char)((param1 >> 0xa) & 3) == 1;
}

// 0x020efe9c (0x20)
int dPathLiftActor_c::Param12ModeIs1() const {
    return (unsigned char)((param1 >> 0xc) & 3) == 1;
}

// 0x020efebc (0x20)
int dPathLiftActor_c::Param08ModeIs2() const {
    return (unsigned char)((param1 >> 8) & 3) == 2;
}

// 0x020efedc (0x28)
int dPathLiftActor_c::Param08ModeIs1Or2() const
{
    unsigned char v = (param1 >> 8) & 3;
    int r = 1;
    if (v == 1) return r;
    if (v != 2) r = 0;
    return r;
}

// 0x020eff04 (0x14)
/* The byte local is load-bearing: without it the TU's compiler-numbered
   objects come out one short of the @851..@861 the manifest renames. */
int dPathLiftActor_c::IsUnk42cSet() const {
    u8 respawn = mRespawnAfterFall;
    return respawn != 0;
}

// 0x020eff18 (0x78)
void dPathLiftActor_c::AfterClsn(int)
{
    if (Param08ModeIs1Or2() != 0 &&
        mState == 0 &&
        DecIfAbove0_Byte(&mTriggerDelay) == 0) {
        if (IsActorID(this, 0x1f))
            func_02012694(0x6f, &mCamSpacePos);
        SetState(1);
    }
    mAfterClsnRan = 1;
}

// 0x020eff90 (0x28)
/* The mesh collider's stoodOnCallback: the actor that landed on the lift
   arrives as AfterClsn's word argument. */
extern "C" void func_ov002_020eff90(dBgW *collider, dActor_c *lift, dActor_c *rider) {
    static_cast<dPathLiftActor_c *>(lift)->AfterClsn((int)rider);
}

/* The path-follow scale applied to the path models. Spelled as three s32s:
 * the repo's Vector3 declares an empty destructor, and a real Vector3 global
 * would make __sinit register it with __register_global_object, growing the
 * initializer past its retail 0xa4. */
s32 data_ov002_0210af00[3] = { 0x2000, 0x2000, 0x2000 };

/* The state names sit in 8-byte slots ahead of the pointer-to-member
 * descriptors. */
static char s_waitName[8] = "WAIT";
static char s_pathName[8] = "PATH";
static char s_fallName[8] = "FALL";

/* The state table SetState and BaseBehavior index by mState. Its six
 * pointer-to-member descriptors are anonymous compiler objects; this
 * definition is what makes mwcc emit __sinit_d_path_lift_actor.cpp. */
PathLiftState data_ov002_0210af2c[3] = {
    { &dPathLiftActor_c::StateWaitInit, &dPathLiftActor_c::StateWait, s_waitName },
    { &dPathLiftActor_c::StatePathInit, &dPathLiftActor_c::StatePath, s_pathName },
    { &dPathLiftActor_c::StateFallInit, &dPathLiftActor_c::StateFall, s_fallName },
};
