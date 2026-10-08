//cpp
/* Jolly Roger Bay's sliding crate (SLIDE_BOX, daSlide_Box_c).
 *
 * Actor 0x39 is KI_FUNE_UP, the upward sunken ship. Until the crate
 * lands it falls with the actor's gravity. On the deck it copies the
 * ship's angles, slides by the sine of the pitch, and steps out from
 * the landing spot by the sine and cosine of the ship's yaw. The deck
 * normal then rebuilds mVertSpeed from the horizontal velocity at
 * unk_0a4 / unk_0ac so the next step stays on the tilt.
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
 * - Behavior: mShip->mAngleX/Y/Z each reloads mShip and grows the
 *   function 0x2dc to 0x2e0. One pointer, then the three halfwords.
 * - Behavior: IsClsnInRange(Fix12<int>, Fix12<int>) homes the two
 *   zeros and grows the function 0x2dc to 0x2e8. dBgActor_c.h does
 *   not declare it; the int extern is the call that matches.
 * - Behavior: keeping the pre-clear slide speed for the rolling-sound
 *   test differs by 99 words. The ROM reloads mHorzSpeed after the
 *   store of 0, so the gate does not open.
 * - Behavior: GetFloorResult is not declared on dBgCh_Actr.h. A
 *   temporary declaration made the member call match; that edit is
 *   the shared header, not this TU, so the call stays mangled.
 * - InitResources: SetFile(Fix12<int> by value) grows the function
 *   0xdc to 0xe8. The int extern is the call that matches.
 * - InitResources: storing beforeClsnCallback directly shrinks the
 *   function 0xdc to 0xd8. The ROM calls func_020393d4, the store.
 */

#include "daSlide_Box_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "Sound.h"
#include "dBgPi.h"

namespace cstd { int fdiv(int a, int b); }

/* File 1605 / 1606 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * func_02017acc / func_02017ab4 and func_02017b4c /
 * SharedFilePtr_Destruct_Clsn. 02113bac is the CLPS block SetFile takes. */
struct SlideBoxModelFilePtr : SharedFilePtr {
    u32 words[2];

    SlideBoxModelFilePtr(u32 fileID);
    ~SlideBoxModelFilePtr();
};

struct SlideBoxClsnFilePtr : SharedFilePtr {
    u32 words[2];

    SlideBoxClsnFilePtr(u32 fileID);
    ~SlideBoxClsnFilePtr();
};

extern "C" SlideBoxModelFilePtr data_ov016_02114e74;
extern "C" SlideBoxClsnFilePtr data_ov016_02114e6c;

extern "C" {
/* dBgCh_Actr::Init's header takes Fix12i (= s32), so the method form
   mangles the two radii as `i` and names
   _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_, which no object
   defines. The ROM's is ..._5Fix12IiES3_P10Vector3_16S5_. */
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int actor, Fix12i radius, Fix12i height, int a, int b);
extern int data_ov016_02113bac[];
extern short data_02082214[];
extern void Matrix4x3_FromRotationXYZExt(void *mtx, int angleX, int angleY, int angleZ);

void dBgCh_Actr_UpdateContinuous_Veneer(void *clsn);
dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(void *clsn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int radius, int height);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *kcl, void *mtx, int scale, short angleY, void *clps);
extern void func_020393d4(void *clsn, void *callback);
}

/* Radius and height passed to dBgCh_Actr::Init, 20.0 in 20.12. */
enum { kClsnRadius = 0x14000 };

/* Scale every moving mesh hands to SetFile. */
enum { kMeshScale = 0x199 };

/* Pitch-to-speed scale, and the travel clamp along the ship.
 * 1279.0 and -50.0 in 20.12. kFixRound is the 0.5 added before >> 12. */
enum {
    kSlideScale = 0x8c,
    kHorzPosMax = 0x4ff000,
    kHorzPosMin = -0x32000,
    kFixRound = 0x800
};

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

/* data_02082214 is sin, cos pairs. (u16)angle >> 4 selects the pair.
 * Angle 0 is (0, 4096): the first short is sine, the second cosine. */
#define Sine(angle) (data_02082214[((u16)(angle) >> 4) * 2])
#define Cosine(angle) (data_02082214[((u16)(angle) >> 4) * 2 + 1])
#define FixMul(a, b) ((int)(((long long)(a) * (b) + kFixRound) >> 12))

#define ModelFile data_ov016_02114e74
#define ClsnFile data_ov016_02114e6c

#pragma defer_codegen off

// @symbol _ZN13daSlide_Box_cD1Ev
// @symbol _ZN13daSlide_Box_cD0Ev
/* Empty on purpose. mwccarm destroys mWithMeshClsn, then the inlined
 * dBgActor_c teardown, and emits D1 followed by D0. */
daSlide_Box_c::~daSlide_Box_c()
{
}

// @symbol _ZN13daSlide_Box_c11UpdateModelEv
void daSlide_Box_c::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt((void *)&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    /* Flat Matrix4x3 (common.h): words 9..11 are the translation, at 1/8. */
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
    ModelFile.Release();
    ClsnFile.Release();
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
            mBasePos.x = mPosX;
            mBasePos.y = mPosY;
            mBasePos.z = mPosZ;
        }
        break;
    case kStateRide: {
        /* One pointer, then the three halfwords. mShip->mAngleX/Y/Z
         * each reloads mShip. */
        s16 *shipAngles = &mShip->mAngleX;
        int spd;
        mAngleX = shipAngles[0];
        mAngleY = shipAngles[1];
        mAngleZ = shipAngles[2];
        mPrevAngleY = mAngleY;
        mHorzSpeed = Sine(mAngleX) * kSlideScale;
        mHorzPos += mHorzSpeed;
        spd = mHorzPos;
        if (spd >= kHorzPosMax)
            mHorzPos = kHorzPosMax;
        else if (spd < kHorzPosMin)
            mHorzPos = kHorzPosMin;
        mPosX = mBasePos.x + FixMul(mHorzPos, Sine(mAngleY));
        mPosZ = mBasePos.z + FixMul(mHorzPos, Cosine(mAngleY));
        mHorzSpeed = 0;
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            dBgPi *floor = _ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);
            floor->surface.CopyNormalTo(normal);
            if (normal.y != 0) {
                mVertSpeed = -(cstd::fdiv(
                    FixMul(normal.x, unk_0a4) + FixMul(normal.z, unk_0ac),
                    normal.y) + kDeckSink);
            }
        }
        if (DistToCPlayer() < kSoundRange) {
            int vel = mHorzSpeed;
            if (vel < 0)
                vel = -vel;
            if (vel > kSoundSpeed) {
                mSoundID = Sound::PlayLong(
                    mSoundID, kSoundPlayer, kRollSound, *(Vector3 *)&mCamSpacePosX, 0);
            }
        }
        break;
    }
    }

    UpdateModel();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN13daSlide_Box_c13InitResourcesEv
int daSlide_Box_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(ModelFile);
    mModel.SetFile(modelFile, 1, -1);
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, kMeshScale, mAngleY, data_ov016_02113bac);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)this, kClsnRadius, kClsnRadius, 0, 0);
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
// @symbol daSlide_Box_c_classInit
extern "C" daSlide_Box_c *daSlide_Box_c_classInit()
{
    return new daSlide_Box_c();
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the ROM-ascending run. Construction order is the model
 * handle, then the collision handle. */
SlideBoxModelFilePtr data_ov016_02114e74(1605);
SlideBoxClsnFilePtr data_ov016_02114e6c(1606);
