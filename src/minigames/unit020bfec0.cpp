//cpp
/* ov006 0x020bfec0..0x020c4048, the overlay's first linker unit.
 *
 * No RTTI, vtable or static initialiser names a type in this run.
 * Camera_UpdateMatrices at 0x020c0134 is the one named entry.
 *
 * leftovers:
 * - The other functions keep their address names.
 * - Each function keeps its own namespace. The local layouts and
 *   externs disagree, and one shared view changes codegen.
 * - A char* or void* parameter stays where the typed header form
 *   changes the call.
 * - The records' models are real dExtSimpleModel_c / dExtAnmModel_c / dExtBlendAnmModel_c
 *   members, so their SetFile/SetAnim/Advance/Render calls, frame tests,
 *   matrix stores and destructor calls are member calls. Every record here
 *   (Dealer, MascotWalker, Mascot, FallingPetal, FlowerFace, Presenter) has
 *   its own constructor and destructor at its ROM address, so the compiler
 *   builds and tears down their members.
 * - The other per-record helpers are still free functions over a char*
 *   record, read through the WALKER() / PRESENTER() / DEALER() / MASCOT() /
 *   PETAL() casts.
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
#include "dScMgBase_c.h"
#include "MWCPlusLib.h"
#include "LevelRng.h"
#include "MgCamera.h"
#include "math/Quaternion.h"
#include "nitro/fx/fx_cp.h"
/* Typed access to a MascotWalker record from the char* helpers below. */
#define WALKER(p) ((MascotWalker *)(p))
/* Typed access to a Presenter record from the char* helpers below. */
#define PRESENTER(p) ((Presenter *)(p))
/* Typed access to a Dealer record from the char* helpers below. */
#define DEALER(p) ((Dealer *)(p))
/* Typed access to a Mascot record from the char* helpers below. */
#define MASCOT(p) ((Mascot *)(p))
/* Typed access to the flower game's Yoshi and petal records. */
#define FLOWERFACE(p) ((FlowerFace *)(p))
#define PETAL(p) ((FallingPetal *)(p))

struct Sound {
    static void PlayBank2_2D(unsigned int);
};

void CopyToViewMat(const Matrix4x3 *);

extern "C" {

/* local extern: no shared math header declares MTX_MultVec43. */
void MTX_MultVec43(Vector3 *, Matrix4x3 *, Vector3 *);
void Matrix4x3_ApplyInPlaceToRotationZ(void *, short);
void Matrix4x3_FromQuaternion(const Quaternion *, Matrix4x3 *);
void MTX_MultVec33(void *, void *, void *);
void MTX_Concat43(void *, void *, void *);
void __cxa_vec_cleanup(void *, unsigned int, unsigned int, dtor_t);
}

void func_020169d8(void *, int, unsigned int);
extern "C" {

void func_0203cebc(void *, void *, void *, void *);
/* local extern: CalcLib conflicts with this TU's void* Vec3_HorzAngle
 * spelling; keep these declarations local but use the shared vector types. */
void VEC_Add(Vector3 *, Vector3 *, Vector3 *);
int VEC_DotProduct(const Vector3 *, const Vector3 *);
void Matrix4x3_ApplyInPlaceToRotationX(void *, short);
void Matrix4x3_ApplyInPlaceToRotationY(void *, short);
void Matrix4x3_FromTranslation(void *, int, int, int);
int NormalizeVec3IfNonZero(Vector3 *);
}

void Quaternion_FromVector3(void *, void *, void *);
extern "C" {

void Quaternion_Normalize(void *);
void Quaternion_SLerp(void *, void *, int, void *);
int RandomIntInternal(int *);
/* local extern: CalcLib cannot be included without conflicting with this TU's
 * other helper spellings, including its Vec3_HorzAngle declaration. */
void VEC_Subtract(Vector3 *, Vector3 *, Vector3 *);
void Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *);
}

short Vec3_HorzAngle(const Vector3 *, const Vector3 *);
// local extern: no shared header declares Vec3_VertAngle.
short Vec3_VertAngle(const Vector3 *, const Vector3 *);
extern "C" {

void Vec3_MulScalar(Vector3 *, const Vector3 *, int);
void Vec3_MulScalarInPlace(Vector3 *, int);
void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);
}

void *SharedFilePtr_Construct_TexSeq(void *, unsigned int);
int SharedFilePtr_Destruct_Anim(void *);
int SharedFilePtr_Destruct_TexSeq(void *);
extern "C" {


void __cxa_vec_ctor(void *, unsigned int, unsigned int, ctor_t, dtor_t);
}

