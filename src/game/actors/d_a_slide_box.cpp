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
enum daSlide_Box_c_AssetId {
    ASSET_KI_SLIDE_BOX_BMD = 0x0645,
    ASSET_KI_SLIDE_BOX_KCL = 0x0646
};

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
 * func_02017acc / func_02017ab4 and func_02017b4c /
 * SharedFilePtr_Destruct_Clsn. 02113bac is the CLPS block SetFile takes. */
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

extern "C" {
int data_ov016_02113bac[8] = {
    0x53504c43, 0x00010008, 0x01000fc5, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

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
enum {
    kSoundRange = 0x7d0000,
    kSoundSpeed = 0x3000,
    kRollSound = 0x9f,
    kSoundPlayer = 3
};

/* Extra downward speed once the crate is on the deck, 8.0 in 20.12. */
enum { kDeckSink = 0x8000 };

/* Gravity and terminal velocity, -2.0 and -20.0. */
enum {
    kGravity = -0x2000,
    kTerminalVelocity = -0x14000
};

/* KI_FUNE_UP. */
enum { kShipActorId = 0x39 };

enum {
    kStateSeekShip = 0,
    kStateFall = 1,
    kStateRide = 2
};

#define FixMul(a, b) ((int)(((long long)(a) * (b) + kFixRound) >> 12))

#define ModelFile gKiSlideBoxModelFile
#define ClsnFile gKiSlideBoxCollisionFile

#pragma defer_codegen off

/* Empty on purpose. mwccarm destroys mWithMeshClsn, then the inlined
 * dBgActor_c teardown, and emits D1 followed by D0. */
daSlide_Box_c::~daSlide_Box_c()
{
}

void daSlide_Box_c::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    /* The translation is at 1/8. */
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

int daSlide_Box_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ModelFile.Release();
    ClsnFile.Release();
    return 1;
}

int daSlide_Box_c::Render()
{
    mModel.Render(0);
    return 1;
}

int daSlide_Box_c::Behavior()
{
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

int daSlide_Box_c::InitResources()
{
    BMD_File *modelFile = dExtSimpleModel_c::LoadFile(ModelFile);
    mModel.SetFile(modelFile, 1, -1);
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *clsnFile = dBgW_Kc::LoadFile(ClsnFile);
    mMeshCollider.SetFile((clsnFile), mClsnMat, kMeshScale, mAngle.y,
                          *(CLPS_Block *)(data_ov016_02113bac));
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
extern char data_ov016_02113af8[];
extern const u32 data_ov016_0211354c[100] = {
    0,          0x00001000, 0x0000ffff, (u32)&data_ov016_02113af8,
    0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x0000005b, 0x00000000, 0x0000002e, 0x0000005b,
    0x00000089, 0x000000b6, 0x000000e4, 0x00000111,
    0x0000013f, 0x0000016c, 0x0000019a, 0x000001c7,
    0x000001f5, 0x00000222, 0x00000250, 0x0000027d,
    0x000002ab, 0x000002d8, 0x00000306, 0x00000333,
    0x00000361, 0x0000038e, 0x000003bc, 0x000003e9,
    0x00000417, 0x00000444, 0x00000472, 0x0000049f,
    0x000004cd, 0x000004fa, 0x00000528, 0x00000555,
    0x00000583, 0x000005b0, 0x000005de, 0x0000060b,
    0x00000639, 0x00000666, 0x00000694, 0x000006c1,
    0x000006ef, 0x0000071c, 0x0000074a, 0x00000777,
    0x000007a5, 0x000007d2, 0x00000800, 0x0000082e,
    0x0000085b, 0x00000889, 0x000008b6, 0x000008e4,
    0x00000911, 0x0000093f, 0x0000096c, 0x0000099a,
    0x000009c7, 0x000009f5, 0x00000a22, 0x00000a50,
    0x00000a7d, 0x00000aab, 0x00000ad8, 0x00000b06,
    0x00000b33, 0x00000b61, 0x00000b8e, 0x00000bbc,
    0x00000be9, 0x00000c17, 0x00000c44, 0x00000c72,
    0x00000c9f, 0x00000ccd, 0x00000cfa, 0x00000d28,
    0x00000d55, 0x00000d83, 0x00000db0, 0x00000dde,
    0x00000e0b, 0x00000e39, 0x00000e66, 0x00000e94,
    0x00000ec1, 0x00000eef, 0x00000f1c, 0x00000f4a,
    0x00000f77, 0x00000fa5, 0x00000fd2, 0x00001000,
};
