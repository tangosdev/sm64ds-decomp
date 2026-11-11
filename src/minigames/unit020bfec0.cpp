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
 * - The helpers keep their address names: the ROM carries no symbol table
 *   for them.
 * - The helpers the scenes call keep the parameter spelling those scenes
 *   declare (a char* or an int*), because retyping the definition alone
 *   would make the two sides disagree. Each casts once and then works on
 *   the record.
 * - The IsIdle / IsBusy tests compare a state record word by word: the
 *   scenes hand them the record's two words, and a member-pointer compare
 *   would not reproduce it.
 */

#include "types.h"
#include "dFdBrightness_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/hw/registers.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "PlayerInput.h"
#include "G3i.h"
#include "dClipper.h"
#include "MascotWalker.h"
#include "Presenter.h"
#include "Dealer.h"
#include "Mascot.h"
#include "FlowerFace.h"
#include "BMD_File.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmTexSRT_c.h"
#include "cstd.h"
#include "G2.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "dScMgBase_c.h"
#include "MWCPlusLib.h"
#include "LevelRng.h"
#include "MgCamera.h"
#include "math/Quaternion.h"
/* Typed views of the records the helpers the scenes call receive as char* or
   int*. Kept as casts rather than parameters: the scene translation units
   declare those helpers with the flat pointer type, and C++ function
   overloading would not let both spellings name one symbol. */
#define WALKER(p) ((MascotWalker *)(p))
#define PRESENTER(p) ((Presenter *)(p))
#define DEALER(p) ((Dealer *)(p))
#define MASCOT(p) ((Mascot *)(p))
#define FLOWERFACE(p) ((FlowerFace *)(p))
#define PETAL(p) ((FallingPetal *)(p))

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

/* Presenter_PlayIdle copies its state pair through two volatile word reads;
   a plain struct copy moves two words and costs the function its bytes. */
struct VolatileStateSlot {
    void *lo, *hi;
};

/* A quaternion as four raw words. The identity in data_02092768 is copied
   into a scratch quaternion with a 16-byte block move; spelling the four
   members out (math/Quaternion.h) makes mwcc scalarise that copy and costs
   FallingPetal_Update its bytes (measured). */
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