unsigned int func_02012174(unsigned int, unsigned int);
void func_02016a04(void *, int);
void func_02016a14(void *, int);
int func_02017ab4(void *);
void *func_02017acc(void *, unsigned int);
extern "C" {

void Quaternion_Multiply(void *, void *, void *);
void func_0203ccd4(void *, short);
void func_0203cd80(void *, short);
void MascotWalker_StatePause(char *c);
void MascotWalker_StartPause(int *t);
void MascotWalker_StateWalk(char *c);
void MascotWalker_StartNextPath(char *c);
void MascotWalker_UpdateMatrix(MascotWalker *walker);
void MascotWalker_Draw(char *t);
void MascotWalker_Update(MascotWalker *walker);
void MascotWalker_Load(MascotWalker *walker);
void Dealer_ResetMatrices(char *c);
void Dealer_StateWin(char *p);
void Dealer_PlayWin(void *c);
void Dealer_StateLose(char *c);
void Dealer_PlayLose(char *c);
void Dealer_StateSwap(char *c);
void Dealer_StartSwap(int *t);
int Dealer_IsAnimDone(void *a);
int Dealer_IsBusy(int *c);
void Dealer_StateReveal(void *cc);
void Dealer_StartReveal(int *t, int count, s16 *counter);
void Dealer_StateDeal(char *c);
void Dealer_StartDeal(char *c, short count, s16 *counter);
void Dealer_StateShuffle(char *c);
void Dealer_StartShuffle(Dealer *dealer, int unused, short count, s16 *counter);
int Dealer_IsIdleAtRest(char *c);
int Dealer_IsIdle(Dealer *dealer);
void Dealer_StateIdle(void);
void Dealer_PlayIdle(char *c);
void Dealer_Draw(void *self);
void Dealer_Update(Dealer *dealer);
int Dealer_Load(char *c);
void Mascot_StopLooking(int *p);
void Mascot_LookAt(char *c, int *src);
void Mascot_UpdateMatrix(char *c);
void Mascot_Draw(void *c);
void Mascot_Update(void *a);
void Mascot_Load(char *c);
void Presenter_UpdateMatrix(char *c);
void Presenter_StateSurprisedThenIdle(char *t);
void Presenter_PlayCheerLoop(void *c);
void Presenter_StateLoseStart(void *c);
void Presenter_PlayLose(char *c);
void Presenter_StateSurprisedThenSmile(void *c);
void Presenter_PlayCheer(void *c);
void Presenter_StateMistake(char *t);
void Presenter_PlayMiss(char *c);
void Presenter_StateWin(char *t);
void Presenter_PlayMatch(void *c);
void Presenter_StateIdle(char *c);
void Presenter_PlayIdle(char *c);
void Presenter_ResetPose(char *c);
int Presenter_IsAnimDone(void *a);
int Presenter_IsIdle(void *c);
void Presenter_Draw(char *c);
void Presenter_Update(void *c);
void Presenter_TrackStylus(char *c);
int Presenter_Load(char *c);
void FallingPetal_UpdateMatrix(char *c);
void FallingPetal_Draw(char *c);
void FallingPetal_Update(char *self);
void FallingPetal_Spawn(FallingPetal *petal, const VecFx32 *pos, const VecFx32 *vel);
void FallingPetal_SetModel(char *t, int a1);
void FlowerFace_StateHate(void);
void FlowerFace_PlayHate(char *p);
void FlowerFace_StateLike(void);
void FlowerFace_PlayLike(char *p);
void FlowerFace_StatePick(int *c);
void FlowerFace_PlayPetalPulled(char *c);
void FlowerFace_StateIdle(void);
void FlowerFace_PlayIdle(char *p);
int FlowerFace_IsIdle(int *c);
void FlowerFace_Reset(void *a);
void FlowerFace_Draw(void *self);
void FlowerFace_Update(char *c);
int FlowerFace_Load(char *c);
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

namespace s020c3d88 {
extern "C" int FlowerFace_Load(char *c) {
    {
        void *bmd = dExtSimpleModel_c::LoadFile(FLOWERFACE(c)->mModelFile);
        FLOWERFACE(c)->mModel.SetFile((BMD_File *)bmd, 1, -1);
    }
    dExtFrameCtrl_c::LoadFile(FLOWERFACE(c)->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(FLOWERFACE(c)->mPickAnim);
    dExtFrameCtrl_c::LoadFile(FLOWERFACE(c)->mLikeAnim);
    dExtFrameCtrl_c::LoadFile(FLOWERFACE(c)->mHateAnim);
    FlowerFace_PlayIdle(c);
    int petalFile = (int)dExtSimpleModel_c::LoadFile(FLOWERFACE(c)->mPetalFile);
    int i = 0;
    char *p = (char *)FLOWERFACE(c)->mPetals;
    /* c steps a petal at a time too, so mPetals[0] is petal i. */
    for (; i < 0x16; i++) {
        FallingPetal_SetModel(p, petalFile);
        FLOWERFACE(c)->mPetals[0].mActive = 0;
        p += sizeof(FallingPetal);
        c += sizeof(FallingPetal);
    }
    return 1;
}
}

namespace s020c3d18 {
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
extern "C" {

void FlowerFace_Update(char *c) {
    (((C *)c)->*(((C *)c)->pmf))();
    FLOWERFACE(c)->mModel.Advance();
    int i = 0;
    char *e = (char *)FLOWERFACE(c)->mPetals;
    /* c steps a petal at a time, so mPetals[0] is petal i. */
    do {
        if (FLOWERFACE(c)->mPetals[0].mActive != 0)
            FallingPetal_Update(e);
        i++;
        c += sizeof(FallingPetal);
        e += sizeof(FallingPetal);
    } while (i < 0x16);
}
}
}

namespace s020c3bf4 {

extern "C" {

extern int data_ov006_0213aee8[12];
}


extern "C" void FlowerFace_Draw(void *self) {
    char *c = (char *)self;
    struct Vector3 light;
    int i;

    light.x = 0;
    light.y = 0;
    light.z = -0xff8;
    func_0203cd80(&light, 0);
    func_0203ccd4(&light, 0x600);

    G3_LightVector(GX_LIGHTID_0, light.x, light.y, light.z);

    func_02016a14(&FLOWERFACE(c)->mModel, 0x2bff);
    func_02016a04(&FLOWERFACE(c)->mModel, 0x1211);

    FLOWERFACE(c)->mModel.mat4x3 = *(struct Matrix4x3 *)data_ov006_0213aee8;

    FLOWERFACE(c)->mModel.Render(0);

    i = 0;
    {
        char *p = (char *)FLOWERFACE(c)->mPetals;
        /* c steps a petal at a time, so mPetals[0] is petal i. */
        for (; i < 0x16; i++) {
            if (FLOWERFACE(c)->mPetals[0].mActive != 0)
                FallingPetal_Draw(p);
            c += sizeof(FallingPetal);
            p += sizeof(FallingPetal);
        }
    }
}
}

namespace s020c3bc8 {
extern "C" {

void FlowerFace_Reset(void *a) {
    int i = 0;
    char *p = (char *)a;
    /* p steps a petal at a time, so mPetals[0] is petal i. */
    for (; i < 22; i++) {
        FLOWERFACE(p)->mPetals[0].mActive = 0;
        p += sizeof(FallingPetal);
    }
    FlowerFace_PlayIdle((char *)a);
}
}
}

namespace s020c3b80 {
extern "C" {

extern int pmf_FlowerFace_StateIdle_2[];
int FlowerFace_IsIdle(int *c) {
    int *idle = pmf_FlowerFace_StateIdle_2;
    int fn = c[0];
    int differs = 1;
    if (fn == idle[0]) {
        if (c[1] == idle[1] || fn == 0)
            differs = 0;
    }
    return differs == 0;
}
}
}

namespace s020c3b2c {
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
}

namespace s020c3adc {

struct S {
    int w[2];
};
extern "C" {

extern struct S pmf_FlowerFace_StateIdle;
void FlowerFace_PlayIdle(char *p) {
    FLOWERFACE(p)->mModel.SetAnim((BCA_File *)FLOWERFACE(p)->mWaitAnim.filePtr, 0, 0x800, 0);
    *(struct S *)FLOWERFACE(p)->mState = pmf_FlowerFace_StateIdle;
}
}
}

namespace s020c3ad8 {
extern "C" {

void FlowerFace_StateIdle(void) {}
}
}

namespace s020c3990 {

struct S8 {
    int w[2];
};
extern "C" {


extern int pmf_FlowerFace_StatePick[2];

void FlowerFace_PlayPetalPulled(char *c) {
    int i;
    char *p;
    struct {
        VecFx32 pos, vel, pos2, vel2;
    } b;

    FLOWERFACE(c)->mModel.SetAnim((BCA_File *)FLOWERFACE(c)->mPickAnim.filePtr, 0x40000000, 0x800,
                                  0);

    FLOWERFACE(c)->mModel.currFrame = 0; /* rewind */

    /* p steps a petal at a time, so FLOWERFACE(p)->mPetals[0] is the same
       petal as FLOWERFACE(c)->mPetals[i]. Spelling both the test and the spawn
       one way (either way) moves bytes. */
    p = c;
    for (i = 0; i < 0x16; i++, p += sizeof(FallingPetal)) {
        if (FLOWERFACE(p)->mPetals[0].mActive == 0) {
            b.pos.x = 0x2000;
            b.pos.y = 0xa000;
            b.pos.z = 0x1000;
            b.vel.x = 0x600;
            b.vel.y = 0xa00;
            b.vel.z = 0xa00;
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
            FallingPetal_Spawn(&FLOWERFACE(c)->mPetals[i], &b.pos2, &b.vel2);
            break;
        }
    }

    *(S8 *)c = *(S8 *)&pmf_FlowerFace_StatePick[0];
}
}
}

namespace s020c395c {
extern "C" {

void FlowerFace_StatePick(int *c) {
    if (FLOWERFACE(c)->mModel.Finished() == 0)
        return;
    FlowerFace_PlayIdle((char *)c);
}
}
}

namespace s020c3908 {

struct S {
    int w[2];
};
extern "C" {

extern struct S pmf_FlowerFace_StateLike;
void FlowerFace_PlayLike(char *p) {
    FLOWERFACE(p)->mModel.SetAnim((BCA_File *)FLOWERFACE(p)->mLikeAnim.filePtr, 0x40000000, 0x800,
                                  0);
    *(struct S *)FLOWERFACE(p)->mState = pmf_FlowerFace_StateLike;
}
}
}

namespace s020c3904 {
extern "C" {

void FlowerFace_StateLike(void) {}
}
}

namespace s020c38b0 {

struct S {
    int w[2];
};
extern "C" {

extern struct S pmf_FlowerFace_StateHate;
void FlowerFace_PlayHate(char *p) {
    FLOWERFACE(p)->mModel.SetAnim((BCA_File *)FLOWERFACE(p)->mHateAnim.filePtr, 0x40000000, 0x800,
                                  0);
    *(struct S *)FLOWERFACE(p)->mState = pmf_FlowerFace_StateHate;
}
}
}

namespace s020c38ac {
extern "C" {

void FlowerFace_StateHate(void) {}
}
}

namespace s020c3884 {
extern "C" {

void FallingPetal_SetModel(char *t, int a1) {
    PETAL(t)->mModel.SetFile((BMD_File *)a1, 1, -1);
    PETAL(t)->mActive = 0;
}
}
}

namespace s020c3754 {
extern "C" {

extern int data_02092768[4];

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
    petal->mRot[0] = data_02092768[0];
    petal->mRot[1] = data_02092768[1];
    petal->mRot[2] = data_02092768[2];
    petal->mRot[3] = data_02092768[3];
    Quaternion_FromVector3(petal->mRot, &sAxis, &petal->mAxis);
    petal->mActive = 1;
}
}
}

namespace s020c35e8 {

struct Quat {
    int w[4];
};
extern "C" {


extern Quat data_02092768;

void FallingPetal_Update(char *self) {
    Vector3 sway;
    Vector3 flow;
    Vector3 drag;
    Vector3 dir;
    Quat target;
    Quat turn;
    int phase;
    int wave;
    int dot;

    if (ApproachLinear2(PETAL(self)->mLife, 0, 1) != 0) {
        PETAL(self)->mActive = 0;
        return;
    }
    VEC_Add(&PETAL(self)->mPos, &PETAL(self)->mVel, &PETAL(self)->mPos);
    PETAL(self)->mAccel.y = -0x40;
    VEC_Add(&PETAL(self)->mVel, &PETAL(self)->mAccel, &PETAL(self)->mVel);
    phase = ((PETAL(self)->mPos.y >> 1) << 16) >> 20;
    wave = FX_SinIdx(phase);
    sway.x = wave;
    sway.y = 0;
    sway.z = wave;
    Vec3_MulScalarInPlace(&sway, 0x400);
    Vec3_Add(&flow, &PETAL(self)->mVel, &sway);
    dot = VEC_DotProduct(&PETAL(self)->mAxis, &flow);
    Vec3_MulScalar(&drag, &PETAL(self)->mAxis, dot);
    Vec3_MulScalarInPlace(&drag, 0x300);
    VEC_Subtract(&PETAL(self)->mVel, &drag, &PETAL(self)->mVel);
    dir.x = flow.x;
    dir.y = flow.y;
    dir.z = flow.z;
    if (NormalizeVec3IfNonZero(&dir) == 0)
        return;
    turn = data_02092768;
    Quaternion_FromVector3(&target, &PETAL(self)->mAxis, &dir);
    Quaternion_SLerp(&turn, &target, 0x200, &turn);
    Quaternion_Normalize(&turn);
    Quaternion_Multiply(PETAL(self)->mRot, &turn, PETAL(self)->mRot);
}
}
}

namespace s020c35a8 {

extern "C" void FallingPetal_Draw(char *c) {
    FallingPetal_UpdateMatrix(c);
    Vector3 v;
    v.z = 0x1000;
    v.y = 0x1000;
    v.x = 0x1000;
    PETAL(c)->mModel.Render(&v);
}
}

namespace s020c3528 {
extern "C" {

extern struct Vector3 sPetalUpAxis;
extern struct Matrix4x3 data_020a0e68;
void FallingPetal_UpdateMatrix(char *c) {
    struct Matrix4x3 rot;
    Matrix4x3_FromQuaternion(reinterpret_cast<const Quaternion *>(PETAL(c)->mRot), &rot);
    MTX_MultVec33(&sPetalUpAxis, &rot, &PETAL(c)->mAxis);
    Matrix4x3_FromTranslation(&data_020a0e68, PETAL(c)->mPos.x, PETAL(c)->mPos.y, PETAL(c)->mPos.z);
    MTX_Concat43(&rot, &data_020a0e68, &data_020a0e68);
    PETAL(c)->mModel.mat4x3 = data_020a0e68;
}
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

namespace s020c3050 {
extern "C" {

extern char sPresenterLoseLoopTexAnim[];
extern char sPresenterSmileTexAnim[];
extern char sPresenterLoseTexAnim[];
extern char sPresenterIdleTexAnim[];
extern char sPresenterMissTexAnim[];
extern char sPresenterTextureAnim[];
extern char sPresenterCheerTexAnim[];
extern char sPresenterMatchTexAnim[];
extern struct Matrix4x3 data_020a0e68;

int Presenter_Load(char *c) {
    void *faceFile;
    void *backFile;

    faceFile = dExtSimpleModel_c::LoadFile(PRESENTER(c)->mFaceModel);
    backFile = dExtSimpleModel_c::LoadFile(PRESENTER(c)->mBackModel);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mMistakeAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mWinAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mLoseAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mLoseStartAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mLoseEndAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mSmileAnim);
    dExtFrameCtrl_c::LoadFile(PRESENTER(c)->mSurprisedAnim);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mWaitTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mMistakeTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mWinTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mLoseTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mLoseStartTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mLoseEndTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mSmileTexSeq);
    dExtAnmTexPat_c::LoadFile(PRESENTER(c)->mSurprisedTexSeq);

    if (!PRESENTER(c)->mFace.SetFile((BMD_File *)faceFile, 1, -1))
        return 0;

    PRESENTER(c)->mFace.SetMaterialFlags(2);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)PRESENTER(c)->mMistakeTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)PRESENTER(c)->mWinTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile, *(BTP_File *)PRESENTER(c)->mLoseTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)PRESENTER(c)->mLoseStartTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)PRESENTER(c)->mLoseEndTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)PRESENTER(c)->mSmileTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)faceFile,
                             *(BTP_File *)PRESENTER(c)->mSurprisedTexSeq.filePtr);

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterIdleTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterMissTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterMatchTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterLoseLoopTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterLoseTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterTextureAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterCheerTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)faceFile, *(BTA_File *)sPresenterSmileTexAnim);

    Presenter_PlayIdle(c);
    PRESENTER(c)->mPos.x = 0;
    PRESENTER(c)->mPos.y = 0;
    PRESENTER(c)->mPos.z = 0;
    Presenter_UpdateMatrix(c);

    if (!PRESENTER(c)->mBack.SetFile((BMD_File *)backFile, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    PRESENTER(c)->mBack.mat4x3 = data_020a0e68;
    return 1;
}
}
}

