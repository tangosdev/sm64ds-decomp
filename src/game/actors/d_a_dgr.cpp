//cpp
/* daDgr_c -- Spindel (actor 163, DONGURU), ov025.
 *
 * Rolls along Z while turning about X. mRollStage counts 0..20 and the
 * step eases in and out; height is mBasePosY plus a sine bob. Two dust
 * systems (effect 0x2d) trail the ends of the body. The last rolling
 * frame of a stage shakes the ground, then it holds 32 frames and rolls
 * back the other way. A nearby Yoshi egg wakes the mesh collider.
 *
 * common.h is included before daDgr_c.h. Matrix4x3 has two 0x30-byte
 * spellings and whichever a TU sees first wins; the nested one scalarizes
 * the twelve-word copy in UpdateClsnMatrix. mwccarm emits one .text
 * section per function, in reverse source order, so the factory is first.
 * D1 at 0x021111a0 and D0 at 0x021111e4 come from the inline destructor in
 * daDgr_c.h. An out-of-line body would emit D2, which this cartridge does
 * not have.
 *
 * Kept shapes (each alternative was measured against the ROM):
 * - Behavior: `mAngleX = mAngleX + mAngleXSpeed` size-DIFF 0x3ec->0x3e8;
 *   the ROM's update is `mAngleX += mAngleXSpeed`.
 * - Behavior: `speedDivisor = -speedDivisor` and `bob = -bob` size-DIFF
 *   0x3ec->0x3e4. The ROM multiplies, `x *= -1`.
 * - Behavior: deleting the `loc[0..2] = mPos*` seed size-DIFF 0x3ec->0x3d4.
 *   Two Vec3 locals instead of `s32 loc[6]` size-DIFF 0x3ec->0x3ac
 *   (a Vector3 local emits ~Vector3). The six words stay.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDgr_c_AssetId { ASSET_DONGURU_BMD = 0x02da, ASSET_DONGURU_KCL = 0x02db };

#include "common.h"
#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
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
    kHoldFrames = 0x20,
    kRollStages = 0x14,
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
/* CLPS.h keeps the block opaque: header with the entry stride at +4, then
   the entries. This TU hands SetFile a one-entry block. */
struct CLPS_Block {
    u32 words[8];
};

/* CLPS block SetFile takes: CLPS header, one entry, four all-ones words. The
   cartridge also labels the last of them 0x02112c44 -- it is this block's
   eighth word, not a separate object. */
CLPS_Block data_ov025_02112c28 = {
    0x53504c43, 0x00010008, 0x01000fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

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
        mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mAngleY = 0;
    mBasePosY = mPosY;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mRollStage = 0;
    mRollDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}

s32 daDgr_c::Behavior() {
    s32 loc[6];
    s32 speedDivisor;
    s32 rollFrames;

    if (mRollStage == -1) {
        if (mPhaseTimer == kHoldFrames) {
            mRollStage = 0;
            mPhaseTimer = 0;
        } else {
            mPhaseTimer++;
            mSpeedZ = 0;
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
            mSpeedZ = kRollZStep / speedDivisor;
            mAngleXSpeed = (s16)(kRollAngleStep / speedDivisor);
        } else {
            mSpeedZ = (-kRollZStep) / speedDivisor;
            mAngleXSpeed = (s16)((-kRollAngleStep) / speedDivisor);
        }

        mPosZ += mSpeedZ;
        mAngleX += mAngleXSpeed;
        if ((mAngleX & kGrindAngleMask) < kGrindWindow && mAngleXSpeed != 0)
            func_02012694(kGrindSound, &mCamSpacePos);

        {
            s32 bob = FX_SinIdx((u16)(s16)(mAngleX << 2) >> 4) * kBobScale;
            if (bob < 0)
                bob *= -1;
            mPosY = mBasePosY + bob;
        }

        if (mPhaseTimer == rollFrames - 1) {
            loc[3] = mPosX;
            loc[4] = mPosY;
            loc[5] = mPosZ;
            Earthquake(*(const Vector3 *)&loc[3], kQuakeMagnitude);
        }

        loc[0] = mPosX;
        loc[1] = mPosY;
        loc[2] = mPosZ;
        {
            s32 along;
            s32 across;
            across = FX_CosIdx((u16)mAngleY >> 4);
            loc[0] = across * kDustReach + mPosX;
            loc[1] = mBasePosY - kDustDrop;
            along = FX_SinIdx((u16)mAngleY >> 4);
            loc[2] = along * kDustReach + mPosZ;
            mDustParticle1 = Particle::System::New(mDustParticle1, kDustEffect, loc[0], loc[1],
                                                   loc[2], (const Vector3_16f *)((void *)0),
                                                   (dPa_c::level_c::callback_c *)((void *)0));
        }
        {
            s32 along;
            s32 across;
            across = FX_CosIdx((u16)mAngleY >> 4);
            loc[0] = mPosX - across * kDustReach;
            along = FX_SinIdx((u16)mAngleY >> 4);
            loc[2] = mPosZ - along * kDustReach;
            mDustParticle2 = Particle::System::New(mDustParticle2, kDustEffect, loc[0], loc[1],
                                                   loc[2], (const Vector3_16f *)((void *)0),
                                                   (dPa_c::level_c::callback_c *)((void *)0));
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
    Matrix4x3_FromRotationX(&mModel.mat4x3, mAngleX);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Collision matrix. Flat Matrix4x3, copied whole, then translation in full units. */
void daDgr_c::UpdateClsnMatrix() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
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
