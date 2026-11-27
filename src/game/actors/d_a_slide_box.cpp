//cpp
/* Jolly Roger Bay's sliding crate (SLIDE_BOX, daSlide_Box_c).
 *
 * Actor 0x39 is KI_FUNE_UP, the upward sunken ship. Until the crate
 * lands it falls with the actor's gravity. On the deck it copies the
 * ship's angles, slides by the sine of the pitch, and steps out from
 * the landing spot by the sine and cosine of the ship's yaw. The deck
 * normal then rebuilds mSpeed.y from the horizontal velocity at
 * mSpeed.x / mSpeed.z so the next step stays on the tilt.
 *
 * The rolling-sound gate reads mHorzSpeed after this state has stored
 * 0 there, so the sound does not start.
 *
 * daSlide_Box_c_classInit (historical alias SlidingBox_Spawn, folded in
 * from fold-lane-c-0929) hand-called fBase_c::operator new(1272),
 * dBgActor_c::dBgActor_c(), stored _ZTV13daSlide_Box_c, then
 * dBgCh_Actr::dBgCh_Actr() at +0x324. daSlide_Box_c declares no constructor
 * of its own, so the compiler-synthesized default constructor emits exactly
 * that sequence, and the factory is now `return new daSlide_Box_c();`. The
 * retired loose file's comment claimed this hit a measured C/ABI wall
 * (allocator resolving to global _Znwm instead of fBase_c::operator new);
 * re-measured here, that did not reproduce -- 8/8 MATCH, objisolate and
 * reloc-destinations both clean. g_profile_SLIDE_BOX is still outside this
 * TU.
 *
 * deslop leftovers:
 * - Behavior: UpdateContinuous() is WRONG-DEST. Both sites must call
 *   the veneer at arm9 0x020383fc, not _ZN10dBgCh_Actr16UpdateContinuousEv.
 * - Behavior: mShip->mAngle.x/.y/.z each reloads mShip and grows the
 *   function 0x2dc to 0x2e0. One pointer, then the three halfwords.
 * - Behavior: keeping the pre-clear slide speed for the rolling-sound
 *   test differs by 99 words. The ROM reloads mHorzSpeed after the
 *   store of 0, so the gate does not open.
 * - Behavior: GetFloorResult is not declared on dBgCh_Actr.h. A
 *   temporary declaration made the member call match; that edit is
 *   the shared header, not this TU, so the call stays mangled.
 * - InitResources: storing beforeClsnCallback directly shrinks the
 *   function 0xdc to 0xd8. The ROM calls dBgW::SetBeforeClsnCallback, the store.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daSlide_Box_c_AssetId { ASSET_KI_SLIDE_BOX_BMD = 0x0645, ASSET_KI_SLIDE_BOX_KCL = 0x0646 };

#include "daSlide_Box_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "Sound.h"
#include "dBgPi.h"
#include "dBgCh_Actr.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

/* File 1605 / 1606 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * ModelFile::ModelFile / ModelFile::~ModelFile and ClsnFile::ClsnFile /
 * ClsnFile::~ClsnFile. 02113bac is the CLPS block SetFile takes. */
struct SlideBoxModelFilePtr : SharedFilePtr {
    SlideBoxModelFilePtr(u32 fileID);
    ~SlideBoxModelFilePtr();
};

struct SlideBoxClsnFilePtr : SharedFilePtr {
    SlideBoxClsnFilePtr(u32 fileID);
    ~SlideBoxClsnFilePtr();
};

extern "C" SlideBoxModelFilePtr gKiSlideBoxModelFile;
extern "C" SlideBoxClsnFilePtr gKiSlideBoxCollisionFile;

extern struct CLPS_Block data_ov016_02113bac;

extern "C" {
extern void hold_d_a_slide_box(void *thiz, void *kcl, void *mtx, int scale, short angleY,
                               void *clps);
}

/* Radius and height passed to dBgCh_Actr::Init, 20.0 in 20.12. */
enum { kClsnRadius = 0x14000 };

/* Scale every moving mesh hands to SetFile. */
enum { kMeshScale = 0x199 };

/* Pitch-to-speed scale, and the travel clamp along the ship.
 * 1279.0 and -50.0 in 20.12. kFixRound is the 0.5 added before >> 12. */
enum { kSlideScale = 140, kHorzPosMax = 0x4ff000, kHorzPosMin = -0x32000, kFixRound = 0x800 };

/* How close a player has to be, and how fast the crate has to be
 * moving, before the rolling sound would start. 2000.0 and 3.0. */
enum { kSoundRange = 0x7d0000, kSoundSpeed = 0x3000, kRollSound = 0x9f, kSoundPlayer = 3 };

/* Extra downward speed once the crate is on the deck, 8.0 in 20.12. */
enum { kDeckSink = 0x8000 };