namespace s020c2be8 {


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
            int sway0 = PRESENTER(c)->mSway[0]; \
            int sway2 = PRESENTER(c)->mSway[2]; \
            tiltY -= sway2; \
            int sway1 = PRESENTER(c)->mSway[1]; \
            int sway3 = PRESENTER(c)->mSway[3]; \
            PRESENTER(c)->mSway[0] += (tiltX - sway0) >> 2; \
            PRESENTER(c)->mSway[2] += tiltY >> 2; \
            sway1 = lift - sway1; \
            PRESENTER(c)->mSway[1] += sway1 >> 2; \
            sway3 = lift - sway3; \
            PRESENTER(c)->mSway[3] += sway3 >> 2; \
        } \
    }
extern "C" {

void Presenter_TrackStylus(char *c) {
    if (PRESENTER(c)->mTouchLocked != 0) {
        int x = PRESENTER(c)->mTouchX;
        int y = PRESENTER(c)->mTouchY;
        PRESENTER_TRACK(c, x, y);
        return;
    }
    if (Presenter_IsIdle(c) != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int x = gTouchX[i * 4];
            int y = gTouchY[i * 4];
            PRESENTER_TRACK(c, x, y);
            return;
        }
    }
    PRESENTER(c)->mSway[0] = FX_MUL(PRESENTER(c)->mSway[0], 0xE00);
    PRESENTER(c)->mSway[1] = FX_MUL(PRESENTER(c)->mSway[1], 0xE00);
    PRESENTER(c)->mSway[2] = FX_MUL(PRESENTER(c)->mSway[2], 0xE00);
    PRESENTER(c)->mSway[3] = FX_MUL(PRESENTER(c)->mSway[3], 0xE00);
}
}
}

