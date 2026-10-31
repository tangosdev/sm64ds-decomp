//cpp
/**
 * The path lift (profiles KM2_SUSUMU and KM3_LIFT).
 *
 * Rides the level path named by param1's low nibble, node to node, at
 * 10.0 a frame. It waits at the first node until something the mesh
 * callback recognises stands on it for 20 frames, runs to the end of the
 * path, turns round, and runs back. Resource set 0 also sinks up to 30.0
 * while it is loaded and pauses 20 frames before each return leg; the
 * other set loops end to end without stopping.
 *
 * daLinelift2_c_classInit_KM2_SUSUMU / _KM3_LIFT are reconstructed
 * (RTTI daLinelift2_c, KM2_SUSUMU / KM3_LIFT registry profiles). Retail
 * does not store those spellings. Historical aliases:
 * SquareMetalNetLift_Spawn, ArrowPathLift_Spawn.
 *
 * _ZTS13daLinelift2_c 0x02134e70. _ZTI13daLinelift2_c 0x02134e64.
 * _ZTV13daLinelift2_c 0x02134ec0. InitResources is the key function, first
 * non-inline virtual: D1 at 0x02131c14, D0 at 0x02131c58.
 *
 * Functions run in reverse ROM order; do not reorder.
 */

// Notes:
// - func_020393d4 / func_020393c4 install dBgW's two callback words;
//   dBgW carries no setter for them.
// - &mTargetPosX / &mSpeedX: this class and dActor_c store those triples
//   as scalars, so the vector helpers take the first word's address.
// - Vec3_Sub is defined on a file-local struct Vec3, so no shared header
//   declares it yet; it is declared here. AddVec3 takes Vector3 now.
// - Behavior lowers mPosY by the sink offset and restores it before the
//   model update, which the cartridge does too; the dead store is its own.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_linelift2_AssetId {
    ASSET_KM2_SUSUMU_BMD = 0x0669,
    ASSET_KM2_SUSUMU_KCL = 0x066a,
    ASSET_KM3_RIFT_BMD = 0x0683,
    ASSET_KM3_RIFT_KCL = 0x0684
};

#include "daLinelift2_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "cstd.h"

struct Vec3;

typedef void (daLinelift2_c::*StateFunc)();

/* 8-byte file handles. The models use func_02017acc / func_02017ab4 and the
 * collision files func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Linelift2ModelFilePtr : SharedFilePtr {
    Linelift2ModelFilePtr(u32 fileID);
    ~Linelift2ModelFilePtr();
};

struct Linelift2CollisionFilePtr : SharedFilePtr {
    Linelift2CollisionFilePtr(u32 fileID);
    ~Linelift2CollisionFilePtr();
};

extern SharedFilePtr *gLinelift2ModelFiles[];     /* BMD, by mVariant */
extern SharedFilePtr *gLinelift2CollisionFiles[]; /* KCL, by mVariant */
extern CLPS_Block *gLinelift2ClpsBlocks[];        /* CLPS, by mVariant */
extern StateFunc gLinelift2StateFuncs[];          /* by mState */
extern s16 data_02082214[];                       /* sin/cos pairs */

/* arm9 C helpers with no shared header. */
extern "C" {
void Vec3_Sub(Vector3 *res, const Vector3 *a, const Vector3 *b);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *out);
void func_02010da4(dActor_c *actor);
void func_020393d4(int *collider, int callback);
void func_020393c4(int *collider, int callback);
}

enum {
    kPlayerActorID = 0xbf,
    kWaitFrames = 20,
    kRideSpeed = 0xa000, /* 10.0 */
};

/* How far the platform gives under a load, and how fast it gets there. */
static const int cSinkDepth = 0x1e000;
static const int cSinkRate = 0x5000;

// @symbol daLinelift2_c_classInit_KM3_LIFT
extern "C" daLinelift2_c *daLinelift2_c_classInit_KM3_LIFT() {
    return new daLinelift2_c();
}

// @symbol daLinelift2_c_classInit_KM2_SUSUMU
extern "C" daLinelift2_c *daLinelift2_c_classInit_KM2_SUSUMU() {
    return new daLinelift2_c();
}

// @symbol _ZN13daLinelift2_c15StoodOnCallbackEP4dBgWP8dActor_cS3_
/* The collider's stood-on callback word: drop the collider, forward the
   rider to the lift. long_calls emits the pooled `ldr ip,[pc,#8]; bx ip`
   absolute tail-call; bracketed closed immediately because it is
   positional. */
