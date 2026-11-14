//cpp
/* ov006 0x020bfec0..0x020c4048, the overlay's first linker unit: the card
 * games' dealer and the Toad that walks his table, the presenter face of the
 * cup and sound games, the flower game's Yoshi and its falling petals, and
 * the camera preset each of those scenes starts from.
 *
 * No RTTI, vtable or static initialiser names a type in this run, so every
 * record name here is coined. The records' model members are real
 * dExtSimpleModel_c / dExtAnmModel_c / dExtBlendAnmModel_c members, and each
 * record's constructor and destructor sit at its own ROM address, so the
 * compiler builds and tears its members down.
 *
 * leftovers:
 * - The record and method names are coined: the ROM carries no symbol table
 *   for this run.
 * - Measured shapes kept for the bytes they produce: the IsIdle / IsBusy /
 *   IsIdleAtRest tests return through a flag (returning the test directly
 *   shortens the function); Presenter::PlayCheerLoop stores its state through
 *   a u64 identity mask and a local copy; Presenter::PlayIdle copies its pair
 *   through two volatile reads; MascotWalker::StartNextPath keeps its no-op
 *   `adj ? fn : fn`; FallingPetal::Draw stores its scale z, y, x; and
 *   Dealer::StateReveal keeps its `goto other` (the structured rewrite moves
 *   92 words).
 * - Presenter::Draw's material table stays an int* view reached through a
 *   char* components pointer: naming the ModelComponents or the BMD_Material
 *   pad_0c fields moves 30 words. Dealer::Draw fills its lamp array through a
 *   walking pointer with an if/else body (indexing or a value-select moves
 *   three words). MascotWalker::Update compares its state words through int*
 *   (a StateSlot view changes the relocation).
 * - The identity quaternion stays a four-word QuaternionBlock: spelling
 *   math/Quaternion.h's named members makes mwcc scalarise
 *   FallingPetal::Update's 16-byte copy.
 * - Camera_ProjectToScreen / Camera_ProjectToScreen16 keep `void *pos`
 *   because the scene callers hand them a VecFx32* position and their own
 *   screen output pairs.
 */

#include "types.h"
#include "dFdBrightness_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "PlayerInput.h"
#include "nitro/gx/g3_util.h"
#include "dClipper.h"
#include "MascotWalker.h"
#include "Presenter.h"
#include "Dealer.h"
#include "Mascot.h"
#include "FlowerFace.h"
#include "BMD_File.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmTexSRT_c.h"
#include "nitro/gx/gx_bgcnt.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "dScMgBase_c.h"
#include "MWCPlusLib.h"
#include "LevelRng.h"
#include "MgCamera.h"
#include "math/Quaternion.h"
#include "nitro/fx/fx_cp.h"

struct Sound {
    static void PlayBank2_2D(unsigned int);
};

void CopyToViewMat(const Matrix4x3 *);

/* A record whose +0 word pair is its current state handler: slot 0 is the
   handler's code word and slot 1 its this-adjustment (zero in every record
   here). The pair is always stored and copied whole. */
struct StateSlot {
    int w[2];
};

/* Presenter::PlayIdle copies its state pair through two volatile word reads;
   a plain struct copy moves two words and costs the function its bytes. */
struct VolatileStateSlot {
    void *lo, *hi;
};

/* A quaternion as four raw words. The identity in data_02092768 is copied
   into a scratch quaternion with a 16-byte block move; spelling the four
   members out (math/Quaternion.h) makes mwcc scalarise that copy and costs
   FallingPetal::Update its bytes (measured). */
struct QuaternionBlock {
    int w[4];
};

/* A state handler is a member function of its own record, so each record
   that dispatches through one needs its own pointer-to-member type. */
typedef void (MascotWalker::*MascotWalkerState)();
typedef void (Dealer::*DealerState)();
typedef void (Presenter::*PresenterState)();
typedef void (FlowerFace::*FlowerFaceState)();

/* One of the four walks MascotWalker::mPath points at: a point count, then
   the points (coined). */
struct WalkPath {
    int count;
    Vector3 *points;
};

/* local extern: address-named arm9 routines, no owning header. */
extern "C" {
void func_020169d8(void *, int, unsigned int);
void func_02016a04(void *, int);
void func_02016a14(void *, int);
int func_02017ab4(void *);
void *func_02017acc(void *, unsigned int);
unsigned int func_02012174(unsigned int, unsigned int);
void func_0203ccd4(void *, short);
void func_0203cd80(void *, short);
void func_0203cebc(void *, void *, void *, void *);
}

/* The overlay's own identity matrix, which the flower model loads. */
extern Matrix4x3 data_ov006_0213aee8;
/* The scene-shared start position and rotation of the dealer and the walker. */
extern Vector3 data_020a0ebc;
extern Vector3_16 data_020a0edc;
/* Per-record constants and tables in arm9. */
extern Vector3 sPetalUpAxis;
extern Vector3 sMascotWalkerOffset;
extern Vector3 sPresenterScale;
extern Matrix4x3 sDealerIdentityMtx;
extern short sDealerLampTimes[];
extern WalkPath sMascotWalkerPaths[];
extern QuaternionBlock data_02092768; /* identity */
extern dClipper data_0209f43c;
extern dFdBrightness_c *gSceneFader;
/* The presenter's BTA images: each name starts a file the face plays. */
extern char sPresenterLoseLoopTexAnim[];
extern char sPresenterSmileTexAnim[];
extern char sPresenterLoseTexAnim[];
extern char sPresenterIdleTexAnim[];
extern char sPresenterMissTexAnim[];
extern char sPresenterTextureAnim[];
extern char sPresenterCheerTexAnim[];
extern char sPresenterMatchTexAnim[];

/* The {handler, this-adjustment} records of the state handlers. */
extern StateSlot pmf_FlowerFace_StateIdle;
extern StateSlot pmf_FlowerFace_StateLike;
extern StateSlot pmf_FlowerFace_StateHate;
extern StateSlot pmf_FlowerFace_StatePick;
extern StateSlot pmf_Presenter_StateIdle;
extern StateSlot pmf_Presenter_StateLoseStart;
extern StateSlot pmf_Presenter_StateMistake;
extern volatile VolatileStateSlot pmf_Presenter_StateIdle_2;
extern long long pmf_Presenter_StateWin;
extern volatile long long pmf_Presenter_StateSurprisedThenSmile;
extern long long pmf_Presenter_StateSurprisedThenIdle;
extern StateSlot pmf_MascotWalker_StatePause;
extern StateSlot pmf_MascotWalker_StateWalk_3;
extern StateSlot pmf_Dealer_StateIdle;
extern StateSlot pmf_Dealer_StateShuffle;
extern StateSlot pmf_Dealer_StateDeal;
extern StateSlot pmf_Dealer_StateReveal_2;
extern StateSlot pmf_Dealer_StateSwap;
extern StateSlot pmf_Dealer_StateLose;
extern StateSlot pmf_Dealer_StateWin;
/* The same records where the IsIdle/IsBusy tests compare the words. */
extern int pmf_FlowerFace_StateIdle_2[];
extern int pmf_MascotWalker_StateWalk[];
extern int pmf_MascotWalker_StateWalk_2[2];
extern int pmf_Dealer_StateIdle_2[];
extern int pmf_Dealer_StateIdle_3[];
extern int pmf_Dealer_StateDeal_2[2];
extern int pmf_Dealer_StateReveal[2];

/* The file handles' constructors and destructors are the arm9 veneers the
   ROM calls: func_02017acc / func_02017ab4 for a model file,
   SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for an animation.
   No ov006 body exists for them, so they are inline, and defined only here:
   each use expands into the veneer call. */