namespace s020c2b8c {
extern "C" {

struct C;
}

typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
extern "C" {

void Presenter_Update(C *c) {
    Presenter_TrackStylus((char *)c);
    if (c->pmf)
        (c->*(c->pmf))();
    PRESENTER(c)->mTexSeq.Advance();
    PRESENTER(c)->mTexAnim.Advance();
    PRESENTER(c)->mFace.Advance();
    Presenter_UpdateMatrix((char *)c);
}
}
}

namespace s020c29dc {
extern "C" {

extern Vector3 sPresenterScale;
}

extern "C" void Presenter_Draw(char *c) {
    int keyLight[3];
    int fillLight[3];
    int *materials;
    char *components;

    keyLight[0] = 0;
    keyLight[1] = 0;
    keyLight[2] = -0xff8;
    fillLight[0] = 0;
    fillLight[1] = 0;
    fillLight[2] = -0xff8;
    func_02016a04(&PRESENTER(c)->mFace, 0x3d6d);
    func_0203cd80(keyLight, -0x2000);
    func_0203ccd4(keyLight, 0x2000);
    func_0203cd80(fillLight, 0x4200);
    func_0203ccd4(fillLight, 0x7000);

    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightVector(GX_LIGHTID_0, keyLight[0], keyLight[1], keyLight[2]);
    components = (char *)&PRESENTER(c)->mFace.data;
    G3_LightColor(GX_LIGHTID_1, GX_RGB(11, 10, 10));
    G3_LightVector(GX_LIGHTID_1, fillLight[0], fillLight[1], fillLight[2]);

    /* materials is mFace's material table (ModelComponents +0x4, 0x30-byte records);
       0x104..0x138 are +0x14/+0x18 of materials 5 and 6, which BMD_Material
       leaves in its unnamed pad_0c, so they stay offsets. */
    materials = *(int **)(components + 4);
    PRESENTER(c)->mTexSeq.Update(*(ModelComponents *)components);
    PRESENTER(c)->mTexAnim.Update(*(ModelComponents *)components);
    *(int *)((int)materials + 0x104) -= PRESENTER(c)->mSway[2];
    *(int *)((int)materials + 0x108) += PRESENTER(c)->mSway[3];
    *(int *)((int)materials + 0x134) += PRESENTER(c)->mSway[0];
    *(int *)((int)materials + 0x138) += PRESENTER(c)->mSway[1];
    PRESENTER(c)->mFace.Render(0);
    PRESENTER(c)->mBack.Render(&sPresenterScale);
}
}

namespace s020c2994 {

struct P {
    int x, y;
};
extern "C" {

extern struct P pmf_Presenter_StateIdle;
int Presenter_IsIdle(struct P *c) {
    int differs = 1;
    struct P *idle = &pmf_Presenter_StateIdle;
    if (c->x == idle->x) {
        if (c->y != idle->y && c->x != 0)
            ;
        else
            differs = 0;
    }
    return differs == 0;
}
}
}

namespace s020c2984 {
extern "C" {

/* Presenter_IsAnimDone @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b dExtFrameCtrl_c::Finished(). */

int Presenter_IsAnimDone(void *a) {
    return PRESENTER(a)->mFace.Finished();
}
}
}

namespace s020c2924 {
extern "C" {

extern dFdBrightness_c *gSceneFader;
void Presenter_ResetPose(char *c) {
    Presenter_PlayIdle(c);
    if (gSceneFader->IsAtEnd() == 0)
        return;
    PRESENTER(c)->mSway[1] = 0;
    PRESENTER(c)->mSway[0] = PRESENTER(c)->mSway[1];
    PRESENTER(c)->mSway[3] = 0;
    PRESENTER(c)->mSway[2] = PRESENTER(c)->mSway[3];
    /* Rewind each frame control to frame 0. */
    PRESENTER(c)->mFace.currFrame = 0;
    PRESENTER(c)->mTexSeq.currFrame = 0;
    PRESENTER(c)->mTexAnim.currFrame = 0;
}
}
}

namespace s020c2848 {
extern "C" {

extern char sPresenterIdleTexAnim[];
extern dFdBrightness_c *gSceneFader;
}

struct P2 {
    void *a;
    void *b;
};
extern "C" {

extern P2 pmf_Presenter_StateIdle_2;
}


extern "C" void Presenter_PlayIdle(char *c) {
    if (PRESENTER(c)->mFace.file != (BCA_File *)PRESENTER(c)->mWaitAnim.filePtr) {
        int blendFrames = 0;
        if (gSceneFader->IsAtEnd() == 0) {
            if (PRESENTER(c)->mFace.Finished() == 0 ||
                PRESENTER(c)->mFace.file == (BCA_File *)PRESENTER(c)->mSurprisedAnim.filePtr) {
                blendFrames = 8;
            }
        }
        PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mWaitAnim.filePtr, blendFrames, 0,
                                    0x800, 0);
        PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr, 0, 0x800, 0);
        PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
    }
    /* The volatile word reads are load-bearing: a plain struct copy moves two
       words (measured 2026-11-08). */
    {
        void *lo = ((volatile P2 *)&pmf_Presenter_StateIdle_2)->a;
        void *hi = ((volatile P2 *)&pmf_Presenter_StateIdle_2)->b;
        *(void **)&PRESENTER(c)->mState[0] = lo;
        *(void **)&PRESENTER(c)->mState[1] = hi;
    }
}
}