#pragma long_calls on
void daLinelift2_c::StoodOnCallback(dBgW *collider, dActor_c *lift, dActor_c *rider) {
    ((daLinelift2_c *)lift)->OnStoodOn(rider);
}
#pragma long_calls off

// @symbol _ZN13daLinelift2_c9OnStoodOnEP8dActor_c
void daLinelift2_c::OnStoodOn(dActor_c *rider) {
    unsigned char isPlayer = rider->actorID == kPlayerActorID;
    if (isPlayer)
        mIsPressed = 1;
}

// @symbol _ZN13daLinelift2_c13InitResourcesEv
int daLinelift2_c::InitResources() {
    mVariant = param1 >> 8;
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*gLinelift2ModelFiles[mVariant]), 1, -1);

    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*gLinelift2CollisionFiles[mVariant]),
                          mClsnMat, 0x199, mAngleY, *gLinelift2ClpsBlocks[mVariant]);

    UpdateClsnPosAndRot();
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    func_020393c4((int *)&mMeshCollider, (int)&StoodOnCallback);

    mPathPtr.FromID(param1 & 0xf);
    mNodeCount = mPathPtr.NumNodes();
    mNodeIndex = 0;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, mNodeIndex);

    /* The first node is usually where we are standing; head for the next one. */
    if (Vec3_Equal((Vector3 *)&mTargetPosX, (Vector3 *)&mBasePosX)) {
        mNodeIndex++;
        mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, mNodeIndex);
    }

    mBaseAngleY = mAngleY;
    mSinkOffsetY = 0;
    mIsPressed = 0;
    return 1;
}