extern "C" {
/* local extern: no shared math header declares MulVec3Mat4x3 or MulVec3Mat3x3. */
void MulVec3Mat4x3(Vector3 *, Matrix4x3 *, Vector3 *);
void MulVec3Mat3x3(void *, void *, void *);
// local extern: no shared header declares Matrix4x3_FromQuaternion yet.
void Matrix4x3_FromQuaternion(const Quaternion *, Matrix4x3 *);
/* local extern: no shared header declares these five vector helpers yet. */
int DotVec3(const Vector3 *, const Vector3 *);
int NormalizeVec3IfNonZero(Vector3 *);
void Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *);
short Vec3_VertAngle(const Vector3 *, const Vector3 *);
void Vec3_MulScalar(Vector3 *, const Vector3 *, int);
void Vec3_MulScalarInPlace(Vector3 *, int);
void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);
void Quaternion_FromVector3(void *, void *, void *);
void Quaternion_Normalize(void *);
void Quaternion_SLerp(void *, void *, int, void *);
void Quaternion_Multiply(void *, void *, void *);
void *SharedFilePtr_Construct_TexSeq(void *, unsigned int);
int SharedFilePtr_Destruct_Anim(void *);
int SharedFilePtr_Destruct_TexSeq(void *);
void __cxa_vec_cleanup(void *, unsigned int, unsigned int, dtor_t);
void __cxa_vec_ctor(void *, unsigned int, unsigned int, ctor_t, dtor_t);
void func_020169d8(void *, int, unsigned int);
void func_02016a04(void *, int);
void func_02016a14(void *, int);
int func_02017ab4(void *);
void *func_02017acc(void *, unsigned int);
unsigned int func_02012174(unsigned int, unsigned int);
void func_0203ccd4(void *, short);
void func_0203cd80(void *, short);
void func_0203cebc(void *, void *, void *, void *);
void MascotWalker_StatePause(MascotWalker *walker);
void MascotWalker_StartPause(MascotWalker *walker);
void MascotWalker_StateWalk(MascotWalker *walker);
void MascotWalker_StartNextPath(MascotWalker *walker);
void MascotWalker_UpdateMatrix(MascotWalker *walker);
void MascotWalker_Draw(char *t);
void MascotWalker_Update(MascotWalker *walker);
void MascotWalker_Load(MascotWalker *walker);
void Dealer_ResetMatrices(char *c);
void Dealer_StateWin(Dealer *dealer);
void Dealer_PlayWin(Dealer *dealer);
void Dealer_StateLose(Dealer *dealer);
void Dealer_PlayLose(char *c);
void Dealer_StateSwap(Dealer *dealer);
void Dealer_StartSwap(int *t);
int Dealer_IsAnimDone(Dealer *dealer);
int Dealer_IsBusy(Dealer *dealer);
void Dealer_StateReveal(Dealer *dealer);
void Dealer_StartReveal(Dealer *dealer, int count, s16 *counter);
void Dealer_StateDeal(Dealer *dealer);
void Dealer_StartDeal(Dealer *dealer, short count, s16 *counter);
void Dealer_StateShuffle(Dealer *dealer);
void Dealer_StartShuffle(Dealer *dealer, int, short count, s16 *counter);
int Dealer_IsIdleAtRest(Dealer *dealer);
int Dealer_IsIdle(Dealer *dealer);
void Dealer_StateIdle(void);
void Dealer_PlayIdle(char *c);
void Dealer_Draw(void *self);
void Dealer_Update(Dealer *dealer);
int Dealer_Load(char *c);
void Mascot_StopLooking(int *p);
void Mascot_LookAt(char *c, int *src);
void Mascot_UpdateMatrix(Mascot *mascot);
void Mascot_Draw(Mascot *c);
void Mascot_Update(Mascot *mascot);
void Mascot_Load(char *c);
void Presenter_UpdateMatrix(char *c);
void Presenter_StateSurprisedThenIdle(Presenter *presenter);
void Presenter_PlayCheerLoop(Presenter *presenter);
void Presenter_StateLoseStart(Presenter *presenter);
void Presenter_PlayLose(char *c);
void Presenter_StateSurprisedThenSmile(Presenter *presenter);
void Presenter_PlayCheer(Presenter *presenter);
void Presenter_StateMistake(Presenter *presenter);
void Presenter_PlayMiss(char *c);
void Presenter_StateWin(Presenter *presenter);
void Presenter_PlayMatch(Presenter *presenter);
void Presenter_StateIdle(Presenter *presenter);
void Presenter_PlayIdle(char *c);
void Presenter_ResetPose(char *c);
int Presenter_IsAnimDone(Presenter *presenter);
int Presenter_IsIdle(Presenter *presenter);
void Presenter_Draw(Presenter *presenter);
void Presenter_Update(Presenter *presenter);
void Presenter_TrackStylus(Presenter *presenter);
int Presenter_Load(char *c);
void FallingPetal_UpdateMatrix(char *c);
void FallingPetal_Draw(char *c);
void FallingPetal_Update(FallingPetal *petal);
void FallingPetal_Spawn(FallingPetal *petal, const VecFx32 *pos, const VecFx32 *vel);
void FallingPetal_SetModel(FallingPetal *petal, BMD_File *model);
void FlowerFace_StateHate(void);
void FlowerFace_PlayHate(char *p);
void FlowerFace_StateLike(void);
void FlowerFace_PlayLike(char *p);
void FlowerFace_StatePick(FlowerFace *face);
void FlowerFace_PlayPetalPulled(char *c);
void FlowerFace_StateIdle(void);
void FlowerFace_PlayIdle(FlowerFace *face);
int FlowerFace_IsIdle(int *c);
void FlowerFace_Reset(FlowerFace *face);
void FlowerFace_Draw(FlowerFace *face);
void FlowerFace_Update(char *c);
int FlowerFace_Load(char *c);

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
extern VolatileStateSlot pmf_Presenter_StateIdle_2;
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
}

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
    SharedFilePtr_Destruct_Anim(this);
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
    SharedFilePtr_Destruct_Anim(this);
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
    SharedFilePtr_Destruct_Anim(this);
}
inline PresenterTexSeqFileHandle::PresenterTexSeqFileHandle(u32 fileID) {
    SharedFilePtr_Construct_TexSeq(this, fileID);
}
inline PresenterTexSeqFileHandle::~PresenterTexSeqFileHandle() {
    SharedFilePtr_Destruct_TexSeq(this);
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
    SharedFilePtr_Destruct_Anim(this);
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

extern "C" int FlowerFace_Load(char *c) {
    FlowerFace *face = FLOWERFACE(c);
    void *bmd = dExtSimpleModel_c::LoadFile(face->mModelFile);
    face->mModel.SetFile((BMD_File *)bmd, 1, -1);
    dExtFrameCtrl_c::LoadFile(face->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(face->mPickAnim);
    dExtFrameCtrl_c::LoadFile(face->mLikeAnim);
    dExtFrameCtrl_c::LoadFile(face->mHateAnim);
    FlowerFace_PlayIdle(face);
    BMD_File *petalFile = (BMD_File *)dExtSimpleModel_c::LoadFile(face->mPetalFile);
    for (int i = 0; i < 22; i++) {
        FallingPetal_SetModel(&face->mPetals[i], petalFile);
        face->mPetals[i].mActive = 0;
    }
    return 1;
}

extern "C" {
void FlowerFace_Update(char *c) {
    FlowerFace *face = FLOWERFACE(c);
    (face->*(*(FlowerFaceState *)face))();
    face->mModel.Advance();
    int i = 0;
    do {
        if (face->mPetals[i].mActive != 0)
            FallingPetal_Update(&face->mPetals[i]);
        i++;
    } while (i < 22);
}
}



extern "C" void FlowerFace_Draw(FlowerFace *face) {
    Vector3 light;
    int i;

    light.x = 0;
    light.y = 0;
    light.z = 0xfffff008;
    func_0203cd80(&light, 0);
    func_0203ccd4(&light, 0x600);

    REG_LIGHT_VECTOR = (((short)light.x >> 3) & 0x3ff) | ((((short)light.y >> 3) & 0x3ff) << 10) |
                       ((((short)light.z >> 3) & 0x3ff) << 20);

    func_02016a14(&face->mModel, 0x2bff);
    func_02016a04(&face->mModel, 0x1211);

    face->mModel.mat4x3 = data_ov006_0213aee8;

    face->mModel.Render(0);

    for (i = 0; i < 22; i++) {
        if (face->mPetals[i].mActive != 0)
            FallingPetal_Draw((char *)&face->mPetals[i]);
    }
}

extern "C" {
void FlowerFace_Reset(FlowerFace *face) {
    for (int i = 0; i < 22; i++)
        face->mPetals[i].mActive = 0;
    FlowerFace_PlayIdle(face);
}
}

extern "C" {
int FlowerFace_IsIdle(int *c) {
    int *idle = pmf_FlowerFace_StateIdle_2;
    int fn = c[0];
    /* The 0/1 result goes through a flag: returning the test directly emits a
       shorter function than the ROM's (measured). */
    int differs = 1;
    if (fn == idle[0]) {
        if (c[1] == idle[1] || fn == 0)
            differs = 0;
    }
    return differs == 0;
}
}

extern "C" {
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
}

extern "C" {
void FlowerFace_PlayIdle(FlowerFace *face) {
    face->mModel.SetAnim((BCA_File *)face->mWaitAnim.filePtr, 0, 0x800, 0);
    *(StateSlot *)face->mState = pmf_FlowerFace_StateIdle;
}
}

extern "C" {
void FlowerFace_StateIdle(void) {}
}

extern "C" {


void FlowerFace_PlayPetalPulled(char *c) {
    FlowerFace *face = FLOWERFACE(c);
    int i;
    /* The spawn point and velocity, and the copies Spawn is handed. */
    struct PetalSpawn {
        VecFx32 pos, vel;
        VecFx32 pos2, vel2;
    } b;

    face->mModel.SetAnim((BCA_File *)face->mPickAnim.filePtr, 0x40000000, 0x800,
                                  0);

    face->mModel.currFrame = 0; /* rewind */

    for (i = 0; i < 0x16; i++) {
        if (face->mPetals[i].mActive == 0) {
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
            FallingPetal_Spawn(&face->mPetals[i], &b.pos2, &b.vel2);
            break;
        }
    }

    *(StateSlot *)face = pmf_FlowerFace_StatePick;
}
}

extern "C" {
void FlowerFace_StatePick(FlowerFace *face) {
    if (face->mModel.Finished() == 0)
        return;
    FlowerFace_PlayIdle(face);
}
}

extern "C" {
void FlowerFace_PlayLike(char *p) {
    FlowerFace *face = FLOWERFACE(p);
    face->mModel.SetAnim((BCA_File *)face->mLikeAnim.filePtr, 0x40000000, 0x800,
                                  0);
    *(StateSlot *)face->mState = pmf_FlowerFace_StateLike;
}
}

extern "C" {
void FlowerFace_StateLike(void) {}
}

extern "C" {
void FlowerFace_PlayHate(char *p) {
    FlowerFace *face = FLOWERFACE(p);
    face->mModel.SetAnim((BCA_File *)face->mHateAnim.filePtr, 0x40000000, 0x800,
                                  0);
    *(StateSlot *)face->mState = pmf_FlowerFace_StateHate;
}
}

extern "C" {
void FlowerFace_StateHate(void) {}
}

extern "C" {
void FallingPetal_SetModel(FallingPetal *petal, BMD_File *model) {
    petal->mModel.SetFile(model, 1, -1);
    petal->mActive = 0;
}
}

extern "C" {

void FallingPetal_Spawn(FallingPetal *petal, const VecFx32 *pos, const VecFx32 *vel) {
    /* Every petal spins about +Y. Constructed on the first spawn: mwcc emits
       the guard test, the three stores and the ~Vector3() registration here. */
    static Vector3 sAxis(0, 0x1000, 0);

    petal->mPos.x = pos->x;
    petal->mPos.y = pos->y;
    petal->mPos.z = pos->z;
    petal->mVel.x = vel->x;
    petal->mVel.y = vel->y;
    petal->mVel.z = vel->z;
    petal->mAccel.x = 0;
    petal->mAccel.y = -0x40;
    petal->mAccel.z = 0;
    petal->mLife = 0xb4;
    petal->mAxis.x = sAxis.x;
    petal->mAxis.y = sAxis.y;
    petal->mAxis.z = sAxis.z;
    func_0203cd80(&petal->mAxis, (short)RandomIntInternal(&data_0209e650.value));
    petal->mRot[0] = data_02092768.w[0];
    petal->mRot[1] = data_02092768.w[1];
    petal->mRot[2] = data_02092768.w[2];
    petal->mRot[3] = data_02092768.w[3];
    Quaternion_FromVector3(petal->mRot, &sAxis, &petal->mAxis);
    petal->mActive = 1;
}
}

extern "C" {


void FallingPetal_Update(FallingPetal *petal) {
    Vector3 sway;
    Vector3 flow;
    Vector3 drag;
    Vector3 dir;
    QuaternionBlock target;
    QuaternionBlock turn;
    int phase;
    int wave;
    int dot;

    if (ApproachLinear2(petal->mLife, 0, 1) != 0) {
        petal->mActive = 0;
        return;
    }
    AddVec3(&petal->mPos, &petal->mVel, &petal->mPos);
    petal->mAccel.y = -0x40;
    AddVec3(&petal->mVel, &petal->mAccel, &petal->mVel);
    phase = ((petal->mPos.y >> 1) << 16) >> 20;
    wave = FX_SinIdx(phase);
    sway.x = wave;
    sway.y = 0;
    sway.z = wave;
    Vec3_MulScalarInPlace(&sway, 0x400);
    Vec3_Add(&flow, &petal->mVel, &sway);
    dot = DotVec3(&petal->mAxis, &flow);
    Vec3_MulScalar(&drag, &petal->mAxis, dot);
    Vec3_MulScalarInPlace(&drag, 0x300);
    SubVec3(&petal->mVel, &drag, &petal->mVel);
    dir.x = flow.x;
    dir.y = flow.y;
    dir.z = flow.z;
    if (NormalizeVec3IfNonZero(&dir) == 0)
        return;
    turn = data_02092768;
    Quaternion_FromVector3(&target, &petal->mAxis, &dir);
    Quaternion_SLerp(&turn, &target, 0x200, &turn);
    Quaternion_Normalize(&turn);
    Quaternion_Multiply(petal->mRot, &turn, petal->mRot);
}
}


extern "C" void FallingPetal_Draw(char *c) {
    FallingPetal *petal = PETAL(c);
    FallingPetal_UpdateMatrix(c);
    /* Stored z, y, x: the ROM's order (measured; x, y, z moves two words). */
    Vector3 scale;
    scale.z = 0x1000;
    scale.y = 0x1000;
    scale.x = 0x1000;
    petal->mModel.Render(&scale);
}

extern "C" {
void FallingPetal_UpdateMatrix(char *c) {
    FallingPetal *petal = PETAL(c);
    Matrix4x3 rot;
    Matrix4x3_FromQuaternion(reinterpret_cast<const Quaternion *>(petal->mRot), &rot);
    MulVec3Mat3x3(&sPetalUpAxis, &rot, &petal->mAxis);
    Matrix4x3_FromTranslation(&data_020a0e68, petal->mPos.x, petal->mPos.y, petal->mPos.z);
    MulMat4x3Mat4x3(&rot, &data_020a0e68, &data_020a0e68);
    petal->mModel.mat4x3 = data_020a0e68;
}
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

extern "C" {

int Presenter_Load(char *c) {
    Presenter *presenter = PRESENTER(c);
    void *faceFile;
    void *backFile;

    faceFile = dExtSimpleModel_c::LoadFile(presenter->mFaceModel);
    backFile = dExtSimpleModel_c::LoadFile(presenter->mBackModel);
    dExtFrameCtrl_c::LoadFile(presenter->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mMistakeAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mWinAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mLoseAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mLoseStartAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mLoseEndAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mSmileAnim);
    dExtFrameCtrl_c::LoadFile(presenter->mSurprisedAnim);
    dExtAnmTexPat_c::LoadFile(presenter->mWaitTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mMistakeTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mWinTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mLoseTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mLoseStartTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mLoseEndTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mSmileTexSeq);
    dExtAnmTexPat_c::LoadFile(presenter->mSurprisedTexSeq);

    if (!presenter->mFace.SetFile((BMD_File *)faceFile, 1, -1))
        return 0;

    presenter->mFace.SetMaterialFlags(2);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)presenter->mWaitTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)presenter->mMistakeTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)presenter->mWinTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)presenter->mLoseTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)presenter->mLoseStartTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)presenter->mLoseEndTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)presenter->mSmileTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)presenter->mSurprisedTexSeq.filePtr);

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterIdleTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterMissTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterMatchTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterLoseLoopTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterLoseTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterTextureAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterCheerTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterSmileTexAnim);

    Presenter_PlayIdle(c);
    presenter->mPos.x = 0;
    presenter->mPos.y = 0;
    presenter->mPos.z = 0;
    Presenter_UpdateMatrix(c);

    if (!presenter->mBack.SetFile((BMD_File *)backFile, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    presenter->mBack.mat4x3 = data_020a0e68;
    return 1;
}
}

