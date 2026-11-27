//cpp
/* daDgr_c -- Spindel (actor 163, DONGURU), ov025.
 *
 * Rolls along Z while turning about X. mRollStage counts 0..20 and the
 * step eases in and out; height is mBasePosY plus a sine bob. Two dust
 * systems (effect 0x2d) trail the ends of the body. The last rolling
 * frame of a stage shakes the ground, then it holds 32 frames and rolls
 * back the other way. A nearby Yoshi egg wakes the mesh collider.
 *
 * mwccarm emits one .text section per function, in reverse source order, so the factory is first.
 * D1 at 0x021111a0 and D0 at 0x021111e4 come from the inline destructor in
 * daDgr_c.h. An out-of-line body would emit D2, which this cartridge does
 * not have.
 *
 * Kept shapes (each alternative was measured against the ROM):
 * - Behavior: `mAngle.x = mAngle.x + mAngleXSpeed` size-DIFF 0x3ec->0x3e8;
 *   the ROM's update is `mAngle.x += mAngleXSpeed`.
 * - Behavior: `speedDivisor = -speedDivisor` and `bob = -bob` size-DIFF
 *   0x3ec->0x3e4. The ROM multiplies, `x *= -1`.
 * - Behavior: deleting the `dustPos = mPos` seed size-DIFF 0x3ec->0x3d4.
 *   dustPos and quakePos are two Vector3 locals, declared in that order
 *   (#821 r77: byte-identical to the old `s32 loc[6]`; the reverse order
 *   moves the frame slots).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDgr_c_AssetId { ASSET_DONGURU_BMD = 0x02da, ASSET_DONGURU_KCL = 0x02db };

#include "CLPS.h"
#include "common.h"
#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* 8-byte file handles. The model uses ModelFile::ModelFile / ModelFile::~ModelFile and the
 * collision file ClsnFile::ClsnFile / ClsnFile::~ClsnFile. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct DgrModelFilePtr : SharedFilePtr {
    DgrModelFilePtr(u32 fileID);
    ~DgrModelFilePtr();
};

struct DgrCollisionFilePtr : SharedFilePtr {
    DgrCollisionFilePtr(u32 fileID);
    ~DgrCollisionFilePtr();
};

enum {
    kYoshiEggActorID = 9,
    kHoldFrames = 32,
    kRollStages = 20,
    kGrindSound = 0x65,
    kDustEffect = 0x2d,
    kQuakeMagnitude = 0x3e8000,
    kDustReach = 100,
    kBobScale = 23,
    kDustDrop = 0xb9000,
    kRollZStep = 0x14000,
    kRollAngleStep = 0x400,
    kGrindAngleMask = 0x1fff,
    kGrindWindow = 0x320
};

extern DgrModelFilePtr data_ov025_02113a68;
extern DgrCollisionFilePtr data_ov025_02113a60;

extern CLPS_Block data_ov025_02112c28;

/* local extern: arm9 C helpers with no shared header yet. */
void func_02012694(unsigned int, const Vector3 *);

daDgr_c *daDgr_c_classInit() {
    return new daDgr_c();
}

s32 daDgr_c::InitResources() {
    UpdateModelMatrix();
    UpdateClsnMatrix();
    {
        BMD_File *bmd = dExtSimpleModel_c::LoadFile(data_ov025_02113a68);
        mModel.SetFile(bmd, 1, -1);
    }
    {
        KCL_File *kcl = dBgW_Kc::LoadFile(data_ov025_02113a60);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngle.y, data_ov025_02112c28);
    }
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mAngle.y = 0;
    mBasePosY = mPos.y;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mRollStage = 0;
    mRollDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}

