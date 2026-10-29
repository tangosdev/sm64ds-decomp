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
 * - Behavior: `mAngleX = (s16)(mAngleX + mAngleXSpeed)` size-DIFF
 *   0x3ec->0x3e8. The halfword is updated through an s16*.
 * - Behavior: `speedDivisor = -speedDivisor` size-DIFF 0x3ec->0x3e4, and
 *   `bob = -bob` size-DIFF 0x3ec->0x3e4. Both keep
 *   `s32 neg = -1; x = x * neg`.
 * - Behavior: deleting the `loc[0..2] = mPos*` seed size-DIFF 0x3ec->0x3d4.
 *   Two Vec3 locals instead of `s32 loc[6]` size-DIFF 0x3ec->0x3ac
 *   (a Vector3 local emits ~Vector3). The six words stay.
 * - Behavior: IsClsnInRange with Fix12<int> parameters size-DIFF
 *   0x3ec->0x3f4. Earthquake's Fix12<int> magnitude size-DIFF
 *   0x3ec->0x3f8. Particle::System::New with Fix12<int> coordinates
 *   size-DIFF 0x3ec->0x41c. All three stay scalar externs.
 * - InitResources: dBgW_KcMbg::SetFile with a Fix12<int> temporary
 *   size-DIFF 0xbc->0xc8. The scalar extern stays.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDgr_c_AssetId { ASSET_DONGURU_BMD = 0x02da, ASSET_DONGURU_KCL = 0x02db };

#include "common.h"
#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/CalcLib.h"

/* POD position. Vector3's destructor is not in Behavior's bytes. */
typedef struct {
    s32 x, y, z;
} Vec3;

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
extern s16 data_02082214[];
extern CLPS_Block data_ov025_02112c28;

/* local extern: arm9 C helpers with no shared header yet. */
extern "C" {
void Matrix4x3_FromRotationX(void *mat, int angleX);
void func_02012694(unsigned int, const Vector3 *);
void func_020393d4(int *collider, int callback);
}

// @symbol daDgr_c_classInit
extern "C" daDgr_c *daDgr_c_classInit() {
    return new daDgr_c();
}

// @symbol _ZN7daDgr_c13InitResourcesEv
s32 daDgr_c::InitResources() {
    UpdateModelMatrix();
    UpdateClsnMatrix();
    {
        BMD_File *bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov025_02113a68);
        mModel.SetFile(bmd, 1, -1);
    }
    {
        KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov025_02113a60);
        ((dBgW_KcMbg *)(&mMeshCollider))
            ->dBgW_KcMbg::SetFile(kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosAndAngs);
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

// @symbol _ZN7daDgr_c8BehaviorEv
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
    if (speedDivisor < 0) {
        s32 neg = -1;
        speedDivisor = speedDivisor * neg;
    }
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

        {
            s16 *angleX = &mAngleX;
            mPosZ = mPosZ + mSpeedZ;
            *angleX = (s16)(*angleX + mAngleXSpeed);
            if ((mAngleX & kGrindAngleMask) < kGrindWindow) {
                if (mAngleXSpeed != 0)
                    func_02012694(kGrindSound, &mCamSpacePos);
            }
        }

        {
            s16 angleX = mAngleX;
            s32 idx = ((u16)(s16)(angleX << 2) >> 4) * 2;
            s32 bob = (s32)data_02082214[idx] * kBobScale;
            if (bob < 0) {
                s32 neg = -1;
                bob = bob * neg;
            }
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
            across = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = across * kDustReach + mPosX;
            loc[1] = mBasePosY - kDustDrop;
            along = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = along * kDustReach + mPosZ;
            mDustParticle1 = Particle::System::New(mDustParticle1, kDustEffect, loc[0], loc[1],
                                                   loc[2], (const Vector3_16f *)((void *)0),
                                                   (Particle::Callback *)((void *)0));
        }
        {
            s32 along;
            s32 across;
            across = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = mPosX - across * kDustReach;
            along = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = mPosZ - along * kDustReach;
            mDustParticle2 = Particle::System::New(mDustParticle2, kDustEffect, loc[0], loc[1],
                                                   loc[2], (const Vector3_16f *)((void *)0),
                                                   (Particle::Callback *)((void *)0));
        }
    }

    UpdateModelMatrix();
    if (IsClsnInRange(0, 0) || EnableClsnNearEgg())
        UpdateClsnMatrix();

    mPhaseTimer++;
    return 1;
}

// @symbol _ZN7daDgr_c6RenderEv
s32 daDgr_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN7daDgr_c16CleanupResourcesEv
s32 daDgr_c::CleanupResources() {
    data_ov025_02113a68.Release();
    data_ov025_02113a60.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

// @symbol _ZN7daDgr_c17UpdateModelMatrixEv
/* dExtSimpleModel_c matrix: X rotation, then translation at 1/8 of the actor position. */
void daDgr_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationX(&mModel.mat4x3, mAngleX);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN7daDgr_c16UpdateClsnMatrixEv
/* Collision matrix. Flat Matrix4x3, copied whole, then translation in full units. */
void daDgr_c::UpdateClsnMatrix() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

// @symbol _ZN7daDgr_c17EnableClsnNearEggEv
/* Yoshi egg within eight clip-radii: turn the mesh collider on.
 * aimPos is volatile and unused. Its stores, and ~Vector3, are in the bytes. */
int daDgr_c::EnableClsnNearEgg() {
    dActor_c *egg = ClosestWithActorID(kYoshiEggActorID);
    if (egg != 0) {
        volatile struct Vector3 aimPos;
        aimPos.x = mPosX;
        aimPos.y = mPosY;
        aimPos.z = mPosZ;
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

// @symbol _ZN7daDgr_cD1Ev
// @symbol _ZN7daDgr_cD0Ev
/* Defined by `virtual ~daDgr_c() {}` in daDgr_c.h. */

/* Source order is construction order: model file 730, collision file 731.
 * __sinit_d_a_dgr.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
DgrModelFilePtr data_ov025_02113a68(ASSET_DONGURU_BMD);
DgrCollisionFilePtr data_ov025_02113a60(ASSET_DONGURU_KCL);
