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
void MulVec3Mat4x3(void *, void *, void *);
void Matrix4x3_ApplyInPlaceToRotationZ(void *, short);
void Matrix4x3_FromQuaternion(void *, void *);
void MulVec3Mat3x3(void *, void *, void *);
void MulMat4x3Mat4x3(void *, void *, void *);
void *__cxa_vec_cleanup(void *, int, int, void *);
void func_020169d8(void *, int, unsigned int);
void func_0203cebc(void *, void *, void *, void *);
void AddVec3(void *, void *, void *);
int DotVec3(void *, void *);
void Matrix4x3_ApplyInPlaceToRotationX(void *, short);
void Matrix4x3_ApplyInPlaceToRotationY(void *, short);
void Matrix4x3_FromTranslation(void *, int, int, int);
int NormalizeVec3IfNonZero(void *);
void Quaternion_FromVector3(void *, void *, void *);
void Quaternion_Normalize(void *);
void Quaternion_SLerp(void *, void *, int, void *);
int RandomIntInternal(int *);
void SubVec3(void *, void *, void *);
void Vec3_Add(void *, void *, void *);
short Vec3_HorzAngle(const void *, const void *);
short Vec3_VertAngle(const void *, const void *);
void Vec3_MulScalar(void *, void *, int);
void Vec3_MulScalarInPlace(int *, int);
void Vec3_Sub(void *, void *, void *);
void SharedFilePtr_Construct_TexSeq(void *, unsigned int);
void SharedFilePtr_Destruct_Anim(void *);
void SharedFilePtr_Destruct_TexSeq(void *);

void *__cxa_vec_ctor(void *, int, int, void *, void *);
void func_02012174(int, int);
void func_02016a04(void *, int);
void func_02016a14(void *, int);
void func_02016b24(void *, unsigned int);
void func_02017ab4(void *);
void *func_02017acc(void *, unsigned int);
void Quaternion_Multiply(void *, void *, void *);
void func_0203ccd4(void *, short);
void func_0203cd80(void *, short);
void Camera_ProjectToScreen16(char *r4, void *r1, short *r5);
void Camera_ProjectToScreen(char *r4, void *r1, int *r6, int *r5);
void Camera_UpdateMatrices(void *self);
void MascotWalker_StatePause(char *c);
void MascotWalker_StartPause(int *t);
void MascotWalker_StateWalk(char *c);
void MascotWalker_StartNextPath(char *c);
void MascotWalker_UpdateMatrix(MascotWalker *walker);
void MascotWalker_Draw(char *t);
void MascotWalker_Update(void *c);
void MascotWalker_Load(MascotWalker *walker);
void Camera_InitDealerView(void *self);
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
void Dealer_StartReveal(int *t, int a1, int a2);
void Dealer_StateDeal(char *c);
void Dealer_StartDeal(char *c, short arg1, int arg2);
void Dealer_StateShuffle(char *c);
void Dealer_StartShuffle(char *c, int unused, short a2, int a3);
int Dealer_IsIdleAtRest(char *c);
int Dealer_IsIdle(int *r0);
void Dealer_StateIdle(void);
void Dealer_PlayIdle(char *c);
void Dealer_Draw(void *self);
void Dealer_Update(Dealer *dealer);
int Dealer_Load(char *c);
void Camera_InitSlotsView(void *self);
void Mascot_StopLooking(int *p);
void Mascot_LookAt(char *c, int *src);
void Mascot_UpdateMatrix(char *c);
void Mascot_Draw(void *c);
void Mascot_Update(void *a);
void Mascot_Load(char *c);
void Camera_InitPresenterView(void *self);
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
void FallingPetal_Spawn(int *r6, int *r5, int *r4);
void FallingPetal_SetModel(char *t, int a1);
void FlowerFace_StateHate(void);
void FlowerFace_PlayHate(char *p);
void FlowerFace_StateLike(void);
void FlowerFace_PlayLike(char *p);
void FlowerFace_StatePick(int *c);
void FlowerFace_PlayPetalPulled(char *c);
void FlowerFace_StateIdle(void);
void FlowerFace_PlayIdle(char *p);
void Camera_InitFlowerView(void *self);
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
// @symbol _ZN12FallingPetalC1Ev
FallingPetal::FallingPetal() {}

/* ---- FlowerFace::FlowerFace ---- */
/* The petals, the model and the six file handles are members, so the
   compiler builds them in declaration order: the petal array through
   __cxa_vec_ctor with FallingPetal's own constructor and destructor. */
// @symbol _ZN10FlowerFaceC1Ev
FlowerFace::FlowerFace()
    : mModelFile(0x222), mPetalFile(0x201), mWaitAnim(0x224), mPickAnim(0x223), mLikeAnim(0x221),
      mHateAnim(0x220) {}

/* ---- FlowerFace::~FlowerFace ---- */
/* Releases every file; the members' destructors then run in reverse
   declaration order: the four animations, the two models' files, the
   model and the petal array. */
// @symbol _ZN10FlowerFaceD1Ev
FlowerFace::~FlowerFace() {
    mModelFile.Release();
    mPetalFile.Release();
    mWaitAnim.Release();
    mPickAnim.Release();
    mLikeAnim.Release();
    mHateAnim.Release();
}

/* ---- FallingPetal::~FallingPetal ---- */
// @symbol _ZN12FallingPetalD1Ev
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
    int m = (int)dExtSimpleModel_c::LoadFile(FLOWERFACE(c)->mPetalFile);
    int i = 0;
    int z = i;
    char *p = (char *)FLOWERFACE(c)->mPetals;
    /* c steps a petal at a time too, so mPetals[0] is petal i. */
    for (; i < 0x16; i++) {
        FallingPetal_SetModel(p, m);
        FLOWERFACE(c)->mPetals[0].mActive = z;
        p += sizeof(FallingPetal);
        c += sizeof(FallingPetal);
    }
    return 1;
}
}