s32 daDgr_c::Behavior() {
    Vector3 dustPos;
    Vector3 quakePos;
    s32 speedDivisor;
    s32 rollFrames;

    if (mRollStage == -1) {
        if (mPhaseTimer == kHoldFrames) {
            mRollStage = 0;
            mPhaseTimer = 0;
        } else {
            mPhaseTimer++;
            mSpeed.z = 0;
            mAngleXSpeed = 0;
            UpdateModelMatrix();
            if (IsClsnInRange(0, 0))
                UpdateClsnMatrix();
            return 1;
        }
    }

    speedDivisor = 10 - mRollStage;
    if (speedDivisor < 0)
        speedDivisor *= -1;
    speedDivisor = speedDivisor - 6;
    if (speedDivisor < 0)
        speedDivisor = 0;

    if (mPhaseTimer == speedDivisor + 8) {
        mPhaseTimer = 0;
        mRollStage++;
        if (mRollStage == kRollStages) {
            mRollDir ^= 1;
            mRollStage = -1;
        }
    }

    if ((u32)(speedDivisor - 3) <= 1u)
        speedDivisor = 4;
    else if ((u32)(speedDivisor - 1) <= 1u)
        speedDivisor = 2;
    else if (speedDivisor == 0)
        speedDivisor = 1;

    rollFrames = speedDivisor << 3;

    if (mPhaseTimer < rollFrames) {
        if (mRollDir == 0) {
            mSpeed.z = kRollZStep / speedDivisor;
            mAngleXSpeed = (s16)(kRollAngleStep / speedDivisor);
        } else {
            mSpeed.z = (-kRollZStep) / speedDivisor;
            mAngleXSpeed = (s16)((-kRollAngleStep) / speedDivisor);
        }

        mPos.z += mSpeed.z;
        mAngle.x += mAngleXSpeed;
        if ((mAngle.x & kGrindAngleMask) < kGrindWindow && mAngleXSpeed != 0)
            func_02012694(kGrindSound, &mCamSpacePos);

        {
            s32 bob = FX_SinIdx((u16)(s16)(mAngle.x << 2) >> 4) * kBobScale;
            if (bob < 0)
                bob *= -1;
            mPos.y = mBasePosY + bob;
        }

        if (mPhaseTimer == rollFrames - 1) {
            quakePos = mPos;
            Earthquake(quakePos, kQuakeMagnitude);
        }

        dustPos = mPos;
        {
            s32 along;
            s32 across;
            across = FX_CosIdx((u16)mAngle.y >> 4);
            dustPos.x = across * kDustReach + mPos.x;
            dustPos.y = mBasePosY - kDustDrop;
            along = FX_SinIdx((u16)mAngle.y >> 4);
            dustPos.z = along * kDustReach + mPos.z;
            mDustParticle1 = Particle::System::New(
                mDustParticle1, kDustEffect, dustPos.x, dustPos.y, dustPos.z,
                (const Vector3_16f *)((void *)0), (dPa_c::level_c::callback_c *)((void *)0));
        }
        {
            s32 along;
            s32 across;
            across = FX_CosIdx((u16)mAngle.y >> 4);
            dustPos.x = mPos.x - across * kDustReach;
            along = FX_SinIdx((u16)mAngle.y >> 4);
            dustPos.z = mPos.z - along * kDustReach;
            mDustParticle2 = Particle::System::New(
                mDustParticle2, kDustEffect, dustPos.x, dustPos.y, dustPos.z,
                (const Vector3_16f *)((void *)0), (dPa_c::level_c::callback_c *)((void *)0));
        }
    }

    UpdateModelMatrix();
    if (IsClsnInRange(0, 0) || EnableClsnNearEgg())
        UpdateClsnMatrix();

    mPhaseTimer++;
    return 1;
}

s32 daDgr_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daDgr_c::CleanupResources() {
    data_ov025_02113a68.Release();
    data_ov025_02113a60.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

/* dExtSimpleModel_c matrix: X rotation, then translation at 1/8 of the actor position. */
void daDgr_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationX(&mModel.mat4x3, mAngle.x);
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

/* Collision matrix: the model matrix, copied whole, then translation in full units. */
void daDgr_c::UpdateClsnMatrix() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.t.x = mPos.x;
    mClsnMat.t.y = mPos.y;
    mClsnMat.t.z = mPos.z;
    mMeshCollider.Transform(mClsnMat, mAngle.y);
}

/* Yoshi egg within eight clip-radii: turn the mesh collider on.
 * aimPos (the egg aim height above the actor) is computed and never read;
 * the ROM keeps its stores. */
int daDgr_c::EnableClsnNearEgg() {
    dActor_c *egg = ClosestWithActorID(kYoshiEggActorID);
    if (egg != 0) {
        Vector3 aimPos = mPos;
        aimPos.y += OnAimedAtWithEgg();
        if (Vec3_Dist(&mPos, &egg->mPos) < (mClipRadius << 3)) {
            if (!mMeshCollider.IsEnabled()) {
                mMeshCollider.Enable(this);
                return 1;
            }
        }
    }
    return 0;
}

/* Defined by `virtual ~daDgr_c() {}` in daDgr_c.h. */

/* Source order is construction order: model file 730, collision file 731.
 * __sinit_d_a_dgr.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
DgrModelFilePtr data_ov025_02113a68(ASSET_DONGURU_BMD);
DgrCollisionFilePtr data_ov025_02113a60(ASSET_DONGURU_KCL);