extern "C" {

/* Eases the four mSway words (Presenter_Draw adds them to two face
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

void Presenter_TrackStylus(Presenter *presenter) {
    if (presenter->mTouchLocked != 0) {
        int x = presenter->mTouchX;
        int y = presenter->mTouchY;
        PRESENTER_TRACK(presenter, x, y);
        return;
    }
    if (Presenter_IsIdle(presenter) != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int x = gTouchX[i * 4];
            int y = gTouchY[i * 4];
            PRESENTER_TRACK(presenter, x, y);
            return;
        }
    }
    presenter->mSway[0] = FX_MUL(presenter->mSway[0], 0xE00);
    presenter->mSway[1] = FX_MUL(presenter->mSway[1], 0xE00);
    presenter->mSway[2] = FX_MUL(presenter->mSway[2], 0xE00);
    presenter->mSway[3] = FX_MUL(presenter->mSway[3], 0xE00);
}
}

extern "C" {
void Presenter_Update(Presenter *presenter) {
    Presenter_TrackStylus(presenter);
    if (*(PresenterState *)presenter)
        (presenter->*(*(PresenterState *)presenter))();
    presenter->mTexSeq.Advance();
    presenter->mTexAnim.Advance();
    presenter->mFace.Advance();
    Presenter_UpdateMatrix((char *)presenter);
}
}

extern "C" void Presenter_Draw(Presenter *presenter) {
    int keyLight[3];
    int fillLight[3];
    int *materials;
    char *components;

    keyLight[0] = 0;
    keyLight[1] = 0;
    keyLight[2] = 0xfffff008;
    fillLight[0] = 0;
    fillLight[1] = 0;
    fillLight[2] = 0xfffff008;
    func_02016a04(&presenter->mFace, 0x3d6d);
    func_0203cd80(keyLight, -0x2000);
    func_0203ccd4(keyLight, 0x2000);
    func_0203cd80(fillLight, 0x4200);
    func_0203ccd4(fillLight, 0x7000);

    REG_LIGHT_COLOR = 0x7fff;
    REG_LIGHT_VECTOR = (((short)keyLight[0] >> 3) & 0x3ff) |
                       ((((short)keyLight[1] >> 3) & 0x3ff) << 10) |
                       ((((short)keyLight[2] >> 3) & 0x3ff) << 20);
    components = (char *)&presenter->mFace.data;
    REG_LIGHT_COLOR = 0x4000294b;
    REG_LIGHT_VECTOR = (((short)fillLight[0] >> 3) & 0x3ff) |
                       ((((short)fillLight[1] >> 3) & 0x3ff) << 10) |
                       ((((short)fillLight[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    /* The two tex animations scroll materials 5 and 6 by +0x14/+0x18, which
       BMD_Material leaves in its unnamed pad_0c (the u offsets are pad_0c[8]).
       The material pointer stays an int* view: naming the ModelComponents or
       the pad_0c fields moves 30 words (measured). */
    materials = *(int **)(components + 4);
    presenter->mTexSeq.Update(*(ModelComponents *)components);
    presenter->mTexAnim.Update(*(ModelComponents *)components);
    *(int *)((int)materials + 0x104) -= presenter->mSway[2];
    *(int *)((int)materials + 0x108) += presenter->mSway[3];
    *(int *)((int)materials + 0x134) += presenter->mSway[0];
    *(int *)((int)materials + 0x138) += presenter->mSway[1];
    presenter->mFace.Render(0);
    presenter->mBack.Render(&sPresenterScale);
}

