//cpp
/**
 * dPathLiftActor_c -- the path-following lift base.
 *
 * Production consolidation: the two destructors (D0 0x020ef320, D1 0x020ef390),
 * 19 class methods and the existing C bridge occupy ov002:0x020ef320..0x020effb8.
 * This contiguous range is packaging evidence, not proof of the original TU.
 *
 * The state table at 0x0210af2c is the file-scope PathLiftState array defined
 * at the bottom of this file; mwcc emits __sinit_dPathLiftActor_c.cpp to copy
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
 * - Several helpers keep their mangled extern "C" spellings, and a few of those
 *   declarations are vaguer than the definitions they name (banked in
 *   config/decl-agreement-baseline.json). Tightening one changes its call sites,
 *   which is matching work needing its own byte proof.
 * - StateFall keeps its this-relative casts for mFallAngle (0x44a), mFallStartY
 *   (0x444) and mPosY (0x60): naming the members makes mwcc reuse the signed
 *   load and drops 12 bytes.
 */

#pragma defer_codegen off

// Header order preserves the measured mwccarm 2004/b56 output.
#include "PathLift.h"
#include "types.h"
#include "decl_PathPtr.h"
#include "decl_common.h"
#include "common.h"

bool ApproachLinear(short &value, short target, short step);

typedef void (dPathLiftActor_c::*PathLiftStateFn)();

struct PathLiftState {
    PathLiftStateFn init;
    PathLiftStateFn behavior;
    const char *name;
};

/* POD spelling of Vector3's 0x0c bytes: with a user-declared constructor on
   Vector3, copy-init lowers memberwise; the ROM copy is the block move, so the
   per-frame local in RenderPathModels goes through this. */
struct RawVector3 {
    int x, y, z;
};