inline DealerModelFilePtr::DealerModelFilePtr(u32 fileID) {
    func_02017acc(this, fileID);
}
inline DealerModelFilePtr::~DealerModelFilePtr() {
    func_02017ab4(this);
}
inline DealerAnimationFileHandle::DealerAnimationFileHandle(u32 fileID) {
    Construct(fileID);
}
inline DealerAnimationFileHandle::~DealerAnimationFileHandle() {
    SharedFilePtr_Destruct_Anim((int)this);
}
inline MascotWalkerModelFilePtr::MascotWalkerModelFilePtr(u32 fileID) {
    func_02017acc(this, fileID);
}
inline MascotWalkerModelFilePtr::~MascotWalkerModelFilePtr() {
    func_02017ab4(this);
}
inline MascotWalkerAnimationFileHandle::MascotWalkerAnimationFileHandle(u32 fileID) {
    Construct(fileID);
}
inline MascotWalkerAnimationFileHandle::~MascotWalkerAnimationFileHandle() {
    SharedFilePtr_Destruct_Anim((int)this);
}
inline PresenterModelFilePtr::PresenterModelFilePtr(u32 fileID) {
    func_02017acc(this, fileID);
}
inline PresenterModelFilePtr::~PresenterModelFilePtr() {
    func_02017ab4(this);
}
inline PresenterAnimationFileHandle::PresenterAnimationFileHandle(u32 fileID) {
    Construct(fileID);
}
inline PresenterAnimationFileHandle::~PresenterAnimationFileHandle() {
    SharedFilePtr_Destruct_Anim((int)this);
}
inline PresenterTexSeqFileHandle::PresenterTexSeqFileHandle(u32 fileID) {
    SharedFilePtr_Construct_TexSeq(this, fileID);
}
inline PresenterTexSeqFileHandle::~PresenterTexSeqFileHandle() {
    SharedFilePtr_Destruct_TexSeq((int)this);
}
inline FlowerFaceModelFilePtr::FlowerFaceModelFilePtr(u32 fileID) {
    func_02017acc(this, fileID);
}
inline FlowerFaceModelFilePtr::~FlowerFaceModelFilePtr() {
    func_02017ab4(this);
}
inline FlowerFaceAnimationFileHandle::FlowerFaceAnimationFileHandle(u32 fileID) {
    Construct(fileID);
}
inline FlowerFaceAnimationFileHandle::~FlowerFaceAnimationFileHandle() {
    SharedFilePtr_Destruct_Anim((int)this);
}

/* ---- FallingPetal::FallingPetal ---- */
/* mModel is the only member with a constructor. */
FallingPetal::FallingPetal() {}

/* ---- FlowerFace::FlowerFace ---- */
/* The petals, the model and the six file handles are members, so the
   compiler builds them in declaration order: the petal array through
   __cxa_vec_ctor with FallingPetal's own constructor and destructor. */
FlowerFace::FlowerFace()
    : mModelFile(0x222), mPetalFile(0x201), mWaitAnim(0x224), mPickAnim(0x223), mLikeAnim(0x221),
      mHateAnim(0x220) {}

/* ---- FlowerFace::~FlowerFace ---- */
/* Releases every file; the members' destructors then run in reverse
   declaration order: the four animations, the two models' files, the
   model and the petal array. */
FlowerFace::~FlowerFace() {
    mModelFile.Release();
    mPetalFile.Release();
    mWaitAnim.Release();
    mPickAnim.Release();
    mLikeAnim.Release();
    mHateAnim.Release();
}

/* ---- FallingPetal::~FallingPetal ---- */
FallingPetal::~FallingPetal() {}

int FlowerFace::Load() {
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(mModelFile);
    mModel.SetFile(bmd, 1, -1);
    dExtFrameCtrl_c::LoadFile(mWaitAnim);
    dExtFrameCtrl_c::LoadFile(mPickAnim);
    dExtFrameCtrl_c::LoadFile(mLikeAnim);
    dExtFrameCtrl_c::LoadFile(mHateAnim);
    PlayIdle();
    BMD_File *petalFile = dExtSimpleModel_c::LoadFile(mPetalFile);
    for (int i = 0; i < 22; i++) {
        mPetals[i].SetModel(petalFile);
        mPetals[i].mActive = 0;
    }
    return 1;
}

void FlowerFace::Update() {
    (this->*(*(FlowerFaceState *)mState))();
    mModel.Advance();
    int i = 0;
    do {
        if (mPetals[i].mActive != 0)
            mPetals[i].Update();
        i++;
    } while (i < 22);
}

void FlowerFace::Draw() {
    Vector3 light;
    int i;

    light.x = 0;
    light.y = 0;
    light.z = -0xff8;
    func_0203cd80(&light, 0);
    func_0203ccd4(&light, 0x600);

    G3_LightVector(GX_LIGHTID_0, light.x, light.y, light.z);

    func_02016a14(&mModel, 0x2bff);
    func_02016a04(&mModel, 0x1211);

    mModel.mat4x3 = data_ov006_0213aee8;

    mModel.Render(0);

    for (i = 0; i < 22; i++) {
        if (mPetals[i].mActive != 0)
            mPetals[i].Draw();
    }
}

void FlowerFace::Reset() {
    for (int i = 0; i < 22; i++)
        mPetals[i].mActive = 0;
    PlayIdle();
}

int FlowerFace::IsIdle() {
    int *idle = pmf_FlowerFace_StateIdle_2;
    int fn = mState[0];
    /* The 0/1 result goes through a flag: returning the test directly emits a
       shorter function than the ROM's (measured). */
    int differs = 1;
    if (fn == idle[0]) {
        if (mState[1] == idle[1] || fn == 0)
            differs = 0;
    }
    return differs == 0;
}

/* Camera_InitFlowerView at 0x020c3b2c
 *
 * MgCamera preset: sets the look-at point, position and fov angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

void Camera_InitFlowerView(MgCamera *camera) {
    camera->mLookAt.x = 0x1b000;
    camera->mLookAt.y = 0x17600;
    camera->mLookAt.z = -0x22f00;
    camera->mPos.x = 0x38000;
    camera->mPos.y = 0x22500;
    camera->mPos.z = -0x51200;
    camera->mFovAngle = 0xb30;
    Camera_UpdateMatrices(camera);
}

void FlowerFace::PlayIdle() {
    mModel.SetAnim((BCA_File *)mWaitAnim.filePtr, 0, 0x800, 0);
    *(StateSlot *)mState = pmf_FlowerFace_StateIdle;
}

void FlowerFace::StateIdle() {}

void FlowerFace::PlayPetalPulled() {
    int i;
    /* The spawn point and velocity, and the copies Spawn is handed. */
    struct PetalSpawn {
        VecFx32 pos, vel;
        VecFx32 pos2, vel2;
    } b;

    mModel.SetAnim((BCA_File *)mPickAnim.filePtr, 0x40000000, 0x800, 0);

    mModel.currFrame = 0; /* rewind */

    for (i = 0; i < 0x16; i++) {
        if (mPetals[i].mActive == 0) {
            b.pos.x = 0x2000;
            b.pos.y = 0xa000;
            b.pos.z = 0x1000;
            b.vel.x = 0x600;
            b.vel.y = 0xa00;
            b.vel.z = 0xa00;
            /* Each component gets 0..0x3ff of jitter from a fresh random. */
            b.vel.x +=
                (int)((unsigned int)(RandomIntInternal(&data_0209e650.value) & ~0x80000000) >>
                      19) >>
                2;
            b.vel.y +=
                (int)((unsigned int)(RandomIntInternal(&data_0209e650.value) & ~0x80000000) >>
                      19) >>
                2;
            b.vel.z +=
                (int)((unsigned int)(RandomIntInternal(&data_0209e650.value) & ~0x80000000) >>
                      19) >>
                2;
            b.pos2.x = b.pos.x;
            b.pos2.y = b.pos.y;
            b.pos2.z = b.pos.z;
            b.vel2.x = b.vel.x;
            b.vel2.y = b.vel.y;
            b.vel2.z = b.vel.z;
            mPetals[i].Spawn(&b.pos2, &b.vel2);
            break;
        }
    }

    *(StateSlot *)this = pmf_FlowerFace_StatePick;
}

void FlowerFace::StatePick() {
    if (mModel.Finished() == 0)
        return;
    PlayIdle();
}

void FlowerFace::PlayLike() {
    mModel.SetAnim((BCA_File *)mLikeAnim.filePtr, 0x40000000, 0x800, 0);
    *(StateSlot *)mState = pmf_FlowerFace_StateLike;
}

void FlowerFace::StateLike() {}