// @symbol _ZN13daLinelift2_c8BehaviorEv
int daLinelift2_c::Behavior() {
    int old = mState;
    (this->*gLinelift2StateFuncs[old])();
    mStateTimer += 1;
    /* Restart the clock on a state change, and stop feeding the collider a
       velocity while the state is switching over. */
    if (old != mState) {
        mStateTimer = 0;
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
    if (mVariant == 0) {
        int saved = mPosY;
        ApproachLinear(mSinkOffsetY, mIsPressed ? cSinkDepth : 0, cSinkRate);
        mPosY -= mSinkOffsetY;
        mPosY = saved;
    }
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    mIsPressed = 0;
    return 1;
}

// @symbol _ZN13daLinelift2_c6RenderEv
int daLinelift2_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daLinelift2_c16CleanupResourcesEv
int daLinelift2_c::CleanupResources() {
    gLinelift2ModelFiles[mVariant]->Release();
    gLinelift2CollisionFiles[mVariant]->Release();
    mMeshCollider.Disable();
    return 1;
}

// @symbol _ZN13daLinelift2_c9StateWaitEv
/* State 0: wait at the first node until something has stood on the lift
   for 20 frames, then set off along the path at 10.0 a frame. */
void daLinelift2_c::StateWait() {
    if (mIsPressed != 0) {
        if (mStateTimer <= kWaitFrames)
            return;
        mState = 1;
        mHorzSpeed = kRideSpeed;
        Vector3 target, base;
        target.x = mTargetPosX;
        target.y = mTargetPosY;
        target.z = mTargetPosZ;
        base.x = mBasePosX;
        base.y = mBasePosY;
        base.z = mBasePosZ;
        SetVelocityToward(&target, &base);
        return;
    }
    mStateTimer = 0;
}

// @symbol _ZN13daLinelift2_c12StateForwardEv
/* State 1: run forward; at the path's end, turn round (state 2). */
void daLinelift2_c::StateForward() {
    if (StepAlongPath() == -1) {
        mState = 2;
    }
    s16 old = mAngleY;
    mAngleY = mBaseAngleY;
    if (old != mAngleY) {
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
}

// @symbol _ZN13daLinelift2_c11StateReturnEv
/* State 2: run back facing the other way. Resource set 0 pauses 20 frames
   first and settles in state 0 at the start; the other set loops. */
void daLinelift2_c::StateReturn() {
    s16 old = mAngleY;
    mAngleY = mBaseAngleY + 0x8000;
    if (old != mAngleY) {
        func_020393d4((int *)&mMeshCollider, 0);
    } else {
        func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    }
    if (mVariant == 0 && mStateTimer < kWaitFrames)
        return;
    if (StepAlongPath() != -1)
        return;
    if (mVariant != 0) {
        mState = 1;
        return;
    }
    mState = 0;
    mAngleY = mBaseAngleY;
}

// @symbol _ZN13daLinelift2_c13StepAlongPathEv
/* One step along the path. Moves by the velocity; within half a step of the
   target node, snaps onto it, picks the next node in the direction of travel
   (mState 1 forward, otherwise back) and re-aims. Returns -1 when that runs
   off either end of the path, 1 on reaching a node, 0 in between. */
int daLinelift2_c::StepAlongPath() {
    int result;
    AddVec3((Vector3 *)&mPosX, (Vector3 *)&mSpeedX, (Vector3 *)&mPosX);
    if (Vec3_Dist(&mPos, (const Vector3 *)&mTargetPosX) < (mHorzSpeed >> 1)) {
        result = 1;
        mBasePosX = mTargetPosX;
        mBasePosY = mTargetPosY;
        mBasePosZ = mTargetPosZ;
        mPosX = mBasePosX;
        mPosY = mBasePosY;
        mPosZ = mBasePosZ;
        if (mState == 1) {
            mNodeIndex++;
            if (mNodeIndex >= mNodeCount) {
                mNodeIndex = mNodeCount - 2;
                result = -1;
            }
        } else {
            mNodeIndex--;
            if (mNodeIndex < 0) {
                mNodeIndex = result;
                result = -1;
            }
        }
        mPathPtr.GetNode(*(Vector3 *)&mTargetPosX, (unsigned int)mNodeIndex);
        {
            Vector3 target, base;
            target.x = mTargetPosX;
            target.y = mTargetPosY;
            target.z = mTargetPosZ;
            base.x = mBasePosX;
            base.y = mBasePosY;
            base.z = mBasePosZ;
            SetVelocityToward(&target, &base);
        }
        return result;
    }
    func_02010da4(this);
    return 0;
}

// @symbol _ZN13daLinelift2_c17SetVelocityTowardEPK7Vector3S2_
/* Aim the velocity from `from` toward `to` at mHorzSpeed: pitch into
   mPrevAngleX, heading into mPrevAngleY, components into mSpeedX /
   mVertSpeed / mSpeedZ. */
void daLinelift2_c::SetVelocityToward(const Vector3 *to, const Vector3 *from) {
    Vector3 d;
    Vec3_Sub(&d, to, from);
    mPrevAngleY = cstd::atan2(d.x, d.z);
    mPrevAngleX = cstd::atan2(d.y, Vec3_HorzLen(&d));
    s32 horz = FX_MUL(mHorzSpeed, data_02082214[((u16)mPrevAngleX >> 4) * 2 + 1]);
    mSpeedX = FX_MUL(horz, data_02082214[((u16)mPrevAngleY >> 4) * 2]);
    mVertSpeed = FX_MUL(mHorzSpeed, data_02082214[((u16)mPrevAngleX >> 4) * 2]);
    mSpeedZ = FX_MUL(horz, data_02082214[((u16)mPrevAngleY >> 4) * 2 + 1]);
}

// @symbol _ZN13daLinelift2_cD1Ev
// @symbol _ZN13daLinelift2_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* Source order is construction order: the two model files, the two
 * collision files, then the state table. __sinit_d_a_linelift2.cpp emits
 * the constructions, registers the destructors -- the registration nodes
 * are compiler temporaries -- and copies the three pointer-to-member
 * constants (compiler temporaries in .data) into the table. */
// @symbol __sinit_d_a_linelift2.cpp
Linelift2ModelFilePtr gKm3RiftModelFile(ASSET_KM3_RIFT_BMD);
Linelift2ModelFilePtr gKm2SusumuModelFile(ASSET_KM2_SUSUMU_BMD);
Linelift2CollisionFilePtr gKm3RiftCollisionFile(ASSET_KM3_RIFT_KCL);
Linelift2CollisionFilePtr gKm2SusumuCollisionFile(ASSET_KM2_SUSUMU_KCL);

StateFunc gLinelift2StateFuncs[3] = {
    &daLinelift2_c::StateWait,
    &daLinelift2_c::StateForward,
    &daLinelift2_c::StateReturn,
};