extern "C" {
int Presenter_IsIdle(Presenter *presenter) {
    int differs = 1;
    StateSlot *idle = &pmf_Presenter_StateIdle;
    if (presenter->mState[0] == idle->w[0]) {
        if (presenter->mState[1] != idle->w[1] && presenter->mState[0] != 0)
            ;
        else
            differs = 0;
    }
    return differs == 0;
}
}

extern "C" {
/* Presenter_IsAnimDone @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b dExtFrameCtrl_c::Finished(). */

int Presenter_IsAnimDone(Presenter *presenter) {
    return presenter->mFace.Finished();
}
}

extern "C" {
void Presenter_ResetPose(char *c) {
    Presenter *presenter = PRESENTER(c);
    Presenter_PlayIdle(c);
    if (gSceneFader->IsAtEnd() == 0)
        return;
    presenter->mSway[1] = 0;
    presenter->mSway[0] = presenter->mSway[1];
    presenter->mSway[3] = 0;
    presenter->mSway[2] = presenter->mSway[3];
    /* Rewind each frame control to frame 0. */
    presenter->mFace.currFrame = 0;
    presenter->mTexSeq.currFrame = 0;
    presenter->mTexAnim.currFrame = 0;
}
}

extern "C" void Presenter_PlayIdle(char *c) {
    Presenter *presenter = PRESENTER(c);
    if (presenter->mFace.file != (BCA_File *)presenter->mWaitAnim.filePtr) {
        int blendFrames = 0;
        if (gSceneFader->IsAtEnd() == 0) {
            if (presenter->mFace.Finished() == 0 ||
                presenter->mFace.file == (BCA_File *)presenter->mSurprisedAnim.filePtr) {
                blendFrames = 8;
            }
        }
        presenter->mFace.SetAnim(*(BCA_File *)presenter->mWaitAnim.filePtr, blendFrames, 0,
                                    0x800, 0);
        presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mWaitTexSeq.filePtr, 0, 0x800, 0);
        presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
    }
    /* The volatile word reads are load-bearing: a plain struct copy moves two
       words (measured 2026-11-08). */
    {
        void *lo = ((volatile VolatileStateSlot *)&pmf_Presenter_StateIdle_2)->lo;
        void *hi = ((volatile VolatileStateSlot *)&pmf_Presenter_StateIdle_2)->hi;
        *(void **)&presenter->mState[0] = lo;
        *(void **)&presenter->mState[1] = hi;
    }
}

extern "C" {
void Presenter_StateIdle(Presenter *presenter) {
    if (!presenter->mFace.Finished())
        return;
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mWaitTexSeq.filePtr, 0, 0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
}
}

extern "C" {
void Presenter_PlayMatch(Presenter *presenter) {
    unsigned long long next;
    long long *state;
    if (presenter->mQuiet == 0) {
        func_02012174(0, 0x1c);
    }
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mWinAnim.filePtr, 4, 0x40000000, 0x800,
                                0);
    /* The state pointer and the unsigned copy are load-bearing: storing
       pmf_Presenter_StateWin straight through the record costs the function its bytes
       (measured 2026-11-08). */
    state = (long long *)presenter;
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mWinTexSeq.filePtr, 0x40000000, 0x800,
                                  0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterMatchTexAnim, 0x40000000, 0x800, 0);
    next = pmf_Presenter_StateWin;
    *state = next;
}
}

extern "C" {
void Presenter_StateWin(Presenter *presenter) {
    int r = Presenter_IsAnimDone(presenter);
    if (r == 0)
        return;
    Presenter_PlayIdle((char *)presenter);
}
}

extern "C" {


void Presenter_PlayMiss(char *c) {
    Presenter *presenter = PRESENTER(c);
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mMistakeAnim.filePtr, 4, 0x40000000,
                                0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mMistakeTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterMissTexAnim, 0x40000000, 0x800, 0);
    *(StateSlot *)c = pmf_Presenter_StateMistake;
}
}

extern "C" {
void Presenter_StateMistake(Presenter *presenter) {
    int r = Presenter_IsAnimDone(presenter);
    if (r == 0)
        return;
    Presenter_PlayIdle((char *)presenter);
}
}

extern "C" {
/* volatile is load-bearing: a plain long long load costs Presenter_PlayCheer
   its bytes (measured 2026-11-08). */
void Presenter_PlayCheer(Presenter *presenter) {
    if ((presenter->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mSurprisedAnim.filePtr, 8, 0x40000000,
                                0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mSurprisedTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    *((long long *)presenter) = pmf_Presenter_StateSurprisedThenSmile;
}
}

extern "C" {
void Presenter_StateSurprisedThenSmile(Presenter *presenter) {
    if (!Presenter_IsAnimDone(presenter))
        return;
    if (presenter->mFace.file != (BCA_File *)presenter->mSurprisedAnim.filePtr)
        return;
    if (presenter->mQuiet == 0)
        Sound::PlayBank2_2D(0x1d1);
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mSmileAnim.filePtr, 8, 0, 0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mSmileTexSeq.filePtr, 0, 0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterSmileTexAnim, 0, 0x800, 0);
}
}

extern "C" {
void Presenter_PlayLose(char *c) {
    Presenter *presenter = PRESENTER(c);
    if (presenter->mQuiet == 0)
        Sound::PlayBank2_2D(0x1c9);
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mLoseStartAnim.filePtr, 0, 0x40000000,
                                0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mLoseStartTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterLoseTexAnim, 0x40000000, 0x800, 0);
    *(StateSlot *)c = pmf_Presenter_StateLoseStart;
}
}

extern "C" {
void Presenter_StateLoseStart(Presenter *presenter) {
    if (!Presenter_IsAnimDone(presenter))
        return;
    if (presenter->mFace.file != (BCA_File *)presenter->mLoseStartAnim.filePtr)
        return;
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mLoseAnim.filePtr, 0, 0, 0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mLoseTexSeq.filePtr, 0, 0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterLoseLoopTexAnim, 0, 0x800, 0);
}
}