void FlowerFace::PlayHate() {
    mModel.SetAnim((BCA_File *)mHateAnim.filePtr, 0x40000000, 0x800, 0);
    *(StateSlot *)mState = pmf_FlowerFace_StateHate;
}

void FlowerFace::StateHate() {}

void FallingPetal::SetModel(BMD_File *model) {
    mModel.SetFile(model, 1, -1);
    mActive = 0;
}

void FallingPetal::Spawn(const VecFx32 *pos, const VecFx32 *vel) {
    /* Every petal spins about +Y. Constructed on the first spawn: mwcc emits
       the guard test, the three stores and the ~Vector3() registration here. */
    static Vector3 sAxis(0, 0x1000, 0);

    mPos.x = pos->x;
    mPos.y = pos->y;
    mPos.z = pos->z;
    mVel.x = vel->x;
    mVel.y = vel->y;
    mVel.z = vel->z;
    mAccel.x = 0;
    mAccel.y = -0x40;
    mAccel.z = 0;
    mLife = 0xb4;
    mAxis.x = sAxis.x;
    mAxis.y = sAxis.y;
    mAxis.z = sAxis.z;
    func_0203cd80(&mAxis, RandomIntInternal(&data_0209e650.value));
    mRot[0] = data_02092768.w[0];
    mRot[1] = data_02092768.w[1];
    mRot[2] = data_02092768.w[2];
    mRot[3] = data_02092768.w[3];
    Quaternion_FromVector3(mRot, &sAxis, &mAxis);
    mActive = 1;
}

void FallingPetal::Update() {
    Vector3 sway;
    Vector3 flow;
    Vector3 drag;
    Vector3 dir;
    QuaternionBlock target;
    QuaternionBlock turn;
    int phase;
    int wave;
    int dot;

    if (ApproachLinear2(mLife, 0, 1) != 0) {
        mActive = 0;
        return;
    }
    VEC_Add(&mPos, &mVel, &mPos);
    mAccel.y = -0x40;
    VEC_Add(&mVel, &mAccel, &mVel);
    phase = ((mPos.y >> 1) << 16) >> 20;
    wave = FX_SinIdx(phase);
    sway.x = wave;
    sway.y = 0;
    sway.z = wave;
    Vec3_MulScalarInPlace(&sway, 0x400);
    Vec3_Add(&flow, &mVel, &sway);
    dot = VEC_DotProduct(&mAxis, &flow);
    Vec3_MulScalar(&drag, &mAxis, dot);
    Vec3_MulScalarInPlace(&drag, 0x300);
    VEC_Subtract(&mVel, &drag, &mVel);
    dir.x = flow.x;
    dir.y = flow.y;
    dir.z = flow.z;
    if (NormalizeVec3IfNonZero(&dir) == 0)
        return;
    turn = data_02092768;
    Quaternion_FromVector3(&target, &mAxis, &dir);
    Quaternion_SLerp(turn.w, target.w, 0x200, turn.w);
    Quaternion_Normalize(turn.w);
    Quaternion_Multiply(mRot, &turn, mRot);
}

void FallingPetal::Draw() {
    UpdateMatrix();
    /* Stored z, y, x: the ROM's order (measured; x, y, z moves two words). */
    Vector3 scale;
    scale.z = 0x1000;
    scale.y = 0x1000;
    scale.x = 0x1000;
    mModel.Render(&scale);
}

void FallingPetal::UpdateMatrix() {
    union {
        Matrix4x3 full;
        Matrix3x3 r;
    } rot;
    Matrix4x3_FromQuaternion(reinterpret_cast<const Quaternion *>(mRot), &rot.full);
    MTX_MultVec33(&sPetalUpAxis, &rot.r, &mAxis);
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    MTX_Concat43(&rot.full, &data_020a0e68, &data_020a0e68);
    mModel.mat4x3 = data_020a0e68;
}

/* ---- Presenter::Presenter ---- */
/* The four model parts and the eighteen file handles are members, so the
   compiler builds them in declaration order before the body runs. */
Presenter::Presenter()
    : mFaceModel(0x286), mBackModel(0x200), mWaitAnim(0x1fc), mMistakeAnim(0x1f6), mWinAnim(0x1fe),
      mLoseAnim(0x1f0), mLoseStartAnim(0x1f4), mLoseEndAnim(0x1f2), mSmileAnim(0x1f8),
      mSurprisedAnim(0x1fa), mWaitTexSeq(0x1fd), mMistakeTexSeq(0x1f7), mWinTexSeq(0x1ff),
      mLoseTexSeq(0x1f1), mLoseStartTexSeq(0x1f5), mLoseEndTexSeq(0x1f3), mSmileTexSeq(0x1f9),
      mSurprisedTexSeq(0x1fb) {
    mTouchLocked = 0;
    mQuiet = 0;
}

/* ---- Presenter::~Presenter ---- */
/* Releases every file; the members' destructors then run in reverse
   declaration order: the sixteen animation handles, the two model files,
   then mTexAnim, mTexSeq, mBack and mFace. */
Presenter::~Presenter() {
    mFaceModel.Release();
    mBackModel.Release();
    mWaitAnim.Release();
    mMistakeAnim.Release();
    mWinAnim.Release();
    mLoseAnim.Release();
    mLoseStartAnim.Release();
    mLoseEndAnim.Release();
    mSmileAnim.Release();
    mSurprisedAnim.Release();
    mWaitTexSeq.Release();
    mMistakeTexSeq.Release();
    mWinTexSeq.Release();
    mLoseTexSeq.Release();
    mLoseStartTexSeq.Release();
    mLoseEndTexSeq.Release();
    mSmileTexSeq.Release();
    mSurprisedTexSeq.Release();
}

int Presenter::Load() {
    BMD_File *faceFile;
    BMD_File *backFile;

    faceFile = dExtSimpleModel_c::LoadFile(mFaceModel);
    backFile = dExtSimpleModel_c::LoadFile(mBackModel);
    dExtFrameCtrl_c::LoadFile(mWaitAnim);
    dExtFrameCtrl_c::LoadFile(mMistakeAnim);
    dExtFrameCtrl_c::LoadFile(mWinAnim);
    dExtFrameCtrl_c::LoadFile(mLoseAnim);
    dExtFrameCtrl_c::LoadFile(mLoseStartAnim);
    dExtFrameCtrl_c::LoadFile(mLoseEndAnim);
    dExtFrameCtrl_c::LoadFile(mSmileAnim);
    dExtFrameCtrl_c::LoadFile(mSurprisedAnim);
    dExtAnmTexPat_c::LoadFile(mWaitTexSeq);
    dExtAnmTexPat_c::LoadFile(mMistakeTexSeq);
    dExtAnmTexPat_c::LoadFile(mWinTexSeq);
    dExtAnmTexPat_c::LoadFile(mLoseTexSeq);
    dExtAnmTexPat_c::LoadFile(mLoseStartTexSeq);
    dExtAnmTexPat_c::LoadFile(mLoseEndTexSeq);
    dExtAnmTexPat_c::LoadFile(mSmileTexSeq);
    dExtAnmTexPat_c::LoadFile(mSurprisedTexSeq);

    if (!mFace.SetFile(faceFile, 1, -1))
        return 0;

    mFace.SetMaterialFlags(2);

    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mWaitTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mMistakeTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mWinTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mLoseTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mLoseStartTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mLoseEndTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mSmileTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*faceFile, *(BTP_File *)mSurprisedTexSeq.filePtr);

    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterIdleTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterMissTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterMatchTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterLoseLoopTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterLoseTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterTextureAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterCheerTexAnim);
    dExtAnmTexSRT_c::Prepare(*faceFile, *(BTA_File *)sPresenterSmileTexAnim);

    PlayIdle();
    mPos.x = 0;
    mPos.y = 0;
    mPos.z = 0;
    UpdateMatrix();

    if (!mBack.SetFile(backFile, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    mBack.mat4x3 = data_020a0e68;
    return 1;
}

/* Eases the four mSway words (Presenter::Draw adds them to two face
   materials' texture offsets) a quarter of the way toward targets set by
   the stylus point (x, y), taken as fx32 screen fractions x / 255, y / 192. */
#define PRESENTER_TRACK(c, x, y) \
    { \
        int fracX = ((x) << 12) / 255; \
        int fracY = ((y) << 12) / 192; \
        int below = 0x1000 - fracY; \
        int tiltX; \
        int tiltY; \
        if (fracX > 0x800) { \
            int dist = (fracX - 0x800) << 1; \
            tiltX = FX_MUL(dist, -0x100); \
            tiltY = FX_MUL(dist, -0x260); \
        } else { \
            int dist = (0x800 - fracX) << 1; \
            tiltX = FX_MUL(dist, 0x260); \
            tiltY = FX_MUL(dist, 0x100); \
        } \
        { \
            int lift = FX_MUL(fracY, 0x20) + FX_MUL(below, 0x500); \
            int sway0 = c->mSway[0]; \
            int sway2 = c->mSway[2]; \
            tiltY -= sway2; \
            int sway1 = c->mSway[1]; \
            int sway3 = c->mSway[3]; \
            c->mSway[0] += (tiltX - sway0) >> 2; \
            c->mSway[2] += tiltY >> 2; \
            sway1 = lift - sway1; \
            c->mSway[1] += sway1 >> 2; \
            sway3 = lift - sway3; \
            c->mSway[3] += sway3 >> 2; \
        } \
    }

void Presenter::TrackStylus() {
    if (mTouchLocked != 0) {
        int x = mTouchX;
        int y = mTouchY;
        PRESENTER_TRACK(this, x, y);
        return;
    }
    if (IsIdle() != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int x = gTouchX[i * 4];
            int y = gTouchY[i * 4];
            PRESENTER_TRACK(this, x, y);
            return;
        }
    }
    mSway[0] = FX_MUL(mSway[0], 0xE00);
    mSway[1] = FX_MUL(mSway[1], 0xE00);
    mSway[2] = FX_MUL(mSway[2], 0xE00);
    mSway[3] = FX_MUL(mSway[3], 0xE00);
}