namespace s020c27c4 {
extern "C" {

extern char sPresenterIdleTexAnim[];
void Presenter_StateIdle(char *c) {
    if (!PRESENTER(c)->mFace.Finished())
        return;
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterIdleTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c271c {
extern "C" {

extern char sPresenterMatchTexAnim[];
extern long long pmf_Presenter_StateWin;
void Presenter_PlayMatch(void *c) {
    unsigned long long next;
    long long *state;
    if (PRESENTER(c)->mQuiet == 0) {
        func_02012174(0, 0x1c);
    }
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mWinAnim.filePtr, 4, 0x40000000, 0x800,
                                0);
    /* The state pointer and the unsigned copy are load-bearing: storing
       pmf_Presenter_StateWin straight through c costs the function its bytes
       (measured 2026-11-08). */
    state = (long long *)c;
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mWinTexSeq.filePtr, 0x40000000, 0x800,
                                  0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterMatchTexAnim, 0x40000000, 0x800, 0);
    next = pmf_Presenter_StateWin;
    *state = next;
}
}
}

namespace s020c26f4 {
extern "C" {

void Presenter_StateWin(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0)
        return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2664 {
extern "C" {

extern char sPresenterMissTexAnim[];
extern int pmf_Presenter_StateMistake[];
}


struct W2 {
    int w[2];
};
extern "C" {


void Presenter_PlayMiss(char *c) {
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mMistakeAnim.filePtr, 4, 0x40000000,
                                0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mMistakeTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterMissTexAnim, 0x40000000, 0x800, 0);
    *(struct W2 *)c = *(struct W2 *)pmf_Presenter_StateMistake;
}
}
}

namespace s020c263c {
extern "C" {

void Presenter_StateMistake(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0)
        return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2594 {
extern "C" {

extern char sPresenterCheerTexAnim[];
/* volatile is load-bearing: a plain long long load costs Presenter_PlayCheer
   its bytes (measured 2026-11-08). */
extern volatile long long pmf_Presenter_StateSurprisedThenSmile;
void Presenter_PlayCheer(void *c) {
    if ((PRESENTER(c)->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mSurprisedAnim.filePtr, 8, 0x40000000,
                                0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mSurprisedTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    *((long long *)c) = pmf_Presenter_StateSurprisedThenSmile;
}
}
}

namespace s020c24e4 {
extern "C" {

extern char sPresenterSmileTexAnim[];
void Presenter_StateSurprisedThenSmile(void *c) {
    if (!Presenter_IsAnimDone(c))
        return;
    if (PRESENTER(c)->mFace.file != (BCA_File *)PRESENTER(c)->mSurprisedAnim.filePtr)
        return;
    if (PRESENTER(c)->mQuiet == 0)
        Sound::PlayBank2_2D(0x1d1);
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mSmileAnim.filePtr, 8, 0, 0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mSmileTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterSmileTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c2440 {

struct G2 {
    int w[2];
};
extern "C" {

extern struct G2 pmf_Presenter_StateLoseStart;
extern char sPresenterLoseTexAnim[];
void Presenter_PlayLose(char *c) {
    if (PRESENTER(c)->mQuiet == 0)
        Sound::PlayBank2_2D(0x1c9);
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mLoseStartAnim.filePtr, 0, 0x40000000,
                                0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mLoseStartTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterLoseTexAnim, 0x40000000, 0x800, 0);
    *(struct G2 *)c = pmf_Presenter_StateLoseStart;
}
}
}

namespace s020c23a8 {
extern "C" {

extern char sPresenterLoseLoopTexAnim[];
void Presenter_StateLoseStart(void *c) {
    if (!Presenter_IsAnimDone(c))
        return;
    if (PRESENTER(c)->mFace.file != (BCA_File *)PRESENTER(c)->mLoseStartAnim.filePtr)
        return;
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mLoseAnim.filePtr, 0, 0, 0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mLoseTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterLoseLoopTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c2300 {
extern "C" {

extern char sPresenterCheerTexAnim[];
extern long long pmf_Presenter_StateSurprisedThenIdle;
void Presenter_PlayCheerLoop(void *c) {
    long long new_var;
    if ((PRESENTER(c)->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mSurprisedAnim.filePtr, 8, 0x40000000,
                                0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mSurprisedTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    /* The u64 identity mask is load-bearing (measured 2026-11-06); removing it
       costs the function its bytes. */
    *((long long *)c) = (new_var = pmf_Presenter_StateSurprisedThenIdle & 0xFFFFFFFFFFFFFFFF);
}
}
}

namespace s020c22d8 {
extern "C" {

void Presenter_StateSurprisedThenIdle(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0)
        return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2290 {
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Presenter_UpdateMatrix(char *c) {
    Matrix4x3_FromTranslation(&data_020a0e68, PRESENTER(c)->mPos.x, PRESENTER(c)->mPos.y,
                              PRESENTER(c)->mPos.z);
    PRESENTER(c)->mFace.mat4x3 = data_020a0e68;
}
}
}

namespace s020c225c {
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

namespace s020c2154 {
extern "C" {

void Mascot_Load(char *c) {
    void *f;
    f = dExtSimpleModel_c::LoadFile(MASCOT(c)->mModelFile);
    MASCOT(c)->mModel.SetFile((BMD_File *)f, 1, -1);
    dExtFrameCtrl_c::LoadFile(MASCOT(c)->mWaitAnimFile);
    MASCOT(c)->mModel.SetAnim(*(BCA_File *)MASCOT(c)->mWaitAnimFile.filePtr, 0, 0, 0x800, 0);
    MASCOT(c)->mModel.HideMaterial(7, 0);
    MASCOT(c)->mPos.x = 0xd000;
    MASCOT(c)->mPos.y = 0;
    MASCOT(c)->mPos.z = 0;
    MASCOT(c)->mRot[0] = 0;
    MASCOT(c)->mRot[1] = -0x1400;
    MASCOT(c)->mRot[2] = 0;
}
}
}

namespace s020c2144 {
extern "C" {

/* Mascot_Update @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b dExtBlendAnmModel_c::Advance(). */

void Mascot_Update(void *a) {
    MASCOT(a)->mModel.Advance();
}
}
}

namespace s020c201c {
typedef short s16;

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
    Mascot_UpdateMatrix((char *)c);
    c->mModel.dExtSimpleModel_c::Render(0);
}
}

namespace s020c1f4c {
typedef short s16;

extern "C" {

extern Matrix4x3 data_020a0e68;
}


extern "C" void Mascot_UpdateMatrix(char *c) {
    Matrix4x3_FromTranslation(&data_020a0e68, MASCOT(c)->mPos.x, MASCOT(c)->mPos.y,
                              MASCOT(c)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, MASCOT(c)->mRot[1]);
    MASCOT(c)->mModel.mat4x3 = data_020a0e68;
    MASCOT(c)->mModel.UpdateVerts();
    /* bone 6, the head */
    data_020a0e68 = MASCOT(c)->mModel.data.transforms[6];
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, MASCOT(c)->mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, MASCOT(c)->mHeadPitch);
    MASCOT(c)->mModel.data.transforms[6] = data_020a0e68;
}
}

namespace s020c1f04 {
extern "C" {

void Mascot_LookAt(char *c, int *src) {
    if (src != 0) {
        MASCOT(c)->mLookTarget.x = src[0];
        MASCOT(c)->mLookTarget.y = src[1];
        MASCOT(c)->mLookTarget.z = src[2];
    } else {
        MASCOT(c)->mLookTarget.x = 0;
        MASCOT(c)->mLookTarget.y = 0x2000;
        MASCOT(c)->mLookTarget.z = -0x2000;
    }
    MASCOT(c)->mLooking = 1;
}
}
}

namespace s020c1ef8 {
extern "C" {

void Mascot_StopLooking(int *p) {
    MASCOT(p)->mLooking = 0;
}
}
}

namespace s020c1eb4 {
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

namespace s020c1a88 {
extern "C" {

extern struct Vector3 data_020a0ebc;
extern struct Vector3_16 data_020a0edc;

int Dealer_Load(char *c) {
    void *file;
    BMD_Material *materials;

    dExtShadowModel_c::CleanAll();
    DEALER(c)->mModelData = (int)dExtSimpleModel_c::LoadFile(DEALER(c)->mModelFile);
    DEALER(c)->mTableData = (int)dExtSimpleModel_c::LoadFile(DEALER(c)->mTableFile);
    file = (void *)DEALER(c)->mModelData;
    if (file == 0 || DEALER(c)->mModel.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mWaitAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mShuffleStartAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mShuffleAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mDealStartAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mDealAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mDealEndAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mDealEnd2Anim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mDealWaitAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mThrowAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mWinStartAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mWinAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mWinEndAnim);
    dExtFrameCtrl_c::LoadFile(DEALER(c)->mLoseAnim);
    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    Dealer_ResetMatrices(c);
    DEALER(c)->mPos.x = data_020a0ebc.x;
    DEALER(c)->mPos.y = data_020a0ebc.y;
    DEALER(c)->mPos.z = data_020a0ebc.z;
    DEALER(c)->mRot[0] = data_020a0edc.x;
    DEALER(c)->mRot[1] = data_020a0edc.y;
    DEALER(c)->mRot[2] = data_020a0edc.z;
    DEALER(c)->mDealCounter = 0;
    DEALER(c)->mModel.HideMaterial(1, 0);
    Dealer_PlayIdle(c);
    file = (void *)DEALER(c)->mTableData;
    if (file == 0 || DEALER(c)->mTable.SetFile((BMD_File *)file, 1, -1) == 0) {
        return 0;
    }
    materials = (BMD_Material *)DEALER(c)->mTable.data.materials;
    materials[0].flags &= ~1;
    materials[5].flags &= ~1;
    materials[4].flags |= 2;
    MascotWalker_Load(&DEALER(c)->mWalker);
    DEALER(c)->mLampTimer = 0;
    DEALER(c)->mLampPattern = 1;
    DEALER(c)->mLightAngle = 0;
    return 1;
}
}
}

namespace s020c19d0 {
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
extern "C" {

extern short sDealerLampTimes[];
}


extern "C" void Dealer_Update(Dealer *dealer) {
    C *self = (C *)dealer;
    if (dealer->mState[0] != 0) {
        (self->*(self->pmf))();
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
}

namespace s020c1804 {


extern "C" void Dealer_Draw(void *self) {
    char *c = (char *)self;
    struct Vector3 keyLight, spinLight;
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

    angle = DEALER(c)->mLightAngle;
    func_0203ccd4(&spinLight, angle);

    G3_LightVector(GX_LIGHTID_0, keyLight.x, keyLight.y, keyLight.z);
    G3_LightVector(GX_LIGHTID_1, spinLight.x, spinLight.y, spinLight.z);

    func_02016a14(&DEALER(c)->mModel, 0x7fff);
    func_02016a04(&DEALER(c)->mModel, 0x4210);

    {
        short pattern = DEALER(c)->mLampPattern;
        unsigned char *p = lamp;
        for (i = 0; i < 3; i++) {
            if ((pattern >> i) & 1)
                *p = 0x1f;
            else
                *p = 0x14;
            p++;
        }
    }

    func_020169d8(&DEALER(c)->mTable, 0,
                  (unsigned short)(lamp[0] | (lamp[1] << 5) | (lamp[2] << 10)));
    func_020169d8(&DEALER(c)->mTable, 4, 0x6318);
    func_020169d8(&DEALER(c)->mTable, 5,
                  (unsigned short)(lamp[1] | (lamp[2] << 5) | (lamp[0] << 10)));

    DEALER(c)->mModel.Render(0);
    MascotWalker_Draw((char *)&DEALER(c)->mWalker);
    DEALER(c)->mTable.Render(0);

    dExtShadowModel_c::RenderAll();
}
}

namespace s020c1764 {
struct P2 {
    int w[2];
};

extern "C" {

extern P2 pmf_Dealer_StateIdle;
}


extern "C" void Dealer_PlayIdle(char *c) {
    if (DEALER(c)->mHand == 1) {
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealWaitAnim.filePtr, 0, 0, 0x800, 0);
        if (DEALER(c)->mDealSoundPlayed == 0) {
            Sound::PlayBank2_2D(0x13a);
            DEALER(c)->mDealSoundPlayed = 1;
        }
    } else {
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    }
    *(P2 *)c = pmf_Dealer_StateIdle;
}
}

namespace s020c1760 {
extern "C" {

void Dealer_StateIdle(void) {}
}
}

namespace s020c1718 {
extern "C" {

extern int pmf_Dealer_StateIdle_2[];
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
}

namespace s020c16b4 {
extern "C" {

extern int pmf_Dealer_StateIdle_3[];
int Dealer_IsIdleAtRest(char *c) {
    int *idle = pmf_Dealer_StateIdle_3;
    int fn = DEALER(c)->mState[0];
    int atRest = 0;
    int differs = 1;
    if (fn == idle[0] && (DEALER(c)->mState[1] == idle[1] || fn == 0))
        differs = 0;
    if (differs == 0 && DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mWaitAnim.filePtr)
        atRest = 1;
    return atRest;
}
}
}

namespace s020c1604 {

typedef short s16;
struct M8 {
    int w[2];
};
extern "C" {

extern char pmf_Dealer_StateShuffle[];

void Dealer_StartShuffle(Dealer *dealer, int unused, short count, s16 *counter) {
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
    *(struct M8 *)dealer = *(struct M8 *)pmf_Dealer_StateShuffle;
}
}
}

namespace s020c14bc {

extern "C" void Dealer_StateShuffle(char *c) {
    if (DEALER(c)->mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        DEALER(c)->mShuffleSoundPlayed = 1;
    }

    if (DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mShuffleStartAnim.filePtr) {
        if (DEALER(c)->mModel.Finished())
            DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mShuffleAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }

    if (DEALER(c)->mModel.WillHitFrame(0)) {
        if (DEALER(c)->mShuffleLeft-- <= 0) {
            Dealer_StartDeal(c, DEALER(c)->mDealLeft, DEALER(c)->mDealCounter);
            return;
        }
    }

    if (!DEALER(c)->mModel.WillHitFrame(4))
        return;

    Sound::PlayBank2_2D(0x141);
    if (DEALER(c)->mShuffleLeft == DEALER(c)->mDealDoneAt) {
        dScMgBase_c *scene = gMgScene;
        if (scene)
            scene->OnDealDone();
    }
}
}

namespace s020c1420 {
struct P2 {
    int w[2];
};

extern "C" {

extern P2 pmf_Dealer_StateDeal;
}


extern "C" void Dealer_StartDeal(char *c, short count, s16 *counter) {
    DEALER(c)->mDealLeft = count;
    DEALER(c)->mDealCounter = counter;
    if (Dealer_IsIdle(DEALER(c)) &&
        DEALER(c)->mModel.file != (BCA_File *)DEALER(c)->mDealWaitAnim.filePtr) {
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mShuffleStartAnim.filePtr, 0, 0x40000000,
                                  0x800, 0);
    } else {
        DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealStartAnim.filePtr, 0, 0x40000000,
                                  0x800, 0);
    }
    *(P2 *)c = pmf_Dealer_StateDeal;
}
}

namespace s020c11c0 {
extern "C" {


void Dealer_StateDeal(char *c) {
    void *anim = DEALER(c)->mModel.file;

    if (anim == (void *)DEALER(c)->mDealStartAnim.filePtr) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealAnim.filePtr, 0, 0, 0x800, 0);
        }
    }

    anim = DEALER(c)->mModel.file;
    if (anim == (void *)DEALER(c)->mDealEndAnim.filePtr ||
        anim == (void *)DEALER(c)->mDealEnd2Anim.filePtr) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mDealCounter = 0;
            Dealer_PlayIdle(c);
        }
        return;
    }

    if (anim == (void *)DEALER(c)->mDealAnim.filePtr) {
        if (DEALER(c)->mModel.WillHitFrame(0)) {
            if (DEALER(c)->mDealLeft-- <= 0) {
                short state = DEALER(c)->mHand;
                if (state == 0) {
                    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEndAnim.filePtr, 0,
                                              0x40000000, 0x800, 0);
                    if (DEALER(c)->mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    DEALER(c)->mDealSoundPlayed = 1;
                    return;
                } else if (state == 2) {
                    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEnd2Anim.filePtr, 0,
                                              0x40000000, 0x800, 0);
                    if (DEALER(c)->mDealSoundPlayed != 0)
                        return;
                    Sound::PlayBank2_2D(0x13a);
                    DEALER(c)->mDealSoundPlayed = 1;
                    return;
                } else {
                    Dealer_PlayIdle(c);
                    return;
                }
            }
        }

        if (DEALER(c)->mDealCounter == 0)
            return;
        if (!DEALER(c)->mModel.WillHitFrame(5))
            return;
        Sound::PlayBank2_2D(0x142);
        *DEALER(c)->mDealCounter += 1;
        return;
    }

    if (anim == (void *)DEALER(c)->mShuffleStartAnim.filePtr) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealStartAnim.filePtr, 0, 0x40000000,
                                      0x800, 0);
        }
    }
}
}
}

namespace s020c1164 {

struct P2 {
    int w[2];
};
extern "C" {

extern P2 pmf_Dealer_StateReveal_2;

void Dealer_StartReveal(int *t, int count, s16 *counter) {
    DEALER(t)->mDealLeft = count;
    DEALER(t)->mDealCounter = counter;
    DEALER(t)->mModel.SetAnim(*(BCA_File *)DEALER(t)->mDealAnim.filePtr, 8, 0, 0x800, 0);
    *(P2 *)t = pmf_Dealer_StateReveal_2;
}
}
}

namespace s020c0f9c {

extern "C" void Dealer_StateReveal(void *cc) {
    char *c = (char *)cc;

    if (DEALER(c)->mModel.file != (BCA_File *)DEALER(c)->mDealAnim.filePtr)
        goto other;

    if (DEALER(c)->mModel.WillHitFrame(0)) {
        if (DEALER(c)->mDealLeft-- <= 0) {
            int hand;
            DEALER(c)->mDealCounter = 0;
            hand = DEALER(c)->mHand;
            if (hand == 0) {
                DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEndAnim.filePtr, 0,
                                          0x40000000, 0x800, 0);
                if (DEALER(c)->mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                DEALER(c)->mDealSoundPlayed = 1;
                return;
            } else if (hand == 2) {
                DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEnd2Anim.filePtr, 0,
                                          0x40000000, 0x800, 0);
                if (DEALER(c)->mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                DEALER(c)->mDealSoundPlayed = 1;
                return;
            } else {
                Dealer_PlayIdle(c);
                return;
            }
        }
    }

    if (DEALER(c)->mDealCounter == 0)
        return;
    if (!DEALER(c)->mModel.WillHitFrame(5))
        return;
    Sound::PlayBank2_2D(0x142);
    *DEALER(c)->mDealCounter += 1;
    return;

other:
    if (DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mDealEndAnim.filePtr ||
        DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mDealEnd2Anim.filePtr) {
        if (DEALER(c)->mModel.Finished())
            Dealer_PlayIdle(c);
    }
}
}

namespace s020c0f0c {
extern "C" {

extern int pmf_Dealer_StateDeal_2[2];
extern int pmf_Dealer_StateReveal[2];

int Dealer_IsBusy(int *c) {
    int *pmf = pmf_Dealer_StateDeal_2;
    int busy = 1;
    int notDealing = busy;
    int fn = *c;
    if (fn == *pmf) {
        if (c[1] == pmf[1] || fn == 0)
            notDealing = 0;
    }
    if (notDealing != 0) {
        int notRevealing = 1;
        pmf = pmf_Dealer_StateReveal;
        if (fn == *pmf) {
            if (c[1] == pmf[1] || fn == 0)
                notRevealing = 0;
        }
        if (notRevealing)
            busy = 0;
    }
    return busy;
}
}
}

namespace s020c0efc {
extern "C" {

/* Dealer_IsAnimDone @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b dExtFrameCtrl_c::Finished(). */

int Dealer_IsAnimDone(void *a) {
    return DEALER(a)->mModel.Finished();
}
}
}

namespace s020c0e8c {

struct P2 {
    int w[2];
};
extern "C" {

extern P2 pmf_Dealer_StateSwap;

void Dealer_StartSwap(int *t) {
    DEALER(t)->mModel.SetAnim(*(BCA_File *)DEALER(t)->mThrowAnim.filePtr, 0, 0x40000000, 0x800, 0);
    DEALER(t)->mModel.HideMaterial(2, 0);
    DEALER(t)->mModel.ShowMaterial(1, 0);
    *(P2 *)t = pmf_Dealer_StateSwap;
}
}
}

namespace s020c0df0 {

extern "C" void Dealer_StateSwap(char *c) {
    if (DEALER(c)->mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        DEALER(c)->mShuffleSoundPlayed = 1;
    }
    /* 0x6c is the dExtFrameCtrl_c inside mModel (its +0x50). */
    if (DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mThrowAnim.filePtr &&
        DEALER(c)->mModel.Finished()) {
        DEALER(c)->mModel.HideMaterial(1, 0);
        DEALER(c)->mModel.ShowMaterial(2, 0);
        return;
    }
    if (DEALER(c)->mModel.WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}
}

namespace s020c0d68 {

struct P2 {
    int w[2];
};
extern "C" {

extern P2 pmf_Dealer_StateLose;
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
    *(P2 *)c = pmf_Dealer_StateLose;
}
}

namespace s020c0ce8 {
extern "C" {

void Dealer_StateLose(char *c) {
    if (!DEALER(c)->mModel.Finished())
        return;
    if (DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mLoseAnim.filePtr) {
        Dealer_PlayIdle(c);
        return;
    }
    Sound::PlayBank2_2D(0x13c);
    DEALER(c)->mHand = 0;
    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mLoseAnim.filePtr, 0, 0x40000000, 0x800, 0);
}
}
}

namespace s020c0c80 {

struct G2 {
    int w[2];
};
extern "C" {

extern struct G2 pmf_Dealer_StateWin;
void Dealer_PlayWin(void *c) {
    Sound::PlayBank2_2D(0x13b);
    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mWinStartAnim.filePtr, 0, 0x40000000, 0x800,
                              0);
    DEALER(c)->mShuffleLeft = 5;
    *(struct G2 *)c = pmf_Dealer_StateWin;
}
}
}

namespace s020c0b74 {
extern "C" {

void Dealer_StateWin(char *p) {
    if (DEALER(p)->mModel.file == (BCA_File *)DEALER(p)->mWinStartAnim.filePtr &&
        DEALER(p)->mModel.Finished()) {
        DEALER(p)->mModel.SetAnim(*(BCA_File *)DEALER(p)->mWinAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }
    if (DEALER(p)->mModel.file == (BCA_File *)DEALER(p)->mWinAnim.filePtr &&
        DEALER(p)->mModel.WillHitFrame(0)) {
        DEALER(p)->mShuffleLeft -= 1;
        if (DEALER(p)->mShuffleLeft == 0) {
            DEALER(p)->mModel.SetAnim(*(BCA_File *)DEALER(p)->mWinEndAnim.filePtr, 0, 0x40000000,
                                      0x800, 0);
            return;
        }
    }
    if (DEALER(p)->mModel.file == (BCA_File *)DEALER(p)->mWinEndAnim.filePtr &&
        DEALER(p)->mModel.Finished()) {
        Dealer_PlayIdle(p);
    }
}
}
}

namespace s020c0af8 {
extern "C" {

extern struct Matrix4x3 sDealerIdentityMtx;
void Dealer_ResetMatrices(char *c) {
    struct Matrix4x3 identity = sDealerIdentityMtx;
    DEALER(c)->mModel.mat4x3 = sDealerIdentityMtx;
    DEALER(c)->mTable.mat4x3 = identity;
}
}
}

namespace s020c0aa8 {
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

namespace s020c092c {

extern "C" {

extern Vector3 data_020a0ebc;
}

extern "C" {

extern Vector3_16 data_020a0edc;
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
    MascotWalker_StartNextPath((char *)walker);
}
}

namespace s020c07e8 {
struct C;
typedef void (C::*PMF)();

extern "C" {

extern Vector3 sMascotWalkerOffset;
}

extern "C" {

extern int pmf_MascotWalker_StateWalk_2[2];
}


extern "C" void MascotWalker_Update(MascotWalker *walker) {
    ((C *)walker->*(*(PMF *)walker->mState))();

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
}

namespace s020c07a0 {
extern "C" {

void MascotWalker_Draw(char *t) {
    MascotWalker_UpdateMatrix((MascotWalker *)t);
    (&WALKER(t)->mShadow)->InitModel(&WALKER(t)->mModel.mat4x3, 0x3c000, 0x14000, 0x3c000, 0xc);
    /* dExtSimpleModel_c's own render, called directly (0x02016b78), as Mascot_Draw does. */
    WALKER(t)->mModel.dExtSimpleModel_c::Render(0);
}
}
}

namespace s020c06dc {
extern "C" {

extern Matrix4x3 data_020a0e68;
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
}

/* One of the four walks MascotWalker::mPath points at: a point count, then
   the points (coined). */
struct WalkPath {
    int count;
    Vector3 *points;
};

namespace s020c057c {
extern "C" {

extern WalkPath sMascotWalkerPaths[];
extern int pmf_MascotWalker_StateWalk[];
void MascotWalker_StartNextPath(char *c) {
    Vector3 toNext;
    Vector3 target;
    WALKER(c)->mModel.SetAnim(*(BCA_File *)WALKER(c)->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    if (WALKER(c)->mPathIndex < 3) {
        WALKER(c)->mPathIndex += 1;
    } else {
        WALKER(c)->mPathIndex = 0;
    }
    WALKER(c)->mPath = (int)&sMascotWalkerPaths[WALKER(c)->mPathIndex];
    WALKER(c)->mPathPoint = 0;
    {
        Vector3 *point = &((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint];
        WALKER(c)->mPos.x = point->x;
        WALKER(c)->mPos.y = point->y;
        WALKER(c)->mPos.z = point->z;
    }
    WALKER(c)->mPathPoint += 1;
    Vec3_Sub(&toNext, &WALKER(c)->mPos,
             &((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint]);
    WALKER(c)->mDir.x = toNext.x;
    WALKER(c)->mDir.y = toNext.y;
    WALKER(c)->mDir.z = toNext.z;
    if (NormalizeVec3IfNonZero(&WALKER(c)->mDir) == 0) {
        WALKER(c)->mDir.x = 0xb50;
        WALKER(c)->mDir.y = 0xb50;
        WALKER(c)->mDir.z = 0;
    }
    {
        Vector3 *point = &((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint];
        target.x = point->x;
        target.y = point->y;
        target.z = point->z;
    }
    WALKER(c)->mTargetAngle = (short)Vec3_HorzAngle(&WALKER(c)->mPos, &target);
    {
        int fn = pmf_MascotWalker_StateWalk[0];
        int adj = pmf_MascotWalker_StateWalk[1];
        /* The no-op ternary is load-bearing: plain stores, or a copy through
           two locals, move three words (measured 2026-11-08). */
        WALKER(c)->mState[0] = adj ? fn : fn;
        WALKER(c)->mState[1] = adj;
    }
}
}
}

namespace s020c0364 {
extern "C" {

void MascotWalker_StateWalk(char *c) {
    int dx, dz;
    int stepX, stepZ;
    int doneX, doneZ;

    UpdateAngle(WALKER(c)->mAngleY, WALKER(c)->mTargetAngle, 8, 0x100);

    dx = WALKER(c)->mDir.x;
    if (dx < 0)
        dx = -dx;
    stepX = FX_MUL(dx, 0x1a9);

    dz = WALKER(c)->mDir.z;
    if (dz < 0)
        dz = -dz;

    doneX = ApproachLinear(WALKER(c)->mPos.x,
                           ((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint].x, stepX);

    stepZ = FX_MUL(dz, 0x1a9);

    doneZ = ApproachLinear(WALKER(c)->mPos.z,
                           ((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint].z, stepZ);

    if (doneX != 0 && doneZ != 0) {
        if (WALKER(c)->mPathPoint < ((WalkPath *)WALKER(c)->mPath)->count - 1) {
            Vector3 dir;
            Vector3 next;

            WALKER(c)->mPathPoint++;

            Vec3_Sub(&dir, &WALKER(c)->mPos,
                     &((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint]);

            WALKER(c)->mDir.x = dir.x;
            WALKER(c)->mDir.y = dir.y;
            WALKER(c)->mDir.z = dir.z;

            if (NormalizeVec3IfNonZero(&WALKER(c)->mDir) == 0) {
                WALKER(c)->mDir.x = 0xb50;
                WALKER(c)->mDir.y = 0xb50;
                WALKER(c)->mDir.z = 0;
            }

            {
                Vector3 *point = &((WalkPath *)WALKER(c)->mPath)->points[WALKER(c)->mPathPoint];
                next.x = point->x;
                next.y = point->y;
                next.z = point->z;
            }

            WALKER(c)->mTargetAngle = Vec3_HorzAngle(&WALKER(c)->mPos, &next);
            return;
        }

        MascotWalker_StartNextPath(c);
        return;
    }

    if (ApproachLinear2(WALKER(c)->mStateTimer, 0, 1) == 0)
        return;

    if (WALKER(c)->mModel.WillHitFrame(0) == 0)
        return;

    MascotWalker_StartPause((int *)c);
}
}
}

namespace s020c0304 {

struct P2 {
    int w[2];
};
extern "C" {

extern P2 pmf_MascotWalker_StatePause;

void MascotWalker_StartPause(int *t) {
    WALKER(t)->mModel.SetAnim(*(BCA_File *)WALKER(t)->mPauseAnimFile.filePtr, 8, 0, 0x800, 0);
    WALKER(t)->mStateTimer = 0x78;
    WALKER(t)->mPaused = 1;
    *(P2 *)WALKER(t)->mState = pmf_MascotWalker_StatePause;
}
}
}

namespace s020c0264 {
struct P2 {
    int w[2];
};

extern "C" {

extern P2 pmf_MascotWalker_StateWalk_3;
}


extern "C" void MascotWalker_StatePause(char *c) {
    if (ApproachLinear2(WALKER(c)->mStateTimer, 0, 1) == 0)
        return;
    WALKER(c)->mModel.SetAnim(*(BCA_File *)WALKER(c)->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    int roll = RandomIntInternal(&data_0209e650.value);
    int frac = (int)((unsigned int)(roll & 0x7fffffff) >> 0x13);
    WALKER(c)->mStateTimer = (short)(((frac * 0x258) >> 12) + 0x258);
    WALKER(c)->mPaused = 0;
    *(P2 *)WALKER(c)->mState = pmf_MascotWalker_StateWalk_3;
}
}

namespace s020c0134 {
extern "C" {

/* Camera_UpdateMatrices at 0x020c0134
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern dClipper data_0209f43c;

void Camera_UpdateMatrices(MgCamera *camera) {
    struct Vector3 up;
    struct Vector3 forward;
    struct Vector3 right;
    struct Vector3 scratch0;
    struct Vector3 scratch1;

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
}
}

namespace s020bfff8 {
extern "C" {


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
}
}

namespace s020bfec0 {
extern "C" {


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
}
}