/* Gravity and terminal velocity, -2.0 and -20.0. */
enum { kGravity = -0x2000, kTerminalVelocity = -0x14000 };

/* KI_FUNE_UP. */
enum { kShipActorId = 0x39 };

enum { kStateSeekShip = 0, kStateFall = 1, kStateRide = 2 };

#define FixMul(a, b) ((int)(((long long)(a) * (b) + kFixRound) >> 12))

#define ModelFile gKiSlideBoxModelFile
#define ClsnFile gKiSlideBoxCollisionFile

#pragma defer_codegen off

/* Empty on purpose. mwccarm destroys mWithMeshClsn, then the inlined
 * dBgActor_c teardown, and emits D1 followed by D0. */
daSlide_Box_c::~daSlide_Box_c() {}

void daSlide_Box_c::UpdateModel() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    /* The translation is at 1/8. */
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

int daSlide_Box_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ModelFile.Release();
    ClsnFile.Release();
    return 1;
}

int daSlide_Box_c::Render() {
    mModel.Render(0);
    return 1;
}

int daSlide_Box_c::Behavior() {
    Vector3 normal;

    switch (mState) {
    case kStateSeekShip:
        mShip = dActor_c::FindWithActorID(kShipActorId, 0);
        if (mShip == 0) {
            MarkForDestruction();
            break;
        }
        mState++;
        /* fallthrough */
    case kStateFall:
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            mState++;
            mBasePos.x = mPos.x;
            mBasePos.y = mPos.y;
            mBasePos.z = mPos.z;
        }
        break;
    case kStateRide: {
        /* One pointer, then the three halfwords. mShip->mAngle.x/.y/.z
         * each reloads mShip. */
        s16 *shipAngles = &mShip->mAngle.x;
        int spd;
        mAngle.x = shipAngles[0];
        mAngle.y = shipAngles[1];
        mAngle.z = shipAngles[2];
        mPrevAngle.y = mAngle.y;
        mHorzSpeed = FX_SinIdx((u16)mAngle.x >> 4) * kSlideScale;
        mHorzPos += mHorzSpeed;
        spd = mHorzPos;
        if (spd >= kHorzPosMax)
            mHorzPos = kHorzPosMax;
        else if (spd < kHorzPosMin)
            mHorzPos = kHorzPosMin;
        mPos.x = mBasePos.x + FixMul(mHorzPos, FX_SinIdx((u16)mAngle.y >> 4));
        mPos.z = mBasePos.z + FixMul(mHorzPos, FX_CosIdx((u16)mAngle.y >> 4));
        mHorzSpeed = 0;
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            dBgPi *floor = mWithMeshClsn.GetFloorResult();
            floor->surface.CopyNormalTo(normal);
            if (normal.y != 0) {
                mSpeed.y =
                    -(FX_Div(FixMul(normal.x, mSpeed.x) + FixMul(normal.z, mSpeed.z), normal.y) +
                      kDeckSink);
            }
        }
        if (DistToCPlayer() < kSoundRange) {
            int vel = mHorzSpeed;
            if (vel < 0)
                vel = -vel;
            if (vel > kSoundSpeed) {
                mSoundID = Sound::PlayLong(mSoundID, kSoundPlayer, kRollSound, mCamSpacePos, 0);
            }
        }
        break;
    }
    }

    UpdateModel();
    if (IsClsnInRange(0, 0)) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

int daSlide_Box_c::InitResources() {
    BMD_File *modelFile = dExtSimpleModel_c::LoadFile(ModelFile);
    mModel.SetFile(modelFile, 1, -1);
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *clsnFile = dBgW_Kc::LoadFile(ClsnFile);
    mMeshCollider.SetFile((clsnFile), mClsnMat, kMeshScale, mAngle.y, data_ov016_02113bac);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mWithMeshClsn.Init(this, kClsnRadius, kClsnRadius, 0, 0);
    mVertAccel = kGravity;
    mTerminalVelocity = kTerminalVelocity;
    mShip = 0;
    mState = kStateSeekShip;
    mSoundID = 0;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daSlide_Box_c through RTTI,
 * allocation size, vtable identity, and the SLIDE_BOX registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SlidingBox_Spawn. */
daSlide_Box_c *daSlide_Box_c_classInit() {
    return new daSlide_Box_c();
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the ROM-ascending run. Construction order is the model
 * handle, then the collision handle. */
SlideBoxModelFilePtr gKiSlideBoxModelFile(ASSET_KI_SLIDE_BOX_BMD);
SlideBoxClsnFilePtr gKiSlideBoxCollisionFile(ASSET_KI_SLIDE_BOX_KCL);

extern "C" {
u32 data_ov016_02114e94[3];
}

/* The four zero bytes this class's data points at, then the texture keys. */