void Presenter::Update() {
    TrackStylus();
    if (mState[0])
        (this->*(*(PresenterState *)mState))();
    mTexSeq.Advance();
    mTexAnim.Advance();
    mFace.Advance();
    UpdateMatrix();
}

void Presenter::Draw() {
    int keyLight[3];
    int fillLight[3];
    int *materials;
    ModelComponents *components;

    keyLight[0] = 0;
    keyLight[1] = 0;
    keyLight[2] = -0xff8;
    fillLight[0] = 0;
    fillLight[1] = 0;
    fillLight[2] = -0xff8;
    func_02016a04(&mFace, 0x3d6d);
    func_0203cd80(keyLight, -0x2000);
    func_0203ccd4(keyLight, 0x2000);
    func_0203cd80(fillLight, 0x4200);
    func_0203ccd4(fillLight, 0x7000);

    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightVector(GX_LIGHTID_0, keyLight[0], keyLight[1], keyLight[2]);
    components = &mFace.data;
    G3_LightColor(GX_LIGHTID_1, GX_RGB(11, 10, 10));
    G3_LightVector(GX_LIGHTID_1, fillLight[0], fillLight[1], fillLight[2]);

    /* Materials 5 and 6, texture offsets at +0x14 and +0x18. */
    materials = (int *)components->materials;
    mTexSeq.Update(*components);
    mTexAnim.Update(*components);
    materials[0x41] -= mSway[2];
    materials[0x42] += mSway[3];
    materials[0x4d] += mSway[0];
    materials[0x4e] += mSway[1];
    mFace.Render(0);
    mBack.Render(&sPresenterScale);
}

int Presenter::IsIdle() {
    int differs = 1;
    StateSlot *idle = &pmf_Presenter_StateIdle;
    if (mState[0] == idle->w[0]) {
        if (mState[1] != idle->w[1] && mState[0] != 0)
            ;
        else
            differs = 0;
    }
    return differs == 0;
}

/* Presenter::IsAnimDone @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b dExtFrameCtrl_c::Finished(). */

int Presenter::IsAnimDone() {
    return mFace.Finished();
}

void Presenter::ResetPose() {
    PlayIdle();
    if (gSceneFader->IsAtEnd() == 0)
        return;
    mSway[1] = 0;
    mSway[0] = mSway[1];
    mSway[3] = 0;
    mSway[2] = mSway[3];
    /* Rewind each frame control to frame 0. */
    mFace.currFrame = 0;
    mTexSeq.currFrame = 0;
    mTexAnim.currFrame = 0;
}

void Presenter::PlayIdle() {
    if (mFace.file != mWaitAnim.filePtr) {
        int blendFrames = 0;
        if (gSceneFader->IsAtEnd() == 0) {
            if (mFace.Finished() == 0 || mFace.file == mSurprisedAnim.filePtr) {
                blendFrames = 8;
            }
        }
        mFace.SetAnim(*(BCA_File *)mWaitAnim.filePtr, blendFrames, 0, 0x800, 0);
        mTexSeq.SetFile(*(BTP_File *)mWaitTexSeq.filePtr, 0, 0x800, 0);
        mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
    }
    /* The volatile word reads are load-bearing: a plain struct copy moves two
       words (measured 2026-11-08). */
    {
        void *lo = pmf_Presenter_StateIdle_2.lo;
        void *hi = pmf_Presenter_StateIdle_2.hi;
        mState[0] = (int)lo;
        mState[1] = (int)hi;
    }
}

