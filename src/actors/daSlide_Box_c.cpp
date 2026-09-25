//cpp
/**
 * Production translation unit for ov016/daSlide_Box_c.
 * 7 function(s), .text 0x02112ff8..0x02113510. Jolly Roger Bay's sliding
 * crate (SLIDE_BOX).
 *
 * NAME: _ZTS13daSlide_Box_c is "13daSlide_Box_c" at ov016 0x02114c58; _ZTI at
 * 0x02114c4c reads [__si_class_type_info, that string, _ZTI10dBgActor_c]. The
 * tree previously called the class SlidingBox (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02112ff8), D0
 * (0x02113044), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * InitResources loads the model and the mesh collider, then waits. Behavior
 * looks up actor 0x39 (KI_FUNE_UP, the upward ship). Until the crate is on
 * the deck it falls with the actor's gravity. Once grounded it copies the
 * ship's angles, slides along the pitch (sine of mAngleX, clamped), and
 * places itself out from mBasePos along the ship's yaw. The deck normal
 * sets mVertSpeed so the next UpdatePos stays on the tilt. Near a player,
 * and moving, it keeps a rolling sound alive.
 *
 * The factory daSlide_Box_c_classInit is the next TU (0x02113510).
 */

#include "decl_common.h"
#include "daSlide_Box_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "Sound.h"
#include "dBgPi.h"

namespace cstd { int fdiv(int a, int b); }

extern "C" {
extern short data_02082214[];
void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
/* dBgActor_c::IsClsnInRange takes Fix12<int> by value, so it stays mangled. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
/* dBgCh_Actr::Init takes Fix12<int> by value (the ROM name mangles 5Fix12IiE).
 * dBgCh_Actr.h declares those parameters as Fix12i, a plain s32, so a member
 * call would mangle ii and link to a symbol the ROM does not have: it stays
 * mangled. */
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *thiz, void *act, int radius, int height, void *rotA, int rotB);
}

/* Radius and height passed to dBgCh_Actr::Init, 20.0 in 20.12. */
enum { kClsnRadius = 0x14000 };

/* Pitch-to-speed scale, and the travel clamp along the ship. */
enum {
    kSlideScale = 0x8c,
    kHorzPosMax = 0x4ff000,
    kHorzPosMin = -0x32000,
    kFixRound = 0x800
};

/* How close a player has to be before the rolling sound starts, and
 * how fast the crate has to be moving. */
enum {
    kSoundRange = 0x7d0000,
    kSoundSpeed = 0x3000,
    kRollSound = 0x9f
};

/* The upward ship. KI_FUNE_UP's profile word at ov016 0x02114a1c is 0x00b50039. */
enum { kShipActorId = 0x39 };

#pragma defer_codegen off

// @symbol _ZN13daSlide_Box_cD1Ev
// @symbol _ZN13daSlide_Box_cD0Ev
/* Empty on purpose. mwccarm destroys mWithMeshClsn, then the inlined
 * dBgActor_c teardown, and emits retail D1 followed by D0. */
daSlide_Box_c::~daSlide_Box_c()
{
}

// @symbol _ZN13daSlide_Box_c11UpdateModelEv
void daSlide_Box_c::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt((void *)&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN13daSlide_Box_c16CleanupResourcesEv
int daSlide_Box_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)data_ov016_02114e74)->Release();
    ((SharedFilePtr *)data_ov016_02114e6c)->Release();
    return 1;
}

// @symbol _ZN13daSlide_Box_c6RenderEv
int daSlide_Box_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daSlide_Box_c8BehaviorEv
int daSlide_Box_c::Behavior()
{
    Vector3 normal;

    switch (mState) {
    case 0:
        mShip = dActor_c::FindWithActorID(kShipActorId, 0);
        if (mShip == 0) {
            MarkForDestruction();
            break;
        }
        mState++;
        /* fallthrough */
    case 1:
        UpdatePos(0);
        /* Behavior: UpdateContinuous() relocates to the real method.
         * The ROM calls the veneer at arm9 0x020383fc. */
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            mState++;
            mBasePos.x = mPosX;
            mBasePos.y = mPosY;
            mBasePos.z = mPosZ;
        }
        break;
    case 2: {
        /* Behavior: mShip->mAngleX/Y/Z each reloads mShip and the three
         * halfword loads DIFF. One pointer, then [0]/[1]/[2], matches. */
        s16 *shipAngles = &mShip->mAngleX;
        int spd;
        mAngleX = shipAngles[0];
        mAngleY = shipAngles[1];
        mAngleZ = shipAngles[2];
        mPrevAngleY = mAngleY;
        mHorzSpeed = data_02082214[((u16)mAngleX >> 4) * 2] * kSlideScale;
        mHorzPos += mHorzSpeed;
        spd = mHorzPos;
        if (spd >= kHorzPosMax)
            mHorzPos = kHorzPosMax;
        else if (spd < kHorzPosMin)
            mHorzPos = kHorzPosMin;
        mPosX = mBasePos.x + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2] + kFixRound) >> 12);
        mPosZ = mBasePos.z + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2 + 1] + kFixRound) >> 12);
        mHorzSpeed = 0;
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            dBgPi *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn);
            floor->surface.CopyNormalTo(normal);
            if (normal.y != 0) {
                mVertSpeed = -(cstd::fdiv(
                    (int)(((long long)normal.x * unk_0a4 + kFixRound) >> 12)
                  + (int)(((long long)normal.z * unk_0ac + kFixRound) >> 12),
                    normal.y) + 0x8000);
            }
        }
        if (DistToCPlayer() < kSoundRange) {
            int vel = mHorzSpeed;
            if (vel < 0)
                vel = -vel;
            if (vel > kSoundSpeed) {
                mSoundID = Sound::PlayLong(
                    mSoundID, 3, kRollSound, *(Vector3 *)&mCamSpacePosX, 0);
            }
        }
        break;
    }
    }

    UpdateModel();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_((char *)this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN13daSlide_Box_c13InitResourcesEv
int daSlide_Box_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(
        *(SharedFilePtr *)data_ov016_02114e74);
    mModel.SetFile(modelFile, 1, -1);
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(
        *(SharedFilePtr *)data_ov016_02114e6c);
    /* InitResources: dBgW_KcMbg::SetFile takes Fix12<int> by value.
     * An int argument does not match that parameter, so the call stays
     * the mangled symbol. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY, data_ov016_02113bac);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, kClsnRadius, kClsnRadius, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x14000;
    mShip = 0;
    mState = 0;
    mSoundID = 0;
    return 1;
}