extern "C" {
void Presenter_PlayCheerLoop(Presenter *presenter) {
    long long state;
    if (presenter->mQuiet == 0) {
        func_02012174(0, 3);
    }
    presenter->mFace.SetAnim(*(BCA_File *)presenter->mSurprisedAnim.filePtr, 8, 0x40000000,
                                0x800, 0);
    presenter->mTexSeq.SetFile(*(BTP_File *)presenter->mSurprisedTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    presenter->mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    /* The u64 identity mask and the local copy are load-bearing: storing the
       record straight through costs the function its bytes (measured). */
    *((long long *)presenter) = (state = pmf_Presenter_StateSurprisedThenIdle & 0xFFFFFFFFFFFFFFFF);
}
}

extern "C" {
void Presenter_StateSurprisedThenIdle(Presenter *presenter) {
    int r = Presenter_IsAnimDone(presenter);
    if (r == 0)
        return;
    Presenter_PlayIdle((char *)presenter);
}
}

extern "C" {
void Presenter_UpdateMatrix(char *c) {
    Presenter *presenter = PRESENTER(c);
    Matrix4x3_FromTranslation(&data_020a0e68, presenter->mPos.x, presenter->mPos.y,
                              presenter->mPos.z);
    presenter->mFace.mat4x3 = data_020a0e68;
}
}

extern "C" {
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

extern "C" {
void Mascot_Load(char *c) {
    Mascot *mascot = MASCOT(c);
    void *f;
    f = dExtSimpleModel_c::LoadFile(mascot->mModelFile);
    mascot->mModel.SetFile((BMD_File *)f, 1, -1);
    dExtFrameCtrl_c::LoadFile(mascot->mWaitAnimFile);
    mascot->mModel.SetAnim(*(BCA_File *)mascot->mWaitAnimFile.filePtr, 0, 0, 0x800, 0);
    mascot->mModel.HideMaterial(7, 0);
    mascot->mPos.x = 0xd000;
    mascot->mPos.y = 0;
    mascot->mPos.z = 0;
    mascot->mRot[0] = 0;
    mascot->mRot[1] = -0x1400;
    mascot->mRot[2] = 0;
}
}

extern "C" {
/* Mascot_Update @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b dExtBlendAnmModel_c::Advance(). */

void Mascot_Update(Mascot *mascot) {
    mascot->mModel.Advance();
}
}


/* The render is dExtSimpleModel_c's own, called directly: 0x02016b78, not dExtBlendAnmModel_c's
   slot 5. */
extern "C" void Mascot_Draw(Mascot *c) {
    if (c->mLooking != 0) {
        s16 h = Vec3_HorzAngle(&c->mPos, &c->mLookTarget);
        s16 v = Vec3_VertAngle(&c->mPos, &c->mLookTarget);
        s16 dh = (s16)(h - c->mRot[1]);
        if (dh < -0x2800)
            dh = -0x2800;
        else if (dh > 0x2800)
            dh = 0x2800;
        if (v < -0x1000)
            v = -0x1000;
        else if (v > 0x1000)
            v = 0x1000;
        UpdateAngle(c->mHeadYaw, dh, 8, 0x200);
        UpdateAngle(c->mHeadPitch, v, 8, 0x200);
        dh = (s16)(h - dh);
        UpdateAngle(c->mRot[1], dh, 8, 0x200);
    } else {
        UpdateAngle(c->mHeadPitch, 0, 8, 0x200);
        UpdateAngle(c->mHeadYaw, 0x800, 8, 0x200);
        UpdateAngle(c->mRot[1], -0x1400, 8, 0x200);
    }
    Mascot_UpdateMatrix(c);
    c->mModel.dExtSimpleModel_c::Render(0);
}



extern "C" void Mascot_UpdateMatrix(Mascot *mascot) {
    Matrix4x3_FromTranslation(&data_020a0e68, mascot->mPos.x, mascot->mPos.y,
                              mascot->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mascot->mRot[1]);
    mascot->mModel.mat4x3 = data_020a0e68;
    mascot->mModel.UpdateVerts();
    /* bone 6, the head */
    data_020a0e68 = mascot->mModel.data.transforms[6];
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mascot->mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mascot->mHeadPitch);
    mascot->mModel.data.transforms[6] = data_020a0e68;
}

extern "C" {
void Mascot_LookAt(char *c, int *src) {
    Mascot *mascot = MASCOT(c);
    if (src != 0) {
        mascot->mLookTarget.x = src[0];
        mascot->mLookTarget.y = src[1];
        mascot->mLookTarget.z = src[2];
    } else {
        mascot->mLookTarget.x = 0;
        mascot->mLookTarget.y = 0x2000;
        mascot->mLookTarget.z = -0x2000;
    }
    mascot->mLooking = 1;
}
}

extern "C" {
void Mascot_StopLooking(int *p) {
    Mascot *mascot = MASCOT(p);
    mascot->mLooking = 0;
}
}

extern "C" {
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

extern "C" {

int Dealer_Load(char *c) {
    Dealer *dealer = DEALER(c);
    void *file;
    BMD_Material *materials;

    dExtShadowModel_c::CleanAll();
    dealer->mModelData = dExtSimpleModel_c::LoadFile(dealer->mModelFile);
    dealer->mTableData = dExtSimpleModel_c::LoadFile(dealer->mTableFile);
    file = dealer->mModelData;
    if (file == 0 || dealer->mModel.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    dExtFrameCtrl_c::LoadFile(dealer->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mShuffleStartAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mShuffleAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mDealStartAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mDealAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mDealEndAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mDealEnd2Anim);
    dExtFrameCtrl_c::LoadFile(dealer->mDealWaitAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mThrowAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mWinStartAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mWinAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mWinEndAnim);
    dExtFrameCtrl_c::LoadFile(dealer->mLoseAnim);
    dealer->mModel.SetAnim(*(BCA_File *)dealer->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    Dealer_ResetMatrices(c);
    dealer->mPos.x = data_020a0ebc.x;
    dealer->mPos.y = data_020a0ebc.y;
    dealer->mPos.z = data_020a0ebc.z;
    dealer->mRot[0] = data_020a0edc.x;
    dealer->mRot[1] = data_020a0edc.y;
    dealer->mRot[2] = data_020a0edc.z;
    dealer->mDealCounter = 0;
    dealer->mModel.HideMaterial(1, 0);
    Dealer_PlayIdle(c);
    file = dealer->mTableData;
    if (file == 0 || dealer->mTable.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    materials = dealer->mTable.data.materials;
    materials[0].flags &= ~1;
    materials[5].flags &= ~1;
    materials[4].flags |= 2;
    MascotWalker_Load(&dealer->mWalker);
    dealer->mLampTimer = 0;
    dealer->mLampPattern = 1;
    dealer->mLightAngle = 0;
    return 1;
}
}


extern "C" void Dealer_Update(Dealer *dealer) {
    if (dealer->mState[0] != 0) {
        (dealer->*(*(DealerState *)dealer->mState))();
    }
    dealer->mModel.Advance();
    if (ApproachLinear2(dealer->mLampTimer, 0, 1) != 0) {
        dealer->mLampPattern++;
        if (dealer->mLampPattern > 7)
            dealer->mLampPattern = 1;
        dealer->mLampTimer = sDealerLampTimes[dealer->mLampPattern];
    }
    dealer->mLightAngle += 0x400;
    MascotWalker_Update(&dealer->mWalker);
}



extern "C" void Dealer_Draw(void *self) {
    Dealer *dealer = DEALER(self);
    Vector3 keyLight, spinLight;
    short angle;
    unsigned char lamp[3];
    int i;

    keyLight.x = 0;
    keyLight.y = 0;
    keyLight.z = 0xfffff008;
    spinLight.x = 0;
    spinLight.y = 0;
    spinLight.z = 0xfffff008;
    func_0203cd80(&keyLight, -0x1000);
    func_0203ccd4(&keyLight, 0x1000);

    angle = dealer->mLightAngle;
    func_0203ccd4(&spinLight, angle);

    REG_LIGHT_VECTOR = (((short)keyLight.x >> 3) & 0x3ff) |
                       ((((short)keyLight.y >> 3) & 0x3ff) << 10) |
                       ((((short)keyLight.z >> 3) & 0x3ff) << 20);

    REG_LIGHT_VECTOR = (((short)spinLight.x >> 3) & 0x3ff) |
                       ((((short)spinLight.y >> 3) & 0x3ff) << 10) |
                       ((((short)spinLight.z >> 3) & 0x3ff) << 20) | 0x40000000;

    func_02016a14(&dealer->mModel, 0x7fff);
    func_02016a04(&dealer->mModel, 0x4210);

    {
        short pattern = dealer->mLampPattern;
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

    func_020169d8(&dealer->mTable, 0,
                  (unsigned short)(lamp[0] | (lamp[1] << 5) | (lamp[2] << 10)));
    func_020169d8(&dealer->mTable, 4, 0x6318);
    func_020169d8(&dealer->mTable, 5,
                  (unsigned short)(lamp[1] | (lamp[2] << 5) | (lamp[0] << 10)));

    dealer->mModel.Render(0);
    MascotWalker_Draw((char *)&dealer->mWalker);
    dealer->mTable.Render(0);

    dExtShadowModel_c::RenderAll();
}



extern "C" void Dealer_PlayIdle(char *c) {
    Dealer *dealer = DEALER(c);
    if (dealer->mHand == 1) {
        dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealWaitAnim.filePtr, 0, 0, 0x800, 0);
        if (dealer->mDealSoundPlayed == 0) {
            Sound::PlayBank2_2D(0x13a);
            dealer->mDealSoundPlayed = 1;
        }
    } else {
        dealer->mModel.SetAnim(*(BCA_File *)dealer->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    }
    *(StateSlot *)c = pmf_Dealer_StateIdle;
}

extern "C" {
void Dealer_StateIdle(void) {}
}

extern "C" {
int Dealer_IsIdle(Dealer *dealer) {
    int *idle = pmf_Dealer_StateIdle_2;
    int fn = dealer->mState[0];
    int differs = 1;
    if (fn == idle[0]) {
        if (dealer->mState[1] == idle[1] || fn == 0)
            differs = 0;
    }
    return differs == 0;
}
}

extern "C" {
int Dealer_IsIdleAtRest(Dealer *dealer) {
    int *idle = pmf_Dealer_StateIdle_3;
    int fn = dealer->mState[0];
    int atRest = 0;
    int differs = 1;
    if (fn == idle[0] && (dealer->mState[1] == idle[1] || fn == 0))
        differs = 0;
    if (differs == 0 && dealer->mModel.file == (BCA_File *)dealer->mWaitAnim.filePtr)
        atRest = 1;
    return atRest;
}
}

extern "C" {

void Dealer_StartShuffle(Dealer *dealer, int, short count, s16 *counter) {
    if (dealer->mShuffleRound == 0) {
        dealer->mShuffleLeft = 4;
        dealer->mDealDoneAt = 3;
    } else {
        dealer->mShuffleLeft = 1;
        dealer->mDealDoneAt = 1;
    }
    if (ApproachLinear2(dealer->mShuffleRound, 5, 1) != 0) {
        dealer->mShuffleRound = 0;
    }
    dealer->mDealSoundPlayed = 0;
    dealer->mDealLeft = count;
    dealer->mDealCounter = counter;
    dealer->mModel.SetAnim(*(BCA_File *)dealer->mShuffleStartAnim.filePtr, 0, 0x40000000, 0x800, 0);
    *(StateSlot *)dealer = pmf_Dealer_StateShuffle;
}
}


extern "C" void Dealer_StateShuffle(Dealer *dealer) {
    if (dealer->mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        dealer->mShuffleSoundPlayed = 1;
    }

    if (dealer->mModel.file == (BCA_File *)dealer->mShuffleStartAnim.filePtr) {
        if (dealer->mModel.Finished())
            dealer->mModel.SetAnim(*(BCA_File *)dealer->mShuffleAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }

    if (dealer->mModel.WillHitFrame(0)) {
        if (dealer->mShuffleLeft-- <= 0) {
            Dealer_StartDeal(dealer, dealer->mDealLeft, dealer->mDealCounter);
            return;
        }
    }

    if (!dealer->mModel.WillHitFrame(4))
        return;

    Sound::PlayBank2_2D(0x141);
    if (dealer->mShuffleLeft == dealer->mDealDoneAt) {
        dScMgBase_c *scene = gMgScene;
        if (scene)
            scene->OnDealDone();
    }
}



extern "C" void Dealer_StartDeal(Dealer *dealer, short count, s16 *counter) {
    dealer->mDealLeft = count;
    dealer->mDealCounter = counter;
    if (Dealer_IsIdle(dealer) &&
        dealer->mModel.file != (BCA_File *)dealer->mDealWaitAnim.filePtr) {
        dealer->mModel.SetAnim(*(BCA_File *)dealer->mShuffleStartAnim.filePtr, 0, 0x40000000,
                                  0x800, 0);
    } else {
        dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealStartAnim.filePtr, 0, 0x40000000,
                                  0x800, 0);
    }
    *(StateSlot *)dealer = pmf_Dealer_StateDeal;
}

extern "C" {

void Dealer_StateDeal(Dealer *dealer) {
    void *anim = dealer->mModel.file;

    if (anim == (void *)dealer->mDealStartAnim.filePtr) {
        if (dealer->mModel.Finished()) {
            dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealAnim.filePtr, 0, 0, 0x800, 0);
        }
    }

    anim = dealer->mModel.file;
    if (anim == (void *)dealer->mDealEndAnim.filePtr ||
        anim == (void *)dealer->mDealEnd2Anim.filePtr) {
        if (dealer->mModel.Finished()) {
            dealer->mDealCounter = 0;
            Dealer_PlayIdle((char *)dealer);
        }
        return;
    }

    if (anim == (void *)dealer->mDealAnim.filePtr) {
        if (dealer->mModel.WillHitFrame(0)) {
            if (dealer->mDealLeft-- <= 0) {
                short state = dealer->mHand;
                if (state == 0) {
                    dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealEndAnim.filePtr, 0,
                                              0x40000000, 0x800, 0);
                    if (dealer->mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    dealer->mDealSoundPlayed = 1;
                    return;
                } else if (state == 2) {
                    dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealEnd2Anim.filePtr, 0,
                                              0x40000000, 0x800, 0);
                    if (dealer->mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    dealer->mDealSoundPlayed = 1;
                    return;
                } else {
                    Dealer_PlayIdle((char *)dealer);
                    return;
                }
            }
        }

        if (dealer->mDealCounter == 0)
            return;
        if (!dealer->mModel.WillHitFrame(5))
            return;
        Sound::PlayBank2_2D(0x142);
        *dealer->mDealCounter += 1;
        return;
    }

    if (anim == (void *)dealer->mShuffleStartAnim.filePtr) {
        if (dealer->mModel.Finished()) {
            dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealStartAnim.filePtr, 0, 0x40000000,
                                      0x800, 0);
        }
    }
}
}

extern "C" {

void Dealer_StartReveal(Dealer *dealer, int count, s16 *counter) {
    dealer->mDealLeft = count;
    dealer->mDealCounter = counter;
    dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealAnim.filePtr, 8, 0, 0x800, 0);
    *(StateSlot *)dealer = pmf_Dealer_StateReveal_2;
}
}


extern "C" void Dealer_StateReveal(Dealer *dealer) {
    if (dealer->mModel.file != (BCA_File *)dealer->mDealAnim.filePtr)
        goto other;

    if (dealer->mModel.WillHitFrame(0)) {
        if (dealer->mDealLeft-- <= 0) {
            int hand;
            dealer->mDealCounter = 0;
            hand = dealer->mHand;
            if (hand == 0) {
                dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealEndAnim.filePtr, 0, 0x40000000,
                                       0x800, 0);
                if (dealer->mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                dealer->mDealSoundPlayed = 1;
                return;
            } else if (hand == 2) {
                dealer->mModel.SetAnim(*(BCA_File *)dealer->mDealEnd2Anim.filePtr, 0, 0x40000000,
                                       0x800, 0);
                if (dealer->mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                dealer->mDealSoundPlayed = 1;
                return;
            } else {
                Dealer_PlayIdle((char *)dealer);
                return;
            }
        }
    }

    if (dealer->mDealCounter == 0)
        return;
    if (!dealer->mModel.WillHitFrame(5))
        return;
    Sound::PlayBank2_2D(0x142);
    *dealer->mDealCounter += 1;
    return;

other:
    if (dealer->mModel.file == (BCA_File *)dealer->mDealEndAnim.filePtr ||
        dealer->mModel.file == (BCA_File *)dealer->mDealEnd2Anim.filePtr) {
        if (dealer->mModel.Finished())
            Dealer_PlayIdle((char *)dealer);
    }
}

extern "C" {

int Dealer_IsBusy(Dealer *dealer) {
    int *pmf = pmf_Dealer_StateDeal_2;
    int busy = 1;
    int notDealing = busy;
    int fn = dealer->mState[0];
    if (fn == pmf[0]) {
        if (dealer->mState[1] == pmf[1] || fn == 0)
            notDealing = 0;
    }
    if (notDealing != 0) {
        int notRevealing = 1;
        pmf = pmf_Dealer_StateReveal;
        if (fn == pmf[0]) {
            if (dealer->mState[1] == pmf[1] || fn == 0)
                notRevealing = 0;
        }
        if (notRevealing)
            busy = 0;
    }
    return busy;
}
}

extern "C" {
/* Dealer_IsAnimDone @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b dExtFrameCtrl_c::Finished(). */

int Dealer_IsAnimDone(Dealer *dealer) {
    return dealer->mModel.Finished();
}
}

extern "C" {

void Dealer_StartSwap(int *t) {
    DEALER(t)->mModel.SetAnim(*(BCA_File *)DEALER(t)->mThrowAnim.filePtr, 0, 0x40000000, 0x800, 0);
    DEALER(t)->mModel.HideMaterial(2, 0);
    DEALER(t)->mModel.ShowMaterial(1, 0);
    *(StateSlot *)t = pmf_Dealer_StateSwap;
}
}


extern "C" void Dealer_StateSwap(Dealer *dealer) {
    if (dealer->mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        dealer->mShuffleSoundPlayed = 1;
    }
    /* 0x6c is the dExtFrameCtrl_c inside mModel (its +0x50). */
    if (dealer->mModel.file == (BCA_File *)dealer->mThrowAnim.filePtr &&
        dealer->mModel.Finished()) {
        dealer->mModel.HideMaterial(1, 0);
        dealer->mModel.ShowMaterial(2, 0);
        return;
    }
    if (dealer->mModel.WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}

extern "C" void Dealer_PlayLose(char *c) {
    if (DEALER(c)->mHand == 1) {
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEndAnim.filePtr, 0, 0x40000000,
                                  0x800, 0);
    } else {
        Sound::PlayBank2_2D(0x13c);
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mLoseAnim.filePtr, 0, 0x40000000, 0x800,
                                  0);
    }
    *(StateSlot *)c = pmf_Dealer_StateLose;
}

extern "C" {
void Dealer_StateLose(Dealer *dealer) {
    if (!dealer->mModel.Finished())
        return;
    if (dealer->mModel.file == (BCA_File *)dealer->mLoseAnim.filePtr) {
        Dealer_PlayIdle((char *)dealer);
        return;
    }
    Sound::PlayBank2_2D(0x13c);
    dealer->mHand = 0;
    dealer->mModel.SetAnim(*(BCA_File *)dealer->mLoseAnim.filePtr, 0, 0x40000000, 0x800, 0);
}
}

extern "C" {
void Dealer_PlayWin(Dealer *dealer) {
    Sound::PlayBank2_2D(0x13b);
    dealer->mModel.SetAnim(*(BCA_File *)dealer->mWinStartAnim.filePtr, 0, 0x40000000, 0x800,
                              0);
    dealer->mShuffleLeft = 5;
    *(StateSlot *)dealer = pmf_Dealer_StateWin;
}
}

extern "C" {
void Dealer_StateWin(Dealer *dealer) {
    if (dealer->mModel.file == (BCA_File *)dealer->mWinStartAnim.filePtr &&
        dealer->mModel.Finished()) {
        dealer->mModel.SetAnim(*(BCA_File *)dealer->mWinAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }
    if (dealer->mModel.file == (BCA_File *)dealer->mWinAnim.filePtr &&
        dealer->mModel.WillHitFrame(0)) {
        dealer->mShuffleLeft -= 1;
        if (dealer->mShuffleLeft == 0) {
            dealer->mModel.SetAnim(*(BCA_File *)dealer->mWinEndAnim.filePtr, 0, 0x40000000,
                                      0x800, 0);
            return;
        }
    }
    if (dealer->mModel.file == (BCA_File *)dealer->mWinEndAnim.filePtr &&
        dealer->mModel.Finished()) {
        Dealer_PlayIdle((char *)dealer);
    }
}
}

extern "C" {
void Dealer_ResetMatrices(char *c) {
    Dealer *dealer = DEALER(c);
    Matrix4x3 identity = sDealerIdentityMtx;
    dealer->mModel.mat4x3 = sDealerIdentityMtx;
    dealer->mTable.mat4x3 = identity;
}
}

extern "C" {
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



extern "C" void MascotWalker_Load(MascotWalker *walker) {
    {
        void *bmd = dExtSimpleModel_c::LoadFile(walker->mModelFile);
        walker->mModel.SetFile((BMD_File *)bmd, 1, -1);
    }
    dExtFrameCtrl_c::LoadFile(walker->mWalkAnimFile);
    dExtFrameCtrl_c::LoadFile(walker->mPauseAnimFile);
    walker->mModel.SetAnim(*(BCA_File *)walker->mPauseAnimFile.filePtr, 0, 0, 0x800, 0);
    walker->mModel.SetPolygonID(1);
    walker->mShadow.InitCylinder();
    walker->mScale.x = 0x1000;
    walker->mScale.y = 0x1000;
    walker->mScale.z = 0x1000;
    walker->mPos.x = data_020a0ebc.x;
    walker->mPos.y = data_020a0ebc.y;
    walker->mPos.z = data_020a0ebc.z;
    walker->mAngleX = data_020a0edc.x;
    walker->mAngleY = data_020a0edc.y;
    walker->mAngleZ = data_020a0edc.z;
    MascotWalker_StartNextPath(walker);
}



extern "C" void MascotWalker_Update(MascotWalker *walker) {
    (walker->*(*(MascotWalkerState *)walker->mState))();

    walker->mModel.Advance();

    if (walker->mPaused != 0) {
        Vector3 target;
        target.x = sMascotWalkerOffset.x;
        target.y = sMascotWalkerOffset.y;
        target.z = sMascotWalkerOffset.z;
        short turn;
        int angle;
        angle = Vec3_HorzAngle(&walker->mPos, &target);
        turn = (short)(angle - walker->mAngleY);
        if (turn < -0x3000)
            turn = -0x3000;
        else if (turn > 0x3000)
            turn = 0x3000;
        UpdateAngle(walker->mHeadAngle, turn, 8, 0x200);

        {
            int *state = walker->mState;
            int *walk = pmf_MascotWalker_StateWalk_2;
            if (state[0] == walk[0]) {
                if (state[1] == walk[1])
                    return;
                if (walker->mState[0] == 0)
                    return;
            }
        }
        turn = (short)(angle - turn);
        UpdateAngle(walker->mAngleY, turn, 8, 0x200);
    } else {
        UpdateAngle(walker->mHeadAngle, 0, 8, 0x200);
    }
}

extern "C" void MascotWalker_Draw(char *t) {
    MascotWalker *walker = WALKER(t);
    MascotWalker_UpdateMatrix(walker);
    walker->mShadow.InitModel(&walker->mModel.mat4x3, 0x3c000, 0x14000, 0x3c000, 0xc);
    /* dExtSimpleModel_c's own render, called directly (0x02016b78), as Mascot_Draw does. */
    walker->mModel.dExtSimpleModel_c::Render(0);
}


extern "C" void MascotWalker_UpdateMatrix(MascotWalker *walker) {
    walker->mModel.UpdateVerts();
    /* Bone 6 takes the head angle. */
    data_020a0e68 = walker->mModel.data.transforms[6];
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, walker->mHeadAngle);
    walker->mModel.data.transforms[6] = data_020a0e68;
    Matrix4x3_FromTranslation((Matrix4x3 *)&data_020a0e68, walker->mPos.x, walker->mPos.y,
                              walker->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, walker->mAngleY);
    walker->mModel.mat4x3 = data_020a0e68;
}

extern "C" {
void MascotWalker_StartNextPath(MascotWalker *walker) {
    Vector3 toNext;
    Vector3 target;
    walker->mModel.SetAnim(*(BCA_File *)walker->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    if (walker->mPathIndex < 3) {
        walker->mPathIndex += 1;
    } else {
        walker->mPathIndex = 0;
    }
    walker->mPath = &sMascotWalkerPaths[walker->mPathIndex];
    walker->mPathPoint = 0;
    {
        Vector3 *point = &(walker->mPath)->points[walker->mPathPoint];
        walker->mPos.x = point->x;
        walker->mPos.y = point->y;
        walker->mPos.z = point->z;
    }
    walker->mPathPoint += 1;
    Vec3_Sub(&toNext, &walker->mPos,
             &(walker->mPath)->points[walker->mPathPoint]);
    walker->mDir.x = toNext.x;
    walker->mDir.y = toNext.y;
    walker->mDir.z = toNext.z;
    if (NormalizeVec3IfNonZero(&walker->mDir) == 0) {
        walker->mDir.x = 0xb50;
        walker->mDir.y = 0xb50;
        walker->mDir.z = 0;
    }
    {
        Vector3 *point = &(walker->mPath)->points[walker->mPathPoint];
        target.x = point->x;
        target.y = point->y;
        target.z = point->z;
    }
    walker->mTargetAngle = (short)Vec3_HorzAngle(&walker->mPos, &target);
    {
        int fn = pmf_MascotWalker_StateWalk[0];
        int adj = pmf_MascotWalker_StateWalk[1];
        /* The no-op ternary is load-bearing: plain stores, or a copy through
           two locals, move three words (measured 2026-11-08). */
        walker->mState[0] = adj ? fn : fn;
        walker->mState[1] = adj;
    }
}
}

extern "C" {
void MascotWalker_StateWalk(MascotWalker *walker) {
    int dx, dz;
    int stepX, stepZ;
    int doneX, doneZ;

    UpdateAngle(walker->mAngleY, walker->mTargetAngle, 8, 0x100);

    dx = walker->mDir.x;
    if (dx < 0)
        dx = -dx;
    stepX = FX_MUL(dx, 0x1a9);

    dz = walker->mDir.z;
    if (dz < 0)
        dz = -dz;

    doneX = ApproachLinear(walker->mPos.x,
                           (walker->mPath)->points[walker->mPathPoint].x, stepX);

    stepZ = FX_MUL(dz, 0x1a9);

    doneZ = ApproachLinear(walker->mPos.z,
                           (walker->mPath)->points[walker->mPathPoint].z, stepZ);

    if (doneX != 0 && doneZ != 0) {
        if (walker->mPathPoint < (walker->mPath)->count - 1) {
            Vector3 dir;
            Vector3 next;

            walker->mPathPoint++;

            Vec3_Sub(&dir, &walker->mPos,
                     &(walker->mPath)->points[walker->mPathPoint]);

            walker->mDir.x = dir.x;
            walker->mDir.y = dir.y;
            walker->mDir.z = dir.z;

            if (NormalizeVec3IfNonZero(&walker->mDir) == 0) {
                walker->mDir.x = 0xb50;
                walker->mDir.y = 0xb50;
                walker->mDir.z = 0;
            }

            {
                Vector3 *point = &(walker->mPath)->points[walker->mPathPoint];
                next.x = point->x;
                next.y = point->y;
                next.z = point->z;
            }

            walker->mTargetAngle = Vec3_HorzAngle(&walker->mPos, &next);
            return;
        }

        MascotWalker_StartNextPath(walker);
        return;
    }

    if (ApproachLinear2(walker->mStateTimer, 0, 1) == 0)
        return;

    if (walker->mModel.WillHitFrame(0) == 0)
        return;

    MascotWalker_StartPause(walker);
}
}

extern "C" {

void MascotWalker_StartPause(MascotWalker *walker) {
    walker->mModel.SetAnim(*(BCA_File *)walker->mPauseAnimFile.filePtr, 8, 0, 0x800, 0);
    walker->mStateTimer = 0x78;
    walker->mPaused = 1;
    *(StateSlot *)walker->mState = pmf_MascotWalker_StatePause;
}
}



extern "C" void MascotWalker_StatePause(MascotWalker *walker) {
    if (ApproachLinear2(walker->mStateTimer, 0, 1) == 0)
        return;
    walker->mModel.SetAnim(*(BCA_File *)walker->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    /* Walk for 0x258..0x4af frames: a random 0..0xfff scaled into 0x258. */
    int roll = RandomIntInternal(&data_0209e650.value);
    int frac = (int)((unsigned int)(roll & 0x7fffffff) >> 0x13);
    walker->mStateTimer = (short)(((frac * 0x258) >> 12) + 0x258);
    walker->mPaused = 0;
    *(StateSlot *)walker->mState = pmf_MascotWalker_StateWalk_3;
}

extern "C" {
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

    SubVec3((Vector3 *)&camera->mLookAt, (Vector3 *)&camera->mPos, &forward);
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

    G3i::PerspectiveW_(FX_SinIdx(camera->mFovAngle >> 4), FX_CosIdx(camera->mFovAngle >> 4), 0x1555,
                       0x1000, 0x1388000, 0x1000, 1, (Matrix4x3 *)camera->mProjMtx);

    G3i::LookAt_((Vector3 *)&camera->mPos, &up, (Vector3 *)&camera->mLookAt, true,
                 (Matrix4x3 *)&camera->mViewMtx);

    CopyToViewMat((Matrix4x3 *)&camera->mViewMtx);

    data_0209f43c.Func_020156DC(0x1555, camera->mFovAngle, 0x1000, 0x1388000);
}
}

extern "C" {

void Camera_ProjectToScreen(MgCamera *camera, void *pos, int *screenX, int *screenY) {
    int view[3];
    MulVec3Mat4x3((Vector3 *)pos, (Matrix4x3 *)&camera->mViewMtx, (Vector3 *)view);
    int fovIdx = camera->mFovAngle >> 4;
    int tanFov = cstd::fdiv(FX_SinIdx(fovIdx), FX_CosIdx(fovIdx));
    int height = FX_MUL(tanFov, -view[2]);
    int fracY = cstd::fdiv(view[1], height);
    int pixelY = FX_MUL(fracY, 0x5f800u);
    screenY[0] = -((pixelY + (int)0xfffa0800) >> 0xc);
    int width = FX_MUL(height, 0x1555u);
    int fracX = cstd::fdiv(view[0], width);
    int pixelX = FX_MUL(fracX, 0x7f800u);
    screenX[0] = (pixelX + 0x7f800) >> 0xc;
}
}

extern "C" {

void Camera_ProjectToScreen16(MgCamera *camera, void *pos, short *screen) {
    int view[3];
    MulVec3Mat4x3((Vector3 *)pos, (Matrix4x3 *)&camera->mViewMtx, (Vector3 *)view);
    int fovIdx = camera->mFovAngle >> 4;
    int tanFov = cstd::fdiv(FX_SinIdx(fovIdx), FX_CosIdx(fovIdx));
    int height = FX_MUL(tanFov, -view[2]);
    int fracY = cstd::fdiv(view[1], height);
    int pixelY = FX_MUL(fracY, 0x5f800u);
    screen[1] = -((pixelY + (int)0xfffa0800) >> 0xc);
    int width = FX_MUL(height, 0x1555u);
    int fracX = cstd::fdiv(view[0], width);
    int pixelX = FX_MUL(fracX, 0x7f800u);
    screen[0] = (pixelX + 0x7f800) >> 0xc;
}
}