void Presenter::StateIdle() {
    if (!mFace.Finished())
        return;
    mFace.SetAnim(*(BCA_File *)mWaitAnim.filePtr, 0, 0, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mWaitTexSeq.filePtr, 0, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
}

void Presenter::PlayMatch() {
    unsigned long long stateValue;
    long long *stateRecord;
    if (mQuiet == 0) {
        func_02012174(0, 0x1c);
    }
    mFace.SetAnim(*(BCA_File *)mWinAnim.filePtr, 4, 0x40000000, 0x800, 0);
    /* The state pointer and the unsigned copy are load-bearing: storing
       pmf_Presenter_StateWin straight through the record costs the function its bytes
       (measured 2026-11-08). */
    stateRecord = (long long *)this;
    mTexSeq.SetFile(*(BTP_File *)mWinTexSeq.filePtr, 0x40000000, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterMatchTexAnim, 0x40000000, 0x800, 0);
    stateValue = pmf_Presenter_StateWin;
    *stateRecord = stateValue;
}

void Presenter::StateWin() {
    int r = IsAnimDone();
    if (r == 0)
        return;
    PlayIdle();
}

void Presenter::PlayMiss() {
    mFace.SetAnim(*(BCA_File *)mMistakeAnim.filePtr, 4, 0x40000000, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mMistakeTexSeq.filePtr, 0x40000000, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterMissTexAnim, 0x40000000, 0x800, 0);
    *(StateSlot *)this = pmf_Presenter_StateMistake;
}

void Presenter::StateMistake() {
    int r = IsAnimDone();
    if (r == 0)
        return;
    PlayIdle();
}

/* volatile is load-bearing: a plain long long load costs Presenter::PlayCheer
   its bytes (measured 2026-11-08). */
void Presenter::PlayCheer() {
    if ((mQuiet) == 0) {
        func_02012174(0, 3);
    }
    mFace.SetAnim(*(BCA_File *)mSurprisedAnim.filePtr, 8, 0x40000000, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mSurprisedTexSeq.filePtr, 0x40000000, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    *((long long *)this) = pmf_Presenter_StateSurprisedThenSmile;
}

void Presenter::StateSurprisedThenSmile() {
    if (!IsAnimDone())
        return;
    if (mFace.file != mSurprisedAnim.filePtr)
        return;
    if (mQuiet == 0)
        Sound::PlayBank2_2D(0x1d1);
    mFace.SetAnim(*(BCA_File *)mSmileAnim.filePtr, 8, 0, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mSmileTexSeq.filePtr, 0, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterSmileTexAnim, 0, 0x800, 0);
}

void Presenter::PlayLose() {
    if (mQuiet == 0)
        Sound::PlayBank2_2D(0x1c9);
    mFace.SetAnim(*(BCA_File *)mLoseStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mLoseStartTexSeq.filePtr, 0x40000000, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterLoseTexAnim, 0x40000000, 0x800, 0);
    *(StateSlot *)this = pmf_Presenter_StateLoseStart;
}

void Presenter::StateLoseStart() {
    if (!IsAnimDone())
        return;
    if (mFace.file != mLoseStartAnim.filePtr)
        return;
    mFace.SetAnim(*(BCA_File *)mLoseAnim.filePtr, 0, 0, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mLoseTexSeq.filePtr, 0, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterLoseLoopTexAnim, 0, 0x800, 0);
}

void Presenter::PlayCheerLoop() {
    long long state;
    if (mQuiet == 0) {
        func_02012174(0, 3);
    }
    mFace.SetAnim(*(BCA_File *)mSurprisedAnim.filePtr, 8, 0x40000000, 0x800, 0);
    mTexSeq.SetFile(*(BTP_File *)mSurprisedTexSeq.filePtr, 0x40000000, 0x800, 0);
    mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    /* The u64 identity mask and the local copy are load-bearing: storing the
       record straight through costs the function its bytes (measured). */
    *((long long *)this) = (state = pmf_Presenter_StateSurprisedThenIdle & 0xFFFFFFFFFFFFFFFF);
}

void Presenter::StateSurprisedThenIdle() {
    int r = IsAnimDone();
    if (r == 0)
        return;
    PlayIdle();
}

void Presenter::UpdateMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    mFace.mat4x3 = data_020a0e68;
}

/* Camera_InitPresenterView at 0x020c225c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

void Camera_InitPresenterView(MgCamera *camera) {
    camera->mLookAt.x = 0;
    camera->mLookAt.y = 0;
    camera->mLookAt.z = 0;
    camera->mPos.x = 0;
    camera->mPos.y = 0;
    camera->mPos.z = 0x2d000;
    camera->mFovAngle = 0x800;
    Camera_UpdateMatrices(camera);
}

/* ---- Mascot::Mascot ---- */
/* The two file handles and the model are members, so the compiler builds
   them in declaration order before the body runs. */
Mascot::Mascot() : mModelFile(0x205), mWaitAnimFile(0x206) {
    mLooking = 0;
}

/* ---- Mascot::~Mascot ---- */
/* Releases both files; the members' destructors then run in reverse
   declaration order: the model, the animation, the model file. */
Mascot::~Mascot() {
    mModelFile.Release();
    mWaitAnimFile.Release();
}

void Mascot::Load() {
    BMD_File *f;
    f = dExtSimpleModel_c::LoadFile(mModelFile);
    mModel.SetFile(f, 1, -1);
    dExtFrameCtrl_c::LoadFile(mWaitAnimFile);
    mModel.SetAnim(*(BCA_File *)mWaitAnimFile.filePtr, 0, 0, 0x800, 0);
    mModel.HideMaterial(7, 0);
    mPos.x = 0xd000;
    mPos.y = 0;
    mPos.z = 0;
    mRot[0] = 0;
    mRot[1] = -0x1400;
    mRot[2] = 0;
}

/* Mascot::Update @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b dExtBlendAnmModel_c::Advance(). */

void Mascot::Update() {
    mModel.Advance();
}

/* The render is dExtSimpleModel_c's own, called directly: 0x02016b78, not dExtBlendAnmModel_c's
   slot 5. */
void Mascot::Draw() {
    if (mLooking != 0) {
        s16 horzAngle = Vec3_HorzAngle(&mPos, &mLookTarget);
        s16 vertAngle = Vec3_VertAngle(&mPos, &mLookTarget);
        /* Turn the body toward the target, the head toward the body. */
        s16 deltaYaw = (horzAngle - mRot[1]);
        if (deltaYaw < -0x2800)
            deltaYaw = -0x2800;
        else if (deltaYaw > 0x2800)
            deltaYaw = 0x2800;
        if (vertAngle < -0x1000)
            vertAngle = -0x1000;
        else if (vertAngle > 0x1000)
            vertAngle = 0x1000;
        UpdateAngle(mHeadYaw, deltaYaw, 8, 0x200);
        UpdateAngle(mHeadPitch, vertAngle, 8, 0x200);
        deltaYaw = (horzAngle - deltaYaw);
        UpdateAngle(mRot[1], deltaYaw, 8, 0x200);
    } else {
        UpdateAngle(mHeadPitch, 0, 8, 0x200);
        UpdateAngle(mHeadYaw, 0x800, 8, 0x200);
        UpdateAngle(mRot[1], -0x1400, 8, 0x200);
    }
    UpdateMatrix();
    mModel.dExtSimpleModel_c::Render(0);
}

void Mascot::UpdateMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mRot[1]);
    mModel.mat4x3 = data_020a0e68;
    mModel.UpdateVerts();
    /* bone 6, the head */
    data_020a0e68 = mModel.data.transforms[6];
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mHeadPitch);
    mModel.data.transforms[6] = data_020a0e68;
}

void Mascot::LookAt(int *src) {
    if (src != 0) {
        mLookTarget.x = src[0];
        mLookTarget.y = src[1];
        mLookTarget.z = src[2];
    } else {
        mLookTarget.x = 0;
        mLookTarget.y = 0x2000;
        mLookTarget.z = -0x2000;
    }
    mLooking = 1;
}

void Mascot::StopLooking() {
    mLooking = 0;
}

/* Camera_InitSlotsView at 0x020c1eb4
 *
 * MgCamera preset: sets the look-at point, position and fov angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

void Camera_InitSlotsView(MgCamera *camera) {
    camera->mLookAt.x = 0;
    camera->mLookAt.y = 0xe000;
    camera->mLookAt.z = -0x22f00;
    camera->mPos.x = 0;
    camera->mPos.y = 0xf000;
    camera->mPos.z = 0x32000;
    camera->mFovAngle = 0xb00;
    Camera_UpdateMatrices(camera);
}

/* ---- Dealer::Dealer ---- */
/* mModel, mTable, mWalker and the two model files are members with
   constructors, so the compiler builds them in declaration order; the
   animation handles take their files in the body. */
Dealer::Dealer()
    : mHand(0), mShuffleRound(0), mReserved(0), mModelFile(0x208), mTableFile(0), mWaitAnim(0x212),
      mShuffleStartAnim(0x210), mShuffleAnim(0x20f), mDealStartAnim(0x20c), mDealAnim(0x209),
      mDealEndAnim(0x20a), mDealEnd2Anim(0x20b), mDealWaitAnim(0x20d), mThrowAnim(0x211),
      mWinStartAnim(0x215), mWinAnim(0x213), mWinEndAnim(0x214), mLoseAnim(0x20e) {
    mModelData = 0;
    mTableData = 0;
    mShuffleSoundPlayed = 0;
    mDealSoundPlayed = 0;
}

/* ---- Dealer::~Dealer ---- */
/* Releases every file; the members' destructors then run in reverse
   declaration order: the thirteen animations, the two model files, the
   walker, the table and the model. */
Dealer::~Dealer() {
    mModelFile.Release();
    mTableFile.Release();
    mWaitAnim.Release();
    mShuffleStartAnim.Release();
    mShuffleAnim.Release();
    mDealStartAnim.Release();
    mDealAnim.Release();
    mDealEndAnim.Release();
    mDealEnd2Anim.Release();
    mDealWaitAnim.Release();
    mThrowAnim.Release();
    mWinStartAnim.Release();
    mWinAnim.Release();
    mWinEndAnim.Release();
    mLoseAnim.Release();
}