namespace s020c3d18 {
extern "C" {
void FlowerFace_Update(char *c) {
    int v = *(int *)(c + 4);
    void *p = c + (v >> 1);
    int *fn;
    if (v & 1) {
        int *vt = *(int **)p;
        int off = *(int *)c;
        fn = (int *)*(int *)((char *)vt + off);
    } else {
        fn = (int *)*(int *)c;
    }
    ((void (*)(void *))fn)(p);
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
// @symbol FlowerFace_Draw

extern "C" {
extern "C" {
extern int data_ov006_0213aee8[12];
}
}

extern "C" void FlowerFace_Draw(void *self) {
    char *c = (char *)self;
    struct Vector3 v;
    unsigned int packed;
    int i;

    v.x = 0;
    v.y = 0;
    v.z = 0xfffff008;
    func_0203cd80(&v, 0);
    func_0203ccd4(&v, 0x600);

    packed = (((short)v.x >> 3) & 0x3ff) | ((((short)v.y >> 3) & 0x3ff) << 10) |
             ((((short)v.z >> 3) & 0x3ff) << 20);
    *(unsigned int *)0x40004c8 = packed;

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
    int *g = pmf_FlowerFace_StateIdle_2;
    int r2 = c[0];
    int ip = 1;
    if (r2 == g[0]) {
        if (c[1] == g[1] || r2 == 0)
            ip = 0;
    }
    return ip == 0;
}
}
}

namespace s020c3b2c {
extern "C" {
// @symbol Camera_InitFlowerView
/* Camera_InitFlowerView at 0x020c3b2c
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local {
    int data[12];
};

struct dCamera_c {
    struct Matrix4x3_local viewMat; /* 0x00 */
    char pad30[0x30];               /* 0x30 */
    struct Matrix4x3_local projMat; /* 0x60 */
    char pad90[0x10];               /* 0x90 */
    struct Vector3 target;          /* 0xa0 */
    struct Vector3 eye;             /* 0xac */
    short angle;                    /* 0xb8 */
};

void Camera_InitFlowerView(struct dCamera_c *self) {
    self->target.x = 0x1b000;
    self->target.y = 0x17600;
    self->target.z = -0x22f00;
    self->eye.x = 0x38000;
    self->eye.y = 0x22500;
    self->eye.z = -0x51200;
    self->angle = 0xb30;
    Camera_UpdateMatrices(self);
}
}
}

namespace s020c3adc {
extern "C" {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_FlowerFace_StateIdle;
}
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
extern "C" {
typedef struct {
    int x, y, z;
} Vec3;
struct S8 {
    int w[2];
};

extern "C" {
extern int data_0209e650;
}
extern "C" {
extern int pmf_FlowerFace_StatePick[2];
}

void FlowerFace_PlayPetalPulled(char *c) {
    int i;
    char *p;
    struct {
        Vec3 pos, vel, pos2, vel2;
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
                (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.y +=
                (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.z +=
                (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.pos2.x = b.pos.x;
            b.pos2.y = b.pos.y;
            b.pos2.z = b.pos.z;
            b.vel2.x = b.vel.x;
            b.vel2.y = b.vel.y;
            b.vel2.z = b.vel.z;
            FallingPetal_Spawn((int *)&FLOWERFACE(c)->mPetals[i], (int *)&b.pos2, (int *)&b.vel2);
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
extern "C" {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_FlowerFace_StateLike;
}
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
extern "C" {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_FlowerFace_StateHate;
}
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
extern "C" {
extern int data_0209e650;
}
extern "C" {
extern int data_02092768[4];
}

void FallingPetal_Spawn(int *r6, int *r5, int *r4) {
    /* Every petal spins about +Y. Constructed on the first spawn: mwcc emits
       the guard test, the three stores and the ~Vector3() registration here. */
    static Vector3 sAxis(0, 0x1000, 0);

    r6[0] = r5[0];
    r6[1] = r5[1];
    r6[2] = r5[2];
    r6[3] = r4[0];
    r6[4] = r4[1];
    r6[5] = r4[2];
    r6[6] = 0;
    r6[7] = -0x40;
    r6[8] = 0;
    PETAL(r6)->mLife = 0xb4;
    r6[9] = sAxis.x;
    r6[10] = sAxis.y;
    r6[11] = sAxis.z;
    func_0203cd80(&PETAL(r6)->mAxis, (short)RandomIntInternal(&data_0209e650));
    r6[12] = data_02092768[0];
    r6[13] = data_02092768[1];
    r6[14] = data_02092768[2];
    r6[15] = data_02092768[3];
    Quaternion_FromVector3(PETAL(r6)->mRot, &sAxis, (int *)&PETAL(r6)->mAxis);
    PETAL(r6)->mActive = 1;
}
}
}

namespace s020c35e8 {
extern "C" {
typedef struct {
    int x, y, z;
} Vec3;
struct Quat {
    int w[4];
};

extern "C" {
extern Quat data_02092768;
}
extern int data_02082214[];

void FallingPetal_Update(char *self) {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
    Quat q0;
    Quat q1;
    int idx;
    int val;
    int dot;

    if (ApproachLinear2(PETAL(self)->mLife, 0, 1) != 0) {
        PETAL(self)->mActive = 0;
        return;
    }
    AddVec3(&PETAL(self)->mPos, &PETAL(self)->mVel, &PETAL(self)->mPos);
    PETAL(self)->mAccel.y = -0x40;
    AddVec3(&PETAL(self)->mVel, &PETAL(self)->mAccel, &PETAL(self)->mVel);
    idx = ((PETAL(self)->mPos.y >> 1) << 16) >> 20;
    val = *(short *)&data_02082214[idx];
    v0.x = val;
    v0.y = 0;
    v0.z = val;
    Vec3_MulScalarInPlace((int *)&v0, 0x400);
    Vec3_Add(&v1, &PETAL(self)->mVel, &v0);
    dot = DotVec3(&PETAL(self)->mAxis, &v1);
    Vec3_MulScalar(&v2, &PETAL(self)->mAxis, dot);
    Vec3_MulScalarInPlace((int *)&v2, 0x300);
    SubVec3(&PETAL(self)->mVel, &v2, &PETAL(self)->mVel);
    v3.x = v1.x;
    v3.y = v1.y;
    v3.z = v1.z;
    if (NormalizeVec3IfNonZero(&v3) == 0)
        return;
    q1 = data_02092768;
    Quaternion_FromVector3(&q0, &PETAL(self)->mAxis, &v3);
    Quaternion_SLerp(&q1, &q0, 0x200, &q1);
    Quaternion_Normalize(&q1);
    Quaternion_Multiply(PETAL(self)->mRot, &q1, PETAL(self)->mRot);
}
}
}

namespace s020c35a8 {
// @symbol FallingPetal_Draw

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
// @symbol FallingPetal_UpdateMatrix
extern "C" {
struct Q {
    int a, b, c, d;
};

extern "C" {
extern struct Vector3 sPetalUpAxis;
}
extern "C" {
extern struct Matrix4x3 data_020a0e68;
}
void FallingPetal_UpdateMatrix(char *c) {
    struct Matrix4x3 sp;
    Matrix4x3_FromQuaternion(PETAL(c)->mRot, &sp);
    MulVec3Mat3x3(&sPetalUpAxis, &sp, &PETAL(c)->mAxis);
    Matrix4x3_FromTranslation(&data_020a0e68, PETAL(c)->mPos.x, PETAL(c)->mPos.y, PETAL(c)->mPos.z);
    MulMat4x3Mat4x3(&sp, &data_020a0e68, &data_020a0e68);
    PETAL(c)->mModel.mat4x3 = data_020a0e68;
}
}
}

/* ---- Presenter::Presenter ---- */
/* The four model parts and the eighteen file handles are members, so the
   compiler builds them in declaration order before the body runs. */
// @symbol _ZN9PresenterC1Ev
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
// @symbol _ZN9PresenterD1Ev
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
extern "C" {
extern char sPresenterLoseLoopTexAnim[];
extern char sPresenterSmileTexAnim[];
extern char sPresenterLoseTexAnim[];
extern char sPresenterIdleTexAnim[];
extern char sPresenterMissTexAnim[];
extern char sPresenterTextureAnim[];
extern char sPresenterCheerTexAnim[];
extern char sPresenterMatchTexAnim[];
}
// @symbol Presenter_Load
extern "C" {
extern struct Matrix4x3 data_020a0e68;
}

int Presenter_Load(char *c) {
    void *r5;
    void *r4;

    r5 = dExtSimpleModel_c::LoadFile(PRESENTER(c)->mFaceModel);
    r4 = dExtSimpleModel_c::LoadFile(PRESENTER(c)->mBackModel);
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

    if (!PRESENTER(c)->mFace.SetFile((BMD_File *)r5, 1, -1))
        return 0;

    func_02016b24(&PRESENTER(c)->mFace, 2);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mMistakeTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mWinTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mLoseTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mLoseStartTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mLoseEndTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mSmileTexSeq.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)r5, *(BTP_File *)PRESENTER(c)->mSurprisedTexSeq.filePtr);

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterIdleTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterMissTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterMatchTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterLoseLoopTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterLoseTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterTextureAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterCheerTexAnim);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)r5, *(BTA_File *)sPresenterSmileTexAnim);