extern "C" {
extern s16 data_02082214[];
extern u16 DecIfAbove0_Short(u16 *p);
extern u8 DecIfAbove0_Byte(u8 *p);
extern int Vec3_HorzDist(const void* a, const void* b);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern short Vec3_HorzAngle(void *a, void *b);
extern short Vec3_VertAngle(void *a, void *b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void SubVec3(void *a, void *b, void *c);
void func_02012694(int a, void *p);

/* Defined last: the ROM-ascending order puts the collider callback after
   BaseInitResources, which stores its address. */
void func_ov002_020eff90(int unused, dPathLiftActor_c *lift, int x);
}

extern PathLiftState data_ov002_0210af2c[];

// @symbol _ZN16dPathLiftActor_cD0Ev
// @symbol _ZN16dPathLiftActor_cD1Ev
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

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" dPathLiftActor_c *_ZN16dPathLiftActor_cD0Ev(dPathLiftActor_c *thiz)
{
    thiz->dPathLiftActor_c::~dPathLiftActor_c();          /* direct member/base teardown */
    dPathLiftActor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
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
// @symbol _ZN16dPathLiftActor_c9StateWaitEv
void dPathLiftActor_c::StateWait()
{
}

// 0x020ef3f0 (0x18)
// @symbol _ZN16dPathLiftActor_c13StateWaitInitEv
void dPathLiftActor_c::StateWaitInit()
{
    mTriggerDelay = 24;
    mWaitTimer = 300;
}

// 0x020ef408 (0x174)
// @symbol _ZN16dPathLiftActor_c9StateFallEv
void dPathLiftActor_c::StateFall()
{
    char *c = (char *)this;
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
        raw = *(s16 *)(c + 0x44a);
        v = data_02082214[((u16)(int)(long long)raw >> 4) * 2];
        *(s32 *)(c + 0x60) = *(s32 *)(c + 0x444) +
            (s32)(u32)((((long long)v << 15) + 0x800) >> 12);
        *(u16 *)((int)(c + 0x44a)) =
            (u16)(*(u16 *)((int)(c + 0x44a)) + 0x3000);
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
// @symbol _ZN16dPathLiftActor_c13StateFallInitEv
void dPathLiftActor_c::StateFallInit()
{
  int b = (int)(actorID == 0x82);
  if (b != 0) {
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
  {
    int b2 = (int)(actorID == 0x82);
    if (b2 == 0) {
      return;
    }
    mHorzSpeed = Vec3_HorzDist(&mPosX, &mPrevPosX);
  }
}

// 0x020ef670 (0x3d4)
// @symbol _ZN16dPathLiftActor_c9StatePathEv
void dPathLiftActor_c::StatePath()
{
    struct Vector3 destNode, srcNode, fromDest, segment, stepA, stepB;
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

    Vec3_Sub(&fromDest, &mPosX, &destNode);
    destDist = LenVec3(&fromDest);
    Vec3_Sub(&segment, &srcNode, &destNode);
    segLen = LenVec3(&segment);
    segFrames = _ZN4cstd4fdivEii(segLen, mHorzSpeed) / 0x1000;

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
        Vec3_MulScalar(&stepA, &fromDest, _ZN4cstd4fdivEii(mHorzSpeed, destDist));
        SubVec3(&mPosX, &stepA, &mPosX);
        arrived = 1;
    } else {
        Vec3_MulScalar(&stepB, &fromDest, _ZN4cstd4fdivEii(mHorzSpeed, destDist));
        SubVec3(&mPosX, &stepB, &mPosX);
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
// @symbol _ZN16dPathLiftActor_c13StatePathInitEv
void dPathLiftActor_c::StatePathInit()
{
    mWaitTimer = 300;
}

// 0x020efa54 (0x4c)
// @symbol _ZN16dPathLiftActor_c8SetStateEi
void dPathLiftActor_c::SetState(int state) {
    mState = state;
    int next = mState;
    (this->*data_ov002_0210af2c[next].init)();
}

// 0x020efaa0 (0x50)
// @symbol _ZN16dPathLiftActor_c12BaseBehaviorEv
void dPathLiftActor_c::BaseBehavior()
{
    PathLiftState &state = data_ov002_0210af2c[mState];
    (this->*state.behavior)();
    mAfterClsnRan = 0;
}

// 0x020efaf0 (0xec)
// @symbol _ZN16dPathLiftActor_c17BaseInitResourcesEv
struct BMD_File;
struct PathStuff { void* a; void* file; };  // data_0210d9f0: load [4]

extern PathStuff data_ov002_0210d9f0;
extern "C" {
void func_020393c4(char* p, int v);
}

void dPathLiftActor_c::BaseInitResources()
{
    for (int i = 0; i < 3; i++)
        mModels[i].SetFile((BMD_File *)data_ov002_0210d9f0.file, 1, -1);

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
    func_020393c4((char *)&mMeshCollider, (int)&func_ov002_020eff90);
}

// 0x020efbdc (0x98)
// @symbol _ZN16dPathLiftActor_c9ResetPathEv
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
// @symbol _ZN16dPathLiftActor_c16RenderPathModelsEv
void dPathLiftActor_c::RenderPathModels()
{
    if (Param12ModeIs1() == 0) return;
    if (mState == 2) return;
    int i = 0;
    Model* model = mModels;
    do {
        RawVector3 local = *(RawVector3 *)&data_ov002_0210af00;
        model->Render((Vector3 *)&local);
        i++;
        model++;
    } while (i < 3);
}

// 0x020efcf4 (0x174)
// @symbol _ZN16dPathLiftActor_c16UpdatePathModelsEv
void dPathLiftActor_c::UpdatePathModels()
{
    int idx;
    int found;
    int i;
    int modelNode;
    struct Vector3 node;
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
                struct Vector3 diff;
                mPath.GetNode(node, idx);
                Vec3_Sub(&diff, &node, (struct Vector3 *)&mPosX);
                if (LenVec3(&diff) < 0xc8000) {
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
// @symbol _ZNK16dPathLiftActor_c16HasNonzeroAngleZEv
int dPathLiftActor_c::HasNonzeroAngleZ() const {
  return mAngleZ != 0;
}

// 0x020efe7c (0x20)
// @symbol _ZNK16dPathLiftActor_c14Param10ModeIs1Ev
int dPathLiftActor_c::Param10ModeIs1() const {
    return (unsigned char)((param1 >> 0xa) & 3) == 1;
}

// 0x020efe9c (0x20)
// @symbol _ZNK16dPathLiftActor_c14Param12ModeIs1Ev
int dPathLiftActor_c::Param12ModeIs1() const {
    return (unsigned char)((param1 >> 0xc) & 3) == 1;
}

// 0x020efebc (0x20)
// @symbol _ZNK16dPathLiftActor_c14Param08ModeIs2Ev
int dPathLiftActor_c::Param08ModeIs2() const {
    return (unsigned char)((param1 >> 8) & 3) == 2;
}

// 0x020efedc (0x28)
// @symbol _ZNK16dPathLiftActor_c17Param08ModeIs1Or2Ev
int dPathLiftActor_c::Param08ModeIs1Or2() const
{
    unsigned char v = (param1 >> 8) & 3;
    int r = 1;
    if (v == 1) return r;
    if (v != 2) r = 0;
    return r;
}

// 0x020eff04 (0x14)
// @symbol _ZNK16dPathLiftActor_c11IsUnk42cSetEv
int dPathLiftActor_c::IsUnk42cSet() const {
  unsigned char v = unk_42c;
  if (v != 0x0) return 1;
  return 0;
}

// 0x020eff18 (0x78)
// @symbol _ZN16dPathLiftActor_c9AfterClsnEi
void dPathLiftActor_c::AfterClsn(int)
{
    if (Param08ModeIs1Or2() != 0 &&
        mState == 0 &&
        DecIfAbove0_Byte(&mTriggerDelay) == 0) {
        int b = actorID == 0x1f;
        if (b) {
            func_02012694(0x6f, &mCamSpacePosX);
        }
        SetState(1);
    }
    mAfterClsnRan = 1;
}

// 0x020eff90 (0x28)
// @symbol func_ov002_020eff90
extern "C" void func_ov002_020eff90(int unused, dPathLiftActor_c* lift, int x) {
  lift->AfterClsn(x);
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
 * definition is what makes mwcc emit __sinit_dPathLiftActor_c.cpp. */
PathLiftState data_ov002_0210af2c[3] = {
    { &dPathLiftActor_c::StateWaitInit, &dPathLiftActor_c::StateWait, s_waitName },
    { &dPathLiftActor_c::StatePathInit, &dPathLiftActor_c::StatePath, s_pathName },
    { &dPathLiftActor_c::StateFallInit, &dPathLiftActor_c::StateFall, s_fallName },
};