int Dealer::Load() {
    void *file;
    BMD_Material *materials;

    dExtShadowModel_c::CleanAll();
    mModelData = dExtSimpleModel_c::LoadFile(mModelFile);
    mTableData = dExtSimpleModel_c::LoadFile(mTableFile);
    file = mModelData;
    if (file == 0 || mModel.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    dExtFrameCtrl_c::LoadFile(mWaitAnim);
    dExtFrameCtrl_c::LoadFile(mShuffleStartAnim);
    dExtFrameCtrl_c::LoadFile(mShuffleAnim);
    dExtFrameCtrl_c::LoadFile(mDealStartAnim);
    dExtFrameCtrl_c::LoadFile(mDealAnim);
    dExtFrameCtrl_c::LoadFile(mDealEndAnim);
    dExtFrameCtrl_c::LoadFile(mDealEnd2Anim);
    dExtFrameCtrl_c::LoadFile(mDealWaitAnim);
    dExtFrameCtrl_c::LoadFile(mThrowAnim);
    dExtFrameCtrl_c::LoadFile(mWinStartAnim);
    dExtFrameCtrl_c::LoadFile(mWinAnim);
    dExtFrameCtrl_c::LoadFile(mWinEndAnim);
    dExtFrameCtrl_c::LoadFile(mLoseAnim);
    mModel.SetAnim(*(BCA_File *)mWaitAnim.filePtr, 0, 0, 0x800, 0);
    ResetMatrices();
    mPos.x = data_020a0ebc.x;
    mPos.y = data_020a0ebc.y;
    mPos.z = data_020a0ebc.z;
    mRot[0] = data_020a0edc.x;
    mRot[1] = data_020a0edc.y;
    mRot[2] = data_020a0edc.z;
    mDealCounter = 0;
    mModel.HideMaterial(1, 0);
    PlayIdle();
    file = mTableData;
    if (file == 0 || mTable.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    materials = mTable.data.materials;
    materials[0].flags &= ~1;
    materials[5].flags &= ~1;
    materials[4].flags |= 2;
    mWalker.Load();
    mLampTimer = 0;
    mLampPattern = 1;
    mLightAngle = 0;
    return 1;
}

void Dealer::Update() {
    if (mState[0] != 0) {
        (this->*(*(DealerState *)mState))();
    }
    mModel.Advance();
    if (ApproachLinear2(mLampTimer, 0, 1) != 0) {
        mLampPattern++;
        if (mLampPattern > 7)
            mLampPattern = 1;
        mLampTimer = sDealerLampTimes[mLampPattern];
    }
    mLightAngle += 0x400;
    mWalker.Update();
}

void Dealer::Draw() {
    Vector3 keyLight, spinLight;
    short angle;
    unsigned char lamp[3];
    int i;

    keyLight.x = 0;
    keyLight.y = 0;
    keyLight.z = -0xff8;
    spinLight.x = 0;
    spinLight.y = 0;
    spinLight.z = -0xff8;
    func_0203cd80(&keyLight, -0x1000);
    func_0203ccd4(&keyLight, 0x1000);

    angle = mLightAngle;
    func_0203ccd4(&spinLight, angle);

    G3_LightVector(GX_LIGHTID_0, keyLight.x, keyLight.y, keyLight.z);
    G3_LightVector(GX_LIGHTID_1, spinLight.x, spinLight.y, spinLight.z);

    func_02016a14(&mModel, 0x7fff);
    func_02016a04(&mModel, 0x4210);

    {
        short pattern = mLampPattern;
        /* Filled through a walking pointer with an if/else body: indexing the
           array, or a value-select, moves 3 words (measured). */
        unsigned char *p = lamp;
        for (i = 0; i < 3; i++) {
            if ((pattern >> i) & 1)
                *p = 0x1f;
            else
                *p = 0x14;
            p++;
        }
    }

    func_020169d8(&mTable, 0, (unsigned short)(lamp[0] | (lamp[1] << 5) | (lamp[2] << 10)));
    func_020169d8(&mTable, 4, 0x6318);
    func_020169d8(&mTable, 5, (unsigned short)(lamp[1] | (lamp[2] << 5) | (lamp[0] << 10)));

    mModel.Render(0);
    mWalker.Draw();
    mTable.Render(0);

    dExtShadowModel_c::RenderAll();
}

void Dealer::PlayIdle() {
    if (mHand == 1) {
        mModel.SetAnim(*(BCA_File *)mDealWaitAnim.filePtr, 0, 0, 0x800, 0);
        if (mDealSoundPlayed == 0) {
            Sound::PlayBank2_2D(0x13a);
            mDealSoundPlayed = 1;
        }
    } else {
        mModel.SetAnim(*(BCA_File *)mWaitAnim.filePtr, 0, 0, 0x800, 0);
    }
    *(StateSlot *)this = pmf_Dealer_StateIdle;
}

void Dealer::StateIdle() {}

int Dealer::IsIdle() {
    int *idle = pmf_Dealer_StateIdle_2;
    int fn = mState[0];
    int differs = 1;
    if (fn == idle[0]) {
        if (mState[1] == idle[1] || fn == 0)
            differs = 0;
    }
    return differs == 0;
}

int Dealer::IsIdleAtRest() {
    int *idle = pmf_Dealer_StateIdle_3;
    int fn = mState[0];
    int atRest = 0;
    int differs = 1;
    if (fn == idle[0] && (mState[1] == idle[1] || fn == 0))
        differs = 0;
    if (differs == 0 && mModel.file == mWaitAnim.filePtr)
        atRest = 1;
    return atRest;
}

void Dealer::StartShuffle(int, short count, s16 *counter) {
    if (mShuffleRound == 0) {
        mShuffleLeft = 4;
        mDealDoneAt = 3;
    } else {
        mShuffleLeft = 1;
        mDealDoneAt = 1;
    }
    if (ApproachLinear2(mShuffleRound, 5, 1) != 0) {
        mShuffleRound = 0;
    }
    mDealSoundPlayed = 0;
    mDealLeft = count;
    mDealCounter = counter;
    mModel.SetAnim(*(BCA_File *)mShuffleStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    *(StateSlot *)this = pmf_Dealer_StateShuffle;
}

void Dealer::StateShuffle() {
    if (mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        mShuffleSoundPlayed = 1;
    }

    if (mModel.file == mShuffleStartAnim.filePtr) {
        if (mModel.Finished())
            mModel.SetAnim(*(BCA_File *)mShuffleAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }

    if (mModel.WillHitFrame(0)) {
        if (mShuffleLeft-- <= 0) {
            StartDeal(mDealLeft, mDealCounter);
            return;
        }
    }

    if (!mModel.WillHitFrame(4))
        return;

    Sound::PlayBank2_2D(0x141);
    if (mShuffleLeft == mDealDoneAt) {
        dScMgBase_c *scene = gMgScene;
        if (scene)
            scene->OnDealDone();
    }
}

void Dealer::StartDeal(short count, s16 *counter) {
    mDealLeft = count;
    mDealCounter = counter;
    if (IsIdle() && mModel.file != mDealWaitAnim.filePtr) {
        mModel.SetAnim(*(BCA_File *)mShuffleStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    } else {
        mModel.SetAnim(*(BCA_File *)mDealStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    }
    *(StateSlot *)this = pmf_Dealer_StateDeal;
}

void Dealer::StateDeal() {
    BCA_File *anim = mModel.file;

    if (anim == mDealStartAnim.filePtr) {
        if (mModel.Finished()) {
            mModel.SetAnim(*(BCA_File *)mDealAnim.filePtr, 0, 0, 0x800, 0);
        }
    }

    anim = mModel.file;
    if (anim == mDealEndAnim.filePtr || anim == mDealEnd2Anim.filePtr) {
        if (mModel.Finished()) {
            mDealCounter = 0;
            PlayIdle();
        }
        return;
    }

    if (anim == mDealAnim.filePtr) {
        if (mModel.WillHitFrame(0)) {
            if (mDealLeft-- <= 0) {
                short state = mHand;
                if (state == 0) {
                    mModel.SetAnim(*(BCA_File *)mDealEndAnim.filePtr, 0, 0x40000000, 0x800, 0);
                    if (mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    mDealSoundPlayed = 1;
                    return;
                } else if (state == 2) {
                    mModel.SetAnim(*(BCA_File *)mDealEnd2Anim.filePtr, 0, 0x40000000, 0x800, 0);
                    if (mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    mDealSoundPlayed = 1;
                    return;
                } else {
                    PlayIdle();
                    return;
                }
            }
        }

        if (mDealCounter == 0)
            return;
        if (!mModel.WillHitFrame(5))
            return;
        Sound::PlayBank2_2D(0x142);
        *mDealCounter += 1;
        return;
    }

    if (anim == mShuffleStartAnim.filePtr) {
        if (mModel.Finished()) {
            mModel.SetAnim(*(BCA_File *)mDealStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
        }
    }
}

void Dealer::StartReveal(int count, s16 *counter) {
    mDealLeft = count;
    mDealCounter = counter;
    mModel.SetAnim(*(BCA_File *)mDealAnim.filePtr, 8, 0, 0x800, 0);
    *(StateSlot *)this = pmf_Dealer_StateReveal_2;
}

void Dealer::StateReveal() {
    if (mModel.file != mDealAnim.filePtr)
        goto other;

    if (mModel.WillHitFrame(0)) {
        if (mDealLeft-- <= 0) {
            int hand;
            mDealCounter = 0;
            hand = mHand;
            if (hand == 0) {
                mModel.SetAnim(*(BCA_File *)mDealEndAnim.filePtr, 0, 0x40000000, 0x800, 0);
                if (mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                mDealSoundPlayed = 1;
                return;
            } else if (hand == 2) {
                mModel.SetAnim(*(BCA_File *)mDealEnd2Anim.filePtr, 0, 0x40000000, 0x800, 0);
                if (mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                mDealSoundPlayed = 1;
                return;
            } else {
                PlayIdle();
                return;
            }
        }
    }

    if (mDealCounter == 0)
        return;
    if (!mModel.WillHitFrame(5))
        return;
    Sound::PlayBank2_2D(0x142);
    *mDealCounter += 1;
    return;

other:
    if (mModel.file == (BCA_File *)mDealEndAnim.filePtr || mModel.file == mDealEnd2Anim.filePtr) {
        if (mModel.Finished())
            PlayIdle();
    }
}

int Dealer::IsBusy() {
    int *record = pmf_Dealer_StateDeal_2;
    int busy = 1;
    int notDealing = busy;
    int fn = mState[0];
    if (fn == record[0]) {
        if (mState[1] == record[1] || fn == 0)
            notDealing = 0;
    }
    if (notDealing != 0) {
        int notRevealing = 1;
        record = pmf_Dealer_StateReveal;
        if (fn == record[0]) {
            if (mState[1] == record[1] || fn == 0)
                notRevealing = 0;
        }
        if (notRevealing)
            busy = 0;
    }
    return busy;
}

/* Dealer::IsAnimDone @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b dExtFrameCtrl_c::Finished(). */

int Dealer::IsAnimDone() {
    return mModel.Finished();
}

void Dealer::StartSwap() {
    mModel.SetAnim(*(BCA_File *)mThrowAnim.filePtr, 0, 0x40000000, 0x800, 0);
    mModel.HideMaterial(2, 0);
    mModel.ShowMaterial(1, 0);
    *(StateSlot *)this = pmf_Dealer_StateSwap;
}

void Dealer::StateSwap() {
    if (mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        mShuffleSoundPlayed = 1;
    }
    /* 0x6c is the dExtFrameCtrl_c inside mModel (its +0x50). */
    if (mModel.file == (BCA_File *)mThrowAnim.filePtr && mModel.Finished()) {
        mModel.HideMaterial(1, 0);
        mModel.ShowMaterial(2, 0);
        return;
    }
    if (mModel.WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}

void Dealer::PlayLose() {
    if (mHand == 1) {
        mModel.SetAnim(*(BCA_File *)mDealEndAnim.filePtr, 0, 0x40000000, 0x800, 0);
    } else {
        Sound::PlayBank2_2D(0x13c);
        mModel.SetAnim(*(BCA_File *)mLoseAnim.filePtr, 0, 0x40000000, 0x800, 0);
    }
    *(StateSlot *)this = pmf_Dealer_StateLose;
}

void Dealer::StateLose() {
    if (!mModel.Finished())
        return;
    if (mModel.file == mLoseAnim.filePtr) {
        PlayIdle();
        return;
    }
    Sound::PlayBank2_2D(0x13c);
    mHand = 0;
    mModel.SetAnim(*(BCA_File *)mLoseAnim.filePtr, 0, 0x40000000, 0x800, 0);
}

void Dealer::PlayWin() {
    Sound::PlayBank2_2D(0x13b);
    mModel.SetAnim(*(BCA_File *)mWinStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    mShuffleLeft = 5;
    *(StateSlot *)this = pmf_Dealer_StateWin;
}

void Dealer::StateWin() {
    if (mModel.file == (BCA_File *)mWinStartAnim.filePtr && mModel.Finished()) {
        mModel.SetAnim(*(BCA_File *)mWinAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }
    if (mModel.file == (BCA_File *)mWinAnim.filePtr && mModel.WillHitFrame(0)) {
        mShuffleLeft -= 1;
        if (mShuffleLeft == 0) {
            mModel.SetAnim(*(BCA_File *)mWinEndAnim.filePtr, 0, 0x40000000, 0x800, 0);
            return;
        }
    }
    if (mModel.file == (BCA_File *)mWinEndAnim.filePtr && mModel.Finished()) {
        PlayIdle();
    }
}

void Dealer::ResetMatrices() {
    Matrix4x3 identity = sDealerIdentityMtx;
    mModel.mat4x3 = sDealerIdentityMtx;
    mTable.mat4x3 = identity;
}

/* Camera_InitDealerView at 0x020c0aa8
 *
 * MgCamera preset: sets the look-at point, position and fov angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

void Camera_InitDealerView(MgCamera *camera) {
    camera->mLookAt.x = 0x200;
    camera->mLookAt.y = 0x9600;
    camera->mLookAt.z = -0x22f00;
    camera->mPos.x = -0x100;
    camera->mPos.y = 0x10500;
    camera->mPos.z = 0x1ee00;
    camera->mFovAngle = 0xbb0;
    Camera_UpdateMatrices(camera);
}

/* ---- MascotWalker::MascotWalker ---- */
/* The file handles, the model and the shadow are members, so the compiler
   constructs them in declaration order before the body runs. */
MascotWalker::MascotWalker() : mModelFile(0x205), mPauseAnimFile(0x206), mWalkAnimFile(0x207) {
    mPathIndex = 0;
    mHeadAngle = 0;
    mPaused = 0;
}

/* ---- MascotWalker::~MascotWalker ---- */
/* Releases the three files; the members' destructors then run in reverse
   declaration order: shadow, model, walk, pause, model file. */
MascotWalker::~MascotWalker() {
    mModelFile.Release();
    mWalkAnimFile.Release();
    mPauseAnimFile.Release();
}

void MascotWalker::Load() {
    {
        BMD_File *bmd = dExtSimpleModel_c::LoadFile(mModelFile);
        mModel.SetFile(bmd, 1, -1);
    }
    dExtFrameCtrl_c::LoadFile(mWalkAnimFile);
    dExtFrameCtrl_c::LoadFile(mPauseAnimFile);
    mModel.SetAnim(*(BCA_File *)mPauseAnimFile.filePtr, 0, 0, 0x800, 0);
    mModel.SetPolygonID(1);
    mShadow.InitCylinder();
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mPos.x = data_020a0ebc.x;
    mPos.y = data_020a0ebc.y;
    mPos.z = data_020a0ebc.z;
    mAngleX = data_020a0edc.x;
    mAngleY = data_020a0edc.y;
    mAngleZ = data_020a0edc.z;
    StartNextPath();
}

void MascotWalker::Update() {
    (this->*(*(MascotWalkerState *)mState))();

    mModel.Advance();

    if (mPaused != 0) {
        Vector3 target;
        target.x = sMascotWalkerOffset.x;
        target.y = sMascotWalkerOffset.y;
        target.z = sMascotWalkerOffset.z;
        short turn;
        int angle;
        angle = Vec3_HorzAngle(&mPos, &target);
        turn = (angle - mAngleY);
        if (turn < -0x3000)
            turn = -0x3000;
        else if (turn > 0x3000)
            turn = 0x3000;
        UpdateAngle(mHeadAngle, turn, 8, 0x200);

        {
            int *state = mState;
            int *walk = pmf_MascotWalker_StateWalk_2;
            if (state[0] == walk[0]) {
                if (state[1] == walk[1])
                    return;
                if (mState[0] == 0)
                    return;
            }
        }
        turn = (angle - turn);
        UpdateAngle(mAngleY, turn, 8, 0x200);
    } else {
        UpdateAngle(mHeadAngle, 0, 8, 0x200);
    }
}

void MascotWalker::Draw() {
    UpdateMatrix();
    mShadow.InitModel(&mModel.mat4x3, 0x3c000, 0x14000, 0x3c000, 0xc);
    /* dExtSimpleModel_c's own render, called directly (0x02016b78), as Mascot::Draw does. */
    mModel.dExtSimpleModel_c::Render(0);
}

void MascotWalker::UpdateMatrix() {
    mModel.UpdateVerts();
    /* Bone 6 takes the head angle. */
    data_020a0e68 = mModel.data.transforms[6];
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mHeadAngle);
    mModel.data.transforms[6] = data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mModel.mat4x3 = data_020a0e68;
}

void MascotWalker::StartNextPath() {
    Vector3 toNext;
    Vector3 target;
    mModel.SetAnim(*(BCA_File *)mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    if (mPathIndex < 3) {
        mPathIndex += 1;
    } else {
        mPathIndex = 0;
    }
    mPath = &sMascotWalkerPaths[mPathIndex];
    mPathPoint = 0;
    {
        Vector3 *point = &mPath->points[mPathPoint];
        mPos.x = point->x;
        mPos.y = point->y;
        mPos.z = point->z;
    }
    mPathPoint += 1;
    Vec3_Sub(&toNext, &mPos, &mPath->points[mPathPoint]);
    mDir.x = toNext.x;
    mDir.y = toNext.y;
    mDir.z = toNext.z;
    if (NormalizeVec3IfNonZero(&mDir) == 0) {
        mDir.x = 0xb50;
        mDir.y = 0xb50;
        mDir.z = 0;
    }
    {
        Vector3 *point = &mPath->points[mPathPoint];
        target.x = point->x;
        target.y = point->y;
        target.z = point->z;
    }
    mTargetAngle = Vec3_HorzAngle(&mPos, &target);
    {
        int fn = pmf_MascotWalker_StateWalk[0];
        int adj = pmf_MascotWalker_StateWalk[1];
        /* The no-op ternary is load-bearing: storing the two words directly
           moves three words (measured; the ROM keeps the conditional). */
        mState[0] = adj ? fn : fn;
        mState[1] = adj;
    }
}

void MascotWalker::StateWalk() {
    int dx, dz;
    int stepX, stepZ;
    int doneX, doneZ;

    UpdateAngle(mAngleY, mTargetAngle, 8, 0x100);

    dx = mDir.x;
    if (dx < 0)
        dx = -dx;
    stepX = FX_MUL(dx, 0x1a9);

    dz = mDir.z;
    if (dz < 0)
        dz = -dz;

    doneX = ApproachLinear(mPos.x, mPath->points[mPathPoint].x, stepX);

    stepZ = FX_MUL(dz, 0x1a9);

    doneZ = ApproachLinear(mPos.z, mPath->points[mPathPoint].z, stepZ);

    if (doneX != 0 && doneZ != 0) {
        if (mPathPoint < mPath->count - 1) {
            Vector3 dir;
            Vector3 next;

            mPathPoint++;

            Vec3_Sub(&dir, &mPos, &mPath->points[mPathPoint]);

            mDir.x = dir.x;
            mDir.y = dir.y;
            mDir.z = dir.z;

            if (NormalizeVec3IfNonZero(&mDir) == 0) {
                mDir.x = 0xb50;
                mDir.y = 0xb50;
                mDir.z = 0;
            }

            {
                Vector3 *point = &mPath->points[mPathPoint];
                next.x = point->x;
                next.y = point->y;
                next.z = point->z;
            }

            mTargetAngle = Vec3_HorzAngle(&mPos, &next);
            return;
        }

        StartNextPath();
        return;
    }

    if (ApproachLinear2(mStateTimer, 0, 1) == 0)
        return;

    if (mModel.WillHitFrame(0) == 0)
        return;

    StartPause();
}

void MascotWalker::StartPause() {
    mModel.SetAnim(*(BCA_File *)mPauseAnimFile.filePtr, 8, 0, 0x800, 0);
    mStateTimer = 0x78;
    mPaused = 1;
    *(StateSlot *)mState = pmf_MascotWalker_StatePause;
}

void MascotWalker::StatePause() {
    if (ApproachLinear2(mStateTimer, 0, 1) == 0)
        return;
    mModel.SetAnim(*(BCA_File *)mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    /* Walk for 0x258..0x4af frames: a random 0..0xfff scaled into 0x258. */
    int roll = RandomIntInternal(&data_0209e650.value);
    int frac = ((unsigned int)(roll & 0x7fffffff) >> 0x13);
    mStateTimer = (((frac * 0x258) >> 12) + 0x258);
    mPaused = 0;
    *(StateSlot *)mState = pmf_MascotWalker_StateWalk_3;
}

/* Camera_UpdateMatrices at 0x020c0134
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

void Camera_UpdateMatrices(MgCamera *camera) {
    Vector3 up;
    Vector3 forward;
    Vector3 right;
    Vector3 scratch0;
    Vector3 scratch1;

    up.x = 0;
    up.y = 0x1000;
    up.z = 0;

    VEC_Subtract((Vector3 *)&camera->mLookAt, (Vector3 *)&camera->mPos, &forward);
    if (NormalizeVec3IfNonZero(&forward)) {
        func_0203cebc(&scratch0, &right, &forward, &up);
        if (NormalizeVec3IfNonZero(&right)) {
            func_0203cebc(&scratch1, &up, &right, &forward);
        } else {
            up.x = 0;
            up.y = 0;
            up.z = 0x1000;
        }
    }

    G3i_PerspectiveW_(FX_SinIdx(camera->mFovAngle >> 4), FX_CosIdx(camera->mFovAngle >> 4), 0x1555,
                      0x1000, 0x1388000, 0x1000, 1, (Matrix4x3 *)camera->mProjMtx);

    G3i_LookAt_((Vector3 *)&camera->mPos, &up, (Vector3 *)&camera->mLookAt, true,
                (Matrix4x3 *)&camera->mViewMtx);

    CopyToViewMat((Matrix4x3 *)&camera->mViewMtx);

    data_0209f43c.Func_020156DC(0x1555, camera->mFovAngle, 0x1000, 0x1388000);
}


void Camera_ProjectToScreen(MgCamera *camera, void *pos, int *screenX, int *screenY) {
    int view[3];
    MTX_MultVec43((Vector3 *)pos, (Matrix4x3 *)&camera->mViewMtx, (Vector3 *)view);
    int fovIdx = camera->mFovAngle >> 4;
    int tanFov = FX_Div(FX_SinIdx(fovIdx), FX_CosIdx(fovIdx));
    int height = FX_MUL(tanFov, -view[2]);
    int fracY = FX_Div(view[1], height);
    int pixelY = FX_MUL(fracY, 0x5f800u);
    screenY[0] = -((pixelY + (int)0xfffa0800) >> 0xc);
    int width = FX_MUL(height, 0x1555u);
    int fracX = FX_Div(view[0], width);
    int pixelX = FX_MUL(fracX, 0x7f800u);
    screenX[0] = (pixelX + 0x7f800) >> 0xc;
}


void Camera_ProjectToScreen16(MgCamera *camera, void *pos, short *screen) {
    int view[3];
    MTX_MultVec43((Vector3 *)pos, (Matrix4x3 *)&camera->mViewMtx, (Vector3 *)view);
    int fovIdx = camera->mFovAngle >> 4;
    int tanFov = FX_Div(FX_SinIdx(fovIdx), FX_CosIdx(fovIdx));
    int height = FX_MUL(tanFov, -view[2]);
    int fracY = FX_Div(view[1], height);
    int pixelY = FX_MUL(fracY, 0x5f800u);
    screen[1] = -((pixelY + (int)0xfffa0800) >> 0xc);
    int width = FX_MUL(height, 0x1555u);
    int fracX = FX_Div(view[0], width);
    int pixelX = FX_MUL(fracX, 0x7f800u);
    screen[0] = (pixelX + 0x7f800) >> 0xc;
}