    Presenter_PlayIdle(c);
    PRESENTER(c)->mPos.x = 0;
    PRESENTER(c)->mPos.y = 0;
    PRESENTER(c)->mPos.z = 0;
    Presenter_UpdateMatrix(c);

    if (!PRESENTER(c)->mBack.SetFile((BMD_File *)r4, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    PRESENTER(c)->mBack.mat4x3 = data_020a0e68;
    return 1;
}
}
}

namespace s020c2be8 {
extern "C" {

#define COMPUTE(c, s, t) \
    { \
        int q = ((s) << 12) / 255; \
        int r = ((t) << 12) / 192; \
        int w = 0x1000 - r; \
        int a; \
        int b; \
        if (q > 0x800) { \
            int d = (q - 0x800) << 1; \
            a = FX_MUL(d, -0x100); \
            b = FX_MUL(d, -0x260); \
        } else { \
            int d = (0x800 - q) << 1; \
            a = FX_MUL(d, 0x260); \
            b = FX_MUL(d, 0x100); \
        } \
        { \
            int m = FX_MUL(r, 0x20) + FX_MUL(w, 0x500); \
            int o1 = PRESENTER(c)->mSway[0]; \
            int o2 = PRESENTER(c)->mSway[2]; \
            b -= o2; \
            int o3 = PRESENTER(c)->mSway[1]; \
            int o4 = PRESENTER(c)->mSway[3]; \
            PRESENTER(c)->mSway[0] += (a - o1) >> 2; \
            PRESENTER(c)->mSway[2] += b >> 2; \
            o3 = m - o3; \
            PRESENTER(c)->mSway[1] += o3 >> 2; \
            o4 = m - o4; \
            PRESENTER(c)->mSway[3] += o4 >> 2; \
        } \
    }

void Presenter_TrackStylus(char *c) {
    if (PRESENTER(c)->mTouchLocked != 0) {
        int s = PRESENTER(c)->mTouchX;
        int t = PRESENTER(c)->mTouchY;
        COMPUTE(c, s, t);
        return;
    }
    if (Presenter_IsIdle(c) != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int s = gTouchX[i * 4];
            int t = gTouchY[i * 4];
            COMPUTE(c, s, t);
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
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
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
extern "C" {
extern Vector3 sPresenterScale;
}
}
extern "C" void Presenter_Draw(char *c) {
    int v0[3];
    int v1[3];
    int *p;
    char *mc;

    v0[0] = 0;
    v0[1] = 0;
    v0[2] = 0xfffff008;
    v1[0] = 0;
    v1[1] = 0;
    v1[2] = 0xfffff008;
    func_02016a04(&PRESENTER(c)->mFace, 0x3d6d);
    func_0203cd80(v0, -0x2000);
    func_0203ccd4(v0, 0x2000);
    func_0203cd80(v1, 0x4200);
    func_0203ccd4(v1, 0x7000);

    *(volatile int *)0x40004cc = 0x7fff;
    *(volatile int *)0x40004c8 = (((short)v0[0] >> 3) & 0x3ff) |
                                 ((((short)v0[1] >> 3) & 0x3ff) << 10) |
                                 ((((short)v0[2] >> 3) & 0x3ff) << 20);
    mc = (char *)&PRESENTER(c)->mFace.data;
    *(volatile int *)0x40004cc = 0x4000294b;
    *(volatile int *)0x40004c8 = (((short)v1[0] >> 3) & 0x3ff) |
                                 ((((short)v1[1] >> 3) & 0x3ff) << 10) |
                                 ((((short)v1[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    /* p is mFace's material table (ModelComponents +0x4, 0x30-byte records);
       0x104..0x138 are +0x14/+0x18 of materials 5 and 6, which BMD_Material
       leaves in its unnamed pad_0c, so they stay offsets. */
    p = *(int **)(mc + 4);
    PRESENTER(c)->mTexSeq.Update(*(ModelComponents *)mc);
    PRESENTER(c)->mTexAnim.Update(*(ModelComponents *)mc);
    *(int *)(((int)p + 0x104)) -= PRESENTER(c)->mSway[2];
    *(int *)(((int)p + 0x108)) += PRESENTER(c)->mSway[3];
    *(int *)(((int)p + 0x134)) += PRESENTER(c)->mSway[0];
    *(int *)(((int)p + 0x138)) += PRESENTER(c)->mSway[1];
    PRESENTER(c)->mFace.Render(0);
    PRESENTER(c)->mBack.Render(&sPresenterScale);
}
}

namespace s020c2994 {
extern "C" {
struct P {
    int x, y;
};
extern "C" {
extern struct P pmf_Presenter_StateIdle;
}
int Presenter_IsIdle(struct P *c) {
    int ip = 1;
    struct P *g = &pmf_Presenter_StateIdle;
    if (c->x == g->x) {
        if (c->y != g->y && c->x != 0)
            ;
        else
            ip = 0;
    }
    return ip == 0;
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
extern "C" {
extern dFdBrightness_c *data_0209f5bc;
}
void Presenter_ResetPose(char *c) {
    Presenter_PlayIdle(c);
    if (data_0209f5bc->IsAtEnd() == 0)
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
extern "C" {
extern void *sPresenterIdleTexAnim;
}
extern "C" {
extern dFdBrightness_c *data_0209f5bc;
}
struct P2 {
    void *a;
    void *b;
};
extern "C" {
extern P2 pmf_Presenter_StateIdle_2;
}
}

extern "C" void Presenter_PlayIdle(char *c) {
    if (PRESENTER(c)->mFace.file != (BCA_File *)PRESENTER(c)->mWaitAnim.filePtr) {
        int blendFrames = 0;
        if (data_0209f5bc->IsAtEnd() == 0) {
            if (PRESENTER(c)->mFace.Finished() == 0 ||
                PRESENTER(c)->mFace.file == (BCA_File *)PRESENTER(c)->mSurprisedAnim.filePtr) {
                blendFrames = 8;
            }
        }
        PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mWaitAnim.filePtr, blendFrames, 0,
                                    0x800, 0);
        PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr, 0, 0x800, 0);
        PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)&sPresenterIdleTexAnim, 0, 0x800, 0);
    }
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
extern "C" {
extern void *sPresenterIdleTexAnim;
}
void Presenter_StateIdle(char *c) {
    if (!PRESENTER(c)->mFace.Finished())
        return;
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mWaitAnim.filePtr, 0, 0, 0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mWaitTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)&sPresenterIdleTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c271c {
extern "C" {
extern "C" {
extern int sPresenterMatchTexAnim;
}
extern "C" {
extern long long pmf_Presenter_StateWin;
}
void Presenter_PlayMatch(void *c) {
    char *r4 = (char *)c;
    unsigned long long new_var;
    long long *new_var2;
    if ((PRESENTER(r4)->mQuiet) == 0) {
        func_02012174(0, 0x1c);
    }
    PRESENTER(r4)->mFace.SetAnim(*(BCA_File *)PRESENTER(r4)->mWinAnim.filePtr, 4, 0x40000000, 0x800,
                                 0);
    new_var2 = (long long *)r4;
    PRESENTER(r4)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(r4)->mWinTexSeq.filePtr, 0x40000000,
                                   0x800, 0);
    PRESENTER(r4)->mTexAnim.SetFile(*(BTA_File *)&sPresenterMatchTexAnim, 0x40000000, 0x800, 0);
    new_var = pmf_Presenter_StateWin;
    *new_var2 = new_var;
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
extern int sPresenterMissTexAnim[];
extern int pmf_Presenter_StateMistake[];

struct W2 {
    int w[2];
};

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
extern "C" {
extern int sPresenterCheerTexAnim;
}
extern "C" {
extern volatile long long pmf_Presenter_StateSurprisedThenSmile;
}
void Presenter_PlayCheer(void *c) {
    char *r4 = (char *)c;
    if ((PRESENTER(r4)->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    PRESENTER(r4)->mFace.SetAnim(*(BCA_File *)PRESENTER(r4)->mSurprisedAnim.filePtr, 8, 0x40000000,
                                 0x800, 0);
    PRESENTER(r4)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(r4)->mSurprisedTexSeq.filePtr, 0x40000000,
                                   0x800, 0);
    PRESENTER(r4)->mTexAnim.SetFile(*(BTA_File *)&sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    *((long long *)r4) = pmf_Presenter_StateSurprisedThenSmile;
}
}
}

namespace s020c24e4 {
extern "C" {
extern "C" {
extern int sPresenterSmileTexAnim;
}
void Presenter_StateSurprisedThenSmile(void *c) {
    char *r4 = (char *)c;
    if (!Presenter_IsAnimDone(r4))
        return;
    if (PRESENTER(r4)->mFace.file != (BCA_File *)PRESENTER(r4)->mSurprisedAnim.filePtr)
        return;
    if (PRESENTER(r4)->mQuiet == 0)
        Sound::PlayBank2_2D(0x1d1);
    PRESENTER(r4)->mFace.SetAnim(*(BCA_File *)PRESENTER(r4)->mSmileAnim.filePtr, 8, 0, 0x800, 0);
    PRESENTER(r4)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(r4)->mSmileTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(r4)->mTexAnim.SetFile(*(BTA_File *)&sPresenterSmileTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c2440 {
extern "C" {
struct G2 {
    int w[2];
};
extern "C" {
extern struct G2 pmf_Presenter_StateLoseStart;
}
extern "C" {
extern void *sPresenterLoseTexAnim;
}
void Presenter_PlayLose(char *c) {
    if (PRESENTER(c)->mQuiet == 0)
        Sound::PlayBank2_2D(0x1c9);
    PRESENTER(c)->mFace.SetAnim(*(BCA_File *)PRESENTER(c)->mLoseStartAnim.filePtr, 0, 0x40000000,
                                0x800, 0);
    PRESENTER(c)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(c)->mLoseStartTexSeq.filePtr, 0x40000000,
                                  0x800, 0);
    PRESENTER(c)->mTexAnim.SetFile(*(BTA_File *)&sPresenterLoseTexAnim, 0x40000000, 0x800, 0);
    *(struct G2 *)c = pmf_Presenter_StateLoseStart;
}
}
}

namespace s020c23a8 {
extern "C" {
extern "C" {
extern int sPresenterLoseLoopTexAnim;
}
void Presenter_StateLoseStart(void *c) {
    char *r4 = (char *)c;
    if (!Presenter_IsAnimDone(r4))
        return;
    if (PRESENTER(r4)->mFace.file != (BCA_File *)PRESENTER(r4)->mLoseStartAnim.filePtr)
        return;
    PRESENTER(r4)->mFace.SetAnim(*(BCA_File *)PRESENTER(r4)->mLoseAnim.filePtr, 0, 0, 0x800, 0);
    PRESENTER(r4)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(r4)->mLoseTexSeq.filePtr, 0, 0x800, 0);
    PRESENTER(r4)->mTexAnim.SetFile(*(BTA_File *)&sPresenterLoseLoopTexAnim, 0, 0x800, 0);
}
}
}

namespace s020c2300 {
extern "C" {
extern "C" {
extern int sPresenterCheerTexAnim;
}
extern "C" {
extern long long pmf_Presenter_StateSurprisedThenIdle;
}
void Presenter_PlayCheerLoop(void *c) {
    long long new_var;
    char *r4 = (char *)c;
    if ((PRESENTER(c)->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    PRESENTER(r4)->mFace.SetAnim(*(BCA_File *)PRESENTER(r4)->mSurprisedAnim.filePtr, 8, 0x40000000,
                                 0x800, 0);
    PRESENTER(r4)->mTexSeq.SetFile(*(BTP_File *)PRESENTER(r4)->mSurprisedTexSeq.filePtr, 0x40000000,
                                   0x800, 0);
    PRESENTER(r4)->mTexAnim.SetFile(*(BTA_File *)&sPresenterCheerTexAnim, 0x40000000, 0x800, 0);
    *((long long *)r4) = (new_var = pmf_Presenter_StateSurprisedThenIdle & 0xFFFFFFFFFFFFFFFF);
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
// @symbol Presenter_UpdateMatrix
extern "C" {
extern struct Matrix4x3 data_020a0e68;
}
void Presenter_UpdateMatrix(char *c) {
    Matrix4x3_FromTranslation(&data_020a0e68, PRESENTER(c)->mPos.x, PRESENTER(c)->mPos.y,
                              PRESENTER(c)->mPos.z);
    PRESENTER(c)->mFace.mat4x3 = data_020a0e68;
}
}
}

namespace s020c225c {
extern "C" {
// @symbol Camera_InitPresenterView
/* Camera_InitPresenterView at 0x020c225c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

struct Matrix4x3_local {
    int data[12];
};

struct dCamera_c {
    struct Matrix4x3_local viewMat; /* 0x00 */
    char pad30[0x30];               /* 0x30 */
    struct Matrix4x3_local projMat; /* 0x60 */
    char pad90[0x10];               /* 0x90 */
    struct Vector3 target;          /* 0xa0 */
    struct Vector3 eye;             /* 0xac */
    short angle;                    /* 0xb8 */
};

void Camera_InitPresenterView(struct dCamera_c *self) {
    self->target.x = 0;
    self->target.y = 0;
    self->target.z = 0;
    self->eye.x = 0;
    self->eye.y = 0;
    self->eye.z = 0x2d000;
    self->angle = 0x800;
    Camera_UpdateMatrices(self);
}
}
}

/* ---- Mascot::Mascot ---- */
/* The two file handles and the model are members, so the compiler builds
   them in declaration order before the body runs. */
// @symbol _ZN6MascotC1Ev
Mascot::Mascot() : mModelFile(0x205), mWaitAnimFile(0x206) {
    mLooking = 0;
}

/* ---- Mascot::~Mascot ---- */
/* Releases both files; the members' destructors then run in reverse
   declaration order: the model, the animation, the model file. */
// @symbol _ZN6MascotD1Ev
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
// @symbol Mascot_UpdateMatrix
typedef short s16;

extern "C" {
extern "C" {
extern Matrix4x3 data_020a0e68;
}
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
// @symbol Camera_InitSlotsView
/* Camera_InitSlotsView at 0x020c1eb4
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local {
    int data[12];
};

struct dCamera_c {
    struct Matrix4x3_local viewMat; /* 0x00 */
    char pad30[0x30];               /* 0x30 */
    struct Matrix4x3_local projMat; /* 0x60 */
    char pad90[0x10];               /* 0x90 */
    struct Vector3 target;          /* 0xa0 */
    struct Vector3 eye;             /* 0xac */
    short angle;                    /* 0xb8 */
};

void Camera_InitSlotsView(struct dCamera_c *self) {
    self->target.x = 0;
    self->target.y = 0xe000;
    self->target.z = -0x22f00;
    self->eye.x = 0;
    self->eye.y = 0xf000;
    self->eye.z = 0x32000;
    self->angle = 0xb00;
    Camera_UpdateMatrices(self);
}
}
}

/* ---- Dealer::Dealer ---- */
/* mModel, mTable, mWalker and the two model files are members with
   constructors, so the compiler builds them in declaration order; the
   animation handles take their files in the body. */
// @symbol _ZN6DealerC1Ev
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
// @symbol _ZN6DealerD1Ev
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
// @symbol Dealer_Load
extern "C" {
extern struct Vector3 data_020a0ebc;
}
extern "C" {
extern struct Vector3_16 data_020a0edc;
}

int Dealer_Load(char *c) {
    void *f;
    void *ip;

    dExtShadowModel_c::CleanAll();
    DEALER(c)->mModelData = (int)dExtSimpleModel_c::LoadFile(DEALER(c)->mModelFile);
    DEALER(c)->mTableData = (int)dExtSimpleModel_c::LoadFile(DEALER(c)->mTableFile);
    f = ((void *)DEALER(c)->mModelData);
    if (f == 0 || DEALER(c)->mModel.SetFile((BMD_File *)f, 1, -1) == 0) {
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
    DEALER(c)->mDealtCount = 0;
    DEALER(c)->mModel.HideMaterial(1, 0);
    Dealer_PlayIdle(c);
    f = ((void *)DEALER(c)->mTableData);
    if (f == 0 || DEALER(c)->mTable.SetFile((BMD_File *)f, 1, -1) == 0) {
        return 0;
    }
    ip = DEALER(c)->mTable.data.materials;
    ((BMD_Material *)ip)[0].flags &= ~1;
    ((BMD_Material *)ip)[5].flags &= ~1;
    ((BMD_Material *)ip)[4].flags |= 2;
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
extern short data_ov006_0212b89c[];
}

extern "C" void Dealer_Update(Dealer *dealer) {
    C *o = (C *)dealer;
    if (dealer->mState[0] != 0) {
        (o->*(o->pmf))();
    }
    dealer->mModel.Advance();
    if (ApproachLinear2(dealer->mLampTimer, 0, 1) != 0) {
        short *p = &dealer->mLampPattern;
        *p = *p + 1;
        if (dealer->mLampPattern > 7)
            dealer->mLampPattern = 1;
        dealer->mLampTimer = data_ov006_0212b89c[dealer->mLampPattern];
    }
    short *q = &dealer->mLightAngle;
    *q = *q + 0x400;
    MascotWalker_Update(&dealer->mWalker);
}
}

namespace s020c1804 {
// @symbol Dealer_Draw


extern "C" void Dealer_Draw(void *self) {
    char *c = (char *)self;
    struct Vector3 v1, v2;
    unsigned int p1, p2;
    short ang;
    unsigned char b[3];
    int i;

    v1.x = 0;
    v1.y = 0;
    v1.z = 0xfffff008;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0xfffff008;
    func_0203cd80(&v1, -0x1000);
    func_0203ccd4(&v1, 0x1000);

    ang = DEALER(c)->mLightAngle;
    func_0203ccd4(&v2, ang);

    p1 = (((short)v1.x >> 3) & 0x3ff) | ((((short)v1.y >> 3) & 0x3ff) << 10) |
         ((((short)v1.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int *)0x40004c8 = p1;

    p2 = (((short)v2.x >> 3) & 0x3ff) | ((((short)v2.y >> 3) & 0x3ff) << 10) |
         ((((short)v2.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int *)0x40004c8 = p2 | 0x40000000;

    func_02016a14(&DEALER(c)->mModel, 0x7fff);
    func_02016a04(&DEALER(c)->mModel, 0x4210);

    ang = DEALER(c)->mLampPattern;
    {
        unsigned char *p = b;
        for (i = 0; i < 3; i++) {
            if ((ang >> i) & 1)
                *p = 0x1f;
            else
                *p = 0x14;
            p++;
        }
    }

    func_020169d8(&DEALER(c)->mTable, 0, (unsigned short)(b[0] | (b[1] << 5) | (b[2] << 10)));
    func_020169d8(&DEALER(c)->mTable, 4, 0x6318);
    func_020169d8(&DEALER(c)->mTable, 5, (unsigned short)(b[1] | (b[2] << 5) | (b[0] << 10)));

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
int Dealer_IsIdle(int *r0) {
    int *g = pmf_Dealer_StateIdle_2;
    int w0 = r0[0];
    int ip = 1;
    if (w0 == g[0]) {
        if (r0[1] == g[1] || w0 == 0)
            ip = 0;
    }
    return ip == 0;
}
}
}

namespace s020c16b4 {
extern "C" {
extern int pmf_Dealer_StateIdle_3[];
int Dealer_IsIdleAtRest(char *c) {
    int *G = pmf_Dealer_StateIdle_3;
    int r3 = DEALER(c)->mState[0];
    int r4 = 0;
    int lr = 1;
    if (r3 == G[0] && (DEALER(c)->mState[1] == G[1] || r3 == 0))
        lr = 0;
    if (lr == 0 && DEALER(c)->mModel.file == (BCA_File *)DEALER(c)->mWaitAnim.filePtr)
        r4 = 1;
    return r4;
}
}
}

namespace s020c1604 {
extern "C" {
typedef short s16;
struct M8 {
    int w[2];
};
extern char pmf_Dealer_StateShuffle[];

void Dealer_StartShuffle(char *c, int unused, short a2, int a3) {
    if (DEALER(c)->mShuffleRound == 0) {
        DEALER(c)->mShuffleLeft = 4;
        DEALER(c)->mDealDoneAt = 3;
    } else {
        DEALER(c)->mShuffleLeft = 1;
        DEALER(c)->mDealDoneAt = 1;
    }
    if (ApproachLinear2(DEALER(c)->mShuffleRound, 5, 1) != 0) {
        DEALER(c)->mShuffleRound = 0;
    }
    DEALER(c)->mDealSoundPlayed = 0;
    DEALER(c)->mDealLeft = a2;
    DEALER(c)->mDealtCount = a3;
    DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mShuffleStartAnim.filePtr, 0, 0x40000000,
                              0x800, 0);
    *(struct M8 *)c = *(struct M8 *)pmf_Dealer_StateShuffle;
}
}
}

namespace s020c14bc {
struct V {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void OnDealDone(); /* slot 21 */
};

/* local extern: only slot 21, dScMgBase_c::OnDealDone, is called, through
   the view V above. */
extern "C" {
extern V *gMgScene;
}

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
        short cv = DEALER(c)->mShuffleLeft;
        short *p = &DEALER(c)->mShuffleLeft;
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            Dealer_StartDeal(c, DEALER(c)->mDealLeft, DEALER(c)->mDealtCount);
            return;
        }
    }

    if (!DEALER(c)->mModel.WillHitFrame(4))
        return;

    Sound::PlayBank2_2D(0x141);
    if (DEALER(c)->mShuffleLeft == DEALER(c)->mDealDoneAt) {
        V *o = gMgScene;
        if (o)
            o->OnDealDone();
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

extern "C" void Dealer_StartDeal(char *c, short arg1, int arg2) {
    DEALER(c)->mDealLeft = arg1;
    DEALER(c)->mDealtCount = arg2;
    if (Dealer_IsIdle((int *)c) &&
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
    void *f7c = DEALER(c)->mModel.file;

    if (f7c == ((void *)DEALER(c)->mDealStartAnim.filePtr)) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealAnim.filePtr, 0, 0, 0x800, 0);
        }
    }

    f7c = DEALER(c)->mModel.file;
    if (f7c == ((void *)DEALER(c)->mDealEndAnim.filePtr) ||
        f7c == ((void *)DEALER(c)->mDealEnd2Anim.filePtr)) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mDealtCount = 0;
            Dealer_PlayIdle(c);
        }
        return;
    }

    if (f7c == ((void *)DEALER(c)->mDealAnim.filePtr)) {
        if (DEALER(c)->mModel.WillHitFrame(0)) {
            short cv = DEALER(c)->mDealLeft;
            short *p = &DEALER(c)->mDealLeft;
            short old = *p;
            *p = old - 1;
            if (cv <= 0) {
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

        if (DEALER(c)->mDealtCount == 0)
            return;
        if (!DEALER(c)->mModel.WillHitFrame(5))
            return;
        Sound::PlayBank2_2D(0x142);
        {
            short *p = (short *)DEALER(c)->mDealtCount;
            *p = (short)(*p + 1);
        }
        return;
    }

    if (f7c == ((void *)DEALER(c)->mShuffleStartAnim.filePtr)) {
        if (DEALER(c)->mModel.Finished()) {
            DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealStartAnim.filePtr, 0, 0x40000000,
                                      0x800, 0);
        }
    }
}
}
}

namespace s020c1164 {
extern "C" {
struct P2 {
    int w[2];
};
extern "C" {
extern P2 pmf_Dealer_StateReveal_2;
}

void Dealer_StartReveal(int *t, int a1, int a2) {
    DEALER(t)->mDealLeft = a1;
    DEALER(t)->mDealtCount = a2;
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
        short cv = DEALER(c)->mDealLeft;
        short *p = &DEALER(c)->mDealLeft;
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            int a;
            DEALER(c)->mDealtCount = 0;
            a = DEALER(c)->mHand;
            if (a == 0) {
                DEALER(c)->mModel.SetAnim(*(BCA_File *)DEALER(c)->mDealEndAnim.filePtr, 0,
                                          0x40000000, 0x800, 0);
                if (DEALER(c)->mDealSoundPlayed != 0)
                    return;
                Sound::PlayBank2_2D(0x13a);
                DEALER(c)->mDealSoundPlayed = 1;
                return;
            } else if (a == 2) {
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

    if (DEALER(c)->mDealtCount == 0)
        return;
    if (!DEALER(c)->mModel.WillHitFrame(5))
        return;
    Sound::PlayBank2_2D(0x142);
    *(short *)DEALER(c)->mDealtCount += 1;
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
extern "C" {
extern int pmf_Dealer_StateDeal_2[2];
}
extern "C" {
extern int pmf_Dealer_StateReveal[2];
}

int Dealer_IsBusy(int *c) {
    int *p = pmf_Dealer_StateDeal_2;
    int r = 1;
    int m1 = r;
    int x = *c;
    if (x == *p) {
        if (c[1] == p[1] || x == 0)
            m1 = 0;
    }
    if (m1 != 0) {
        int m2 = 1;
        p = pmf_Dealer_StateReveal;
        if (x == *p) {
            if (c[1] == p[1] || x == 0)
                m2 = 0;
        }
        if (m2)
            r = 0;
    }
    return r;
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
extern "C" {
struct P2 {
    int w[2];
};
extern "C" {
extern P2 pmf_Dealer_StateSwap;
}

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
extern "C" {
extern double pmf_Dealer_StateLose;
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
    *(double *)c = pmf_Dealer_StateLose;
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
extern "C" {
struct G2 {
    int w[2];
};
extern "C" {
extern struct G2 pmf_Dealer_StateWin;
}
void Dealer_PlayWin(void *c) {
    char *r4 = (char *)c;
    Sound::PlayBank2_2D(0x13b);
    DEALER(r4)->mModel.SetAnim(*(BCA_File *)DEALER(r4)->mWinStartAnim.filePtr, 0, 0x40000000, 0x800,
                               0);
    DEALER(r4)->mShuffleLeft = 5;
    *(struct G2 *)r4 = pmf_Dealer_StateWin;
}
}
}

namespace s020c0b74 {
extern "C" {
typedef struct {
    char _pad[0x1d8];
    short field_1d8;
} Obj;

void Dealer_StateWin(char *p) {
    if (DEALER(p)->mModel.file == (BCA_File *)DEALER(p)->mWinStartAnim.filePtr &&
        DEALER(p)->mModel.Finished()) {
        DEALER(p)->mModel.SetAnim(*(BCA_File *)DEALER(p)->mWinAnim.filePtr, 0, 0, 0x800, 0);
        return;
    }
    if (DEALER(p)->mModel.file == (BCA_File *)DEALER(p)->mWinAnim.filePtr &&
        DEALER(p)->mModel.WillHitFrame(0)) {
        DEALER(p)->mShuffleLeft -= 1;
        if (((Obj *)p)->field_1d8 == 0) {
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
extern "C" {
extern volatile struct Matrix4x3 sDealerIdentityMtx;
}
// @symbol Dealer_ResetMatrices
void Dealer_ResetMatrices(char *c) {
    struct Matrix4x3 tmp;
    tmp = *(struct Matrix4x3 *)&sDealerIdentityMtx;
    DEALER(c)->mModel.mat4x3 = (struct Matrix4x3 &)sDealerIdentityMtx;
    DEALER(c)->mTable.mat4x3 = tmp;
}
}
}

namespace s020c0aa8 {
extern "C" {
// @symbol Camera_InitDealerView
/* Camera_InitDealerView at 0x020c0aa8
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local {
    int data[12];
};

struct dCamera_c {
    struct Matrix4x3_local viewMat; /* 0x00 */
    char pad30[0x30];               /* 0x30 */
    struct Matrix4x3_local projMat; /* 0x60 */
    char pad90[0x10];               /* 0x90 */
    struct Vector3 target;          /* 0xa0 */
    struct Vector3 eye;             /* 0xac */
    short angle;                    /* 0xb8 */
};

void Camera_InitDealerView(struct dCamera_c *self) {
    self->target.x = 0x200;
    self->target.y = 0x9600;
    self->target.z = -0x22f00;
    self->eye.x = -0x100;
    self->eye.y = 0x10500;
    self->eye.z = 0x1ee00;
    self->angle = 0xbb0;
    Camera_UpdateMatrices(self);
}
}
}

/* ---- MascotWalker::MascotWalker ---- */
/* The file handles, the model and the shadow are members, so the compiler
   constructs them in declaration order before the body runs. */
// @symbol _ZN12MascotWalkerC1Ev
MascotWalker::MascotWalker() : mModelFile(0x205), mPauseAnimFile(0x206), mWalkAnimFile(0x207) {
    mPathIndex = 0;
    mHeadAngle = 0;
    mPaused = 0;
}

/* ---- MascotWalker::~MascotWalker ---- */
/* Releases the three files; the members' destructors then run in reverse
   declaration order: shadow, model, walk, pause, model file. */
// @symbol _ZN12MascotWalkerD1Ev
MascotWalker::~MascotWalker() {
    mModelFile.Release();
    mWalkAnimFile.Release();
    mPauseAnimFile.Release();
}

namespace s020c092c {
// @symbol MascotWalker_Load

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
typedef int s16;
struct Vector3 {
    int x, y, z;
};

struct C;
typedef void (C::*PMF)();

struct C {
    char pad0[0xb4];
    PMF pmf; // 0xb4 (2 words)
    char pad1[0xc8 - 0xb4 - 8];
    Vector3 mPos; // 0xc8
    char pad2[0xea - 0xc8 - 12];
    short mAngleY; // 0xea
    char pad3[0xf0 - 0xea - 2];
    short mHeadAngle; // 0xf0
    char pad4[0xf4 - 0xf0 - 2];
    int mPaused; // 0xf4
};

extern "C" {
extern Vector3 sMascotWalkerOffset;
}
extern "C" {
extern "C" int pmf_MascotWalker_StateWalk_2[2];
}

extern "C" void MascotWalker_Update(C *c) {
    (c->*(c->pmf))();

    WALKER(c)->mModel.Advance();

    if (c->mPaused != 0) {
        Vector3 t;
        t.x = sMascotWalkerOffset.x;
        t.y = sMascotWalkerOffset.y;
        t.z = sMascotWalkerOffset.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->mPos, &t);
        d = (short)(ang - c->mAngleY);
        if (d < -0x3000)
            d = -0x3000;
        else if (d > 0x3000)
            d = 0x3000;
        UpdateAngle(c->mHeadAngle, d, 8, 0x200);

        {
            int *p = (int *)&c->pmf;
            int *g = pmf_MascotWalker_StateWalk_2;
            if (p[0] == g[0]) {
                if (p[1] == g[1])
                    return;
                if (((int *)&c->pmf)[0] == 0)
                    return;
            }
        }
        d = (short)(ang - d);
        UpdateAngle(c->mAngleY, d, 8, 0x200);
    } else {
        UpdateAngle(c->mHeadAngle, 0, 8, 0x200);
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
// @symbol MascotWalker_UpdateMatrix
typedef short s16;

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

namespace s020c057c {
typedef struct Pair {
    int a, b;
} Pair;
extern "C" {
extern int sMascotWalkerPaths[];
extern int pmf_MascotWalker_StateWalk[];
void MascotWalker_StartNextPath(char *c) {
    int sp8[3];
    int sp14[3];
    WALKER(c)->mModel.SetAnim(*(BCA_File *)WALKER(c)->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    if (WALKER(c)->mPathIndex < 3) {
        WALKER(c)->mPathIndex += 1;
    } else {
        WALKER(c)->mPathIndex = 0;
    }
    WALKER(c)->mPath = (int)((char *)sMascotWalkerPaths + (WALKER(c)->mPathIndex << 3));
    WALKER(c)->mPathPoint = 0;
    {
        int t = WALKER(c)->mPathPoint * 0xc;
        int *arr = (int *)((int *)WALKER(c)->mPath)[1];
        int *el = (int *)((char *)arr + t);
        WALKER(c)->mPos.x = el[0];
        WALKER(c)->mPos.y = el[1];
        WALKER(c)->mPos.z = el[2];
    }
    WALKER(c)->mPathPoint += 1;
    Vec3_Sub(sp8, &WALKER(c)->mPos,
             (char *)((int *)WALKER(c)->mPath)[1] + WALKER(c)->mPathPoint * 0xc);
    WALKER(c)->mDir.x = sp8[0];
    WALKER(c)->mDir.y = sp8[1];
    WALKER(c)->mDir.z = sp8[2];
    if (NormalizeVec3IfNonZero(&WALKER(c)->mDir) == 0) {
        WALKER(c)->mDir.x = 0xb50;
        WALKER(c)->mDir.y = 0xb50;
        WALKER(c)->mDir.z = 0;
    }
    {
        int t = WALKER(c)->mPathPoint * 0xc;
        int *arr = (int *)((int *)WALKER(c)->mPath)[1];
        int *el = (int *)((char *)arr + t);
        sp14[0] = el[0];
        sp14[1] = el[1];
        sp14[2] = el[2];
    }
    WALKER(c)->mTargetAngle = (short)Vec3_HorzAngle(&WALKER(c)->mPos, sp14);
    {
        int a = pmf_MascotWalker_StateWalk[0];
        int b = pmf_MascotWalker_StateWalk[1];
        WALKER(c)->mState[0] = b ? a : a;
        WALKER(c)->mState[1] = b;
    }
}
}
}

namespace s020c0364 {
extern "C" {
typedef short s16;
typedef long long s64;

typedef struct {
    int x, y, z;
} Vector3;

void MascotWalker_StateWalk(char *c) {
    char *path;
    char *arr;
    int idx;
    int a, b;
    int t1, t2;
    int r1, r2;

    UpdateAngle(WALKER(c)->mAngleY, WALKER(c)->mTargetAngle, 8, 0x100);

    a = WALKER(c)->mDir.x;
    if (a < 0)
        a = -a;
    t1 = FX_MUL(a, 0x1a9);

    b = WALKER(c)->mDir.z;
    if (b < 0)
        b = -b;

    idx = WALKER(c)->mPathPoint * 0xc;
    path = (char *)WALKER(c)->mPath;
    arr = *(char **)(path + 4);
    r1 = ApproachLinear(WALKER(c)->mPos.x, *(int *)(arr + idx), t1);

    t2 = FX_MUL(b, 0x1a9);

    idx = WALKER(c)->mPathPoint * 0xc;
    path = (char *)WALKER(c)->mPath;
    arr = *(char **)(path + 4);
    r2 = ApproachLinear(WALKER(c)->mPos.z, *(int *)(arr + idx + 8), t2);

    if (r1 != 0 && r2 != 0) {
        if (WALKER(c)->mPathPoint < *(int *)WALKER(c)->mPath - 1) {
            Vector3 dir;
            Vector3 next;
            int *p;

            p = &WALKER(c)->mPathPoint;
            *p = *p + 1;

            path = (char *)WALKER(c)->mPath;
            idx = WALKER(c)->mPathPoint * 0xc;
            arr = *(char **)(path + 4);
            Vec3_Sub(&dir, &WALKER(c)->mPos, (Vector3 *)(arr + idx));

            WALKER(c)->mDir.x = dir.x;
            WALKER(c)->mDir.y = dir.y;
            WALKER(c)->mDir.z = dir.z;

            if (NormalizeVec3IfNonZero(&WALKER(c)->mDir) == 0) {
                WALKER(c)->mDir.x = 0xb50;
                WALKER(c)->mDir.y = 0xb50;
                WALKER(c)->mDir.z = 0;
            }

            idx = WALKER(c)->mPathPoint * 0xc;
            path = (char *)WALKER(c)->mPath;
            arr = *(char **)(path + 4);
            next.x = *(int *)(arr + idx);
            next.y = *(int *)(arr + idx + 4);
            next.z = *(int *)(arr + idx + 8);

            WALKER(c)->mTargetAngle = Vec3_HorzAngle((const Vector3 *)&WALKER(c)->mPos, &next);
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
extern "C" {
struct P2 {
    int w[2];
};
extern "C" {
extern P2 pmf_MascotWalker_StatePause;
}

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
extern int data_0209e650;
}
extern "C" {
extern P2 pmf_MascotWalker_StateWalk_3;
}

extern "C" void MascotWalker_StatePause(char *c) {
    if (ApproachLinear2(WALKER(c)->mStateTimer, 0, 1) == 0)
        return;
    WALKER(c)->mModel.SetAnim(*(BCA_File *)WALKER(c)->mWalkAnimFile.filePtr, 0, 0, 0x800, 0);
    int r = RandomIntInternal(&data_0209e650);
    int idx = (int)((unsigned int)(r & 0x7fffffff) >> 0x13);
    WALKER(c)->mStateTimer = (short)(((idx * 0x258) >> 12) + 0x258);
    WALKER(c)->mPaused = 0;
    *(P2 *)WALKER(c)->mState = pmf_MascotWalker_StateWalk_3;
}
}

namespace s020c0134 {
extern "C" {
// @symbol Camera_UpdateMatrices
/* Camera_UpdateMatrices at 0x020c0134
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
typedef int s32;

struct Matrix4x3_local {
    int data[12];
};

struct dCamera_c {
    struct Matrix4x3_local viewMat; /* 0x00 */
    char pad30[0x30];               /* 0x30 */
    struct Matrix4x3_local projMat; /* 0x60 */
    char pad90[0x10];               /* 0x90 */
    /* Camera_UpdateMatrices hands 0xac to G3i::LookAt_ as the camera
     * position and 0xa0 as the point it looks at. The flower, slots and
     * dealer presets put 0xac above 0xa0; the presenter view is level. */
    struct Vector3 target; /* 0xa0 */
    struct Vector3 eye;    /* 0xac */
    short angle;           /* 0xb8 */
};

extern short data_02082214[];
extern dClipper data_0209f43c;

void Camera_UpdateMatrices(struct dCamera_c *self) {
    struct Vector3 up;
    struct Vector3 v1c;
    struct Vector3 v28;
    struct Vector3 v34;
    struct Vector3 v40;
    int idx;

    up.x = 0;
    up.y = 0x1000;
    up.z = 0;

    SubVec3(&self->target, &self->eye, &v1c);
    if (NormalizeVec3IfNonZero(&v1c)) {
        func_0203cebc(&v34, &v28, &v1c, &up);
        if (NormalizeVec3IfNonZero(&v28)) {
            func_0203cebc(&v40, &up, &v28, &v1c);
        } else {
            up.x = 0;
            up.y = 0;
            up.z = 0x1000;
        }
    }

    idx = (self->angle >> 4) * 2;
    G3i::PerspectiveW_(data_02082214[idx], data_02082214[idx + 1], 0x1555, 0x1000, 0x1388000,
                       0x1000, 1, (Matrix4x3 *)&self->projMat);

    G3i::LookAt_(&self->eye, &up, &self->target, true, (Matrix4x3 *)&self->viewMat);

    CopyToViewMat((const Matrix4x3 *)&self->viewMat);

    data_0209f43c.Func_020156DC(0x1555, self->angle, 0x1000, 0x1388000);
}
}
}

namespace s020bfff8 {
extern "C" {
extern short data_02082214[];

void Camera_ProjectToScreen(char *r4, void *r1, int *r6, int *r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = ((s020c0134::dCamera_c *)r4)->angle >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = cstd::fdiv(cosv, sinv);
    int r = FX_MUL(f1, -out[2]);
    int f2 = cstd::fdiv(out[1], r);
    int t1 = FX_MUL(f2, 0x5f800u);
    r5[0] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = FX_MUL(r, 0x1555u);
    int f3 = cstd::fdiv(out[0], g);
    int t3 = FX_MUL(f3, 0x7f800u);
    r6[0] = (t3 + 0x7f800) >> 0xc;
}
}
}

namespace s020bfec0 {
extern "C" {
extern short data_02082214[];

void Camera_ProjectToScreen16(char *r4, void *r1, short *r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = ((s020c0134::dCamera_c *)r4)->angle >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = cstd::fdiv(cosv, sinv);
    int r = FX_MUL(f1, -out[2]);
    int f2 = cstd::fdiv(out[1], r);
    int t1 = FX_MUL(f2, 0x5f800u);
    r5[1] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = FX_MUL(r, 0x1555u);
    int f3 = cstd::fdiv(out[0], g);
    int t3 = FX_MUL(f3, 0x7f800u);
    r5[0] = (t3 + 0x7f800) >> 0xc;
}
}
}
