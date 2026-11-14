//cpp
/*
 * dMgTrmpln2Mario_c and dMgTrmpln3DMario_c: the player-controlled Marios of
 * Trampoline2 and Trampoline, and the two jumper adapters the drawn TrmpLines
 * bounce (dMgTrmpRingObjAdapter_c / dMgTrmpObjAdapter_c). 88 functions,
 * ov006 .text 0x020c8dd4..0x020cd744, plus the unit's __sinit.
 *
 * ROM evidence: _ZTI17dMgTrmpln2Mario_c 0x0213b250 and
 * _ZTI18dMgTrmpln3DMario_c 0x0213b244 sit beside the adapters' typeinfos;
 * D1 0x020ca604 / 0x020ccfc8, constructors 0x020ca78c / 0x020cd12c. GetPos is
 * each class's key function, so this file emits both vtables and typeinfos and
 * the manifest discards them against the cartridge's copies. The state table
 * the cartridge keeps at 0x0213b0f4..0x0213b22c is a run of eight-byte
 * {handler, 0} records -- a member pointer with no this-adjustment -- so the
 * handlers are members of their class and mState holds one. Functions are in
 * ROM order under defer_codegen off; do not reorder.
 *
 * Leftovers: the pool walkers (Trmp2Mario_*All, MoveToStart, PlaceAtStart,
 * InitPool, Hide and the Trmp3DMario_* equivalents) are module-level free
 * functions called from d_s_mg_trampoline{,2}.cpp and walk the pool by byte
 * offset, so they keep their char* pool and the MARIO2/MARIO3D view macro.
 * Their C-linkage declarations live in dMgTrmplnShared.h, which the scenes
 * include too; this file defines them without a linkage wrapper.
 * Trmp3DMario_EnterWalkIn and Trmp3DMario_IsOffstage are the scene's entry
 * points on the same byte view. The dispatch in
 * Update and the state tests are spelled out word by word: the typed forms do
 * not reproduce the cartridge. Measured on this tree, `(this->*fn)()` in
 * Update makes mwccarm call its out-of-line PMF helper and lose the
 * 0x104-byte shape, and `state != pmf` in IsActive grows it from 0xa4 to 0xb0
 * bytes; both DIFF.
 * The bounce states still walk the line in gotos and raw offsets -- that is
 * the matched state machine, not decoration. Helper prototypes with no owning
 * header (Puff_*, Sound_PlayBank1Panned, TrmpFx_Spawn, Vec3_MulScalar) stay
 * TU-local until include/ owns them; TrmpItem::UpdateWithJumper and
 * func_0203cd80 come from their headers, and VEC_Add/Subtract/DotProduct from
 * math/CalcLib.h.
 */

#include "Particle__System.h"
#include "ParticleIds.h"
#pragma defer_codegen off
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dExtAnmModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "Sound.h"
#include "dMgTrmpln2Mario_c.h"
#include "dMgTrmpln3DMario_c.h"
#include "TrmpItem.h"
#include "dMgTrmplnShared.h"
#include "MgCamera.h"

/* Typed view of a pool entry at a char* byte offset. Only the pool walkers
 * below use it; every other function is a member of one of the two classes. */
#define MARIO2(p) ((dMgTrmpln2Mario_c *)(p))
#define MARIO3D(p) ((dMgTrmpln3DMario_c *)(p))

/* Two 32-bit words: the shape every state record is copied in. */
struct W2 {
    int w[2];
};

/* The {code, this-adjustment} pair a state record holds; the adjustment is
 * applied as adj >> 1. */
struct Closure {
    int off;
    int adj;
};

/* The animation banks, loaded by the Load methods and released by the
 * destructors; the states select one through sTrmpBounceAnimTable. */
extern BCA_File *sTrmpAnimJump, *sTrmpAnimDoubleJump1, *sTrmpAnimWallJump, *sTrmpAnimHangUp,
    *sTrmpAnimRollJumpEnd, *sTrmpAnimWalk, *sTrmpAnimDoubleJump2, *sTrmpAnimJumpEnd, *sTrmpAnimTurn,
    *sTrmpAnimTurnEnd, *sTrmpAnimBounce, *sTrmpAnimStruggle, *sTrmpAnimSdown;
extern BCA_File **sTrmpBounceAnimTable[];

/* The model handle constructs through func_02017a24, the animation handles
 * through SharedFilePtr::Construct. Neither constructor nor destructor is
 * defined here. */
struct Trmpln2ModelFilePtr : SharedFilePtr {
    Trmpln2ModelFilePtr(unsigned int fileId);
    ~Trmpln2ModelFilePtr();
};
struct Trmpln2MarioAnimFilePtr : SharedFilePtr {
    Trmpln2MarioAnimFilePtr(unsigned int fileId);
    ~Trmpln2MarioAnimFilePtr();
};
typedef char Trmpln2ModelFilePtr_size[sizeof(Trmpln2ModelFilePtr) == 8 ? 1 : -1];
typedef char Trmpln2MarioAnimFilePtr_size[sizeof(Trmpln2MarioAnimFilePtr) == 8 ? 1 : -1];

/* A goal slot. Vector3(x, y, z) matches the first slot of each bank; the
 * other eleven store through an unsigned int*. Every slot registers one
 * destructor. ~TrmpGoal is not defined here. */
struct TrmpGoal {
    int x, y, z;
    TrmpGoal(int px, int py, int pz, int) {
        x = px;
        y = py;
        z = pz;
    }
    TrmpGoal(int px, int py, int pz) {
        unsigned int *p = (unsigned int *)this;
        p[0] = (unsigned int)px;
        p[1] = (unsigned int)py;
        p[2] = (unsigned int)pz;
    }
    ~TrmpGoal();
};
typedef char TrmpGoal_size[sizeof(TrmpGoal) == 0xc ? 1 : -1];

extern Trmpln2ModelFilePtr sTrmpMarioModelFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioJumpAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioDoubleJump1AnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioWallJumpAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioHangUpAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioRollJumpEndAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioWalkAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioDoubleJump2AnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioJumpEndAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioTurnAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioTurnEndAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioRollJumpAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioStruggleAnimFile;
extern Trmpln2MarioAnimFilePtr sTrmpMarioSdownAnimFile;

/* One declaration per symbol for the whole unit. The state records live in
 * unit020cd744; the tuning words and pools are defined at the bottom of this
 * file. Each record is declared in the shape its one consumer copies it in. */
/* .data / .rodata owned by other units */
extern Vector3 data_020a0ebc;
extern Vector3 sTrmp2MarioScale;
extern Vector3 sTrmp3DMarioScale;
extern MgCamera *sMgSubCamera;
extern int sTrmpRowBaseHeights[2];
extern short sTrmpGoalRow;
extern int sTrmp2Lives;
extern int sTrmp3DLives;

/* 2D state records (8 bytes each) */
extern int pmf_Trmp2Mario_StateMissed[2];
extern int pmf_Trmp2Mario_StateHidden_4[2];
extern int pmf_Trmp2Mario_StateHidden_5[2];
extern int pmf_Trmp2Mario_StateRespawnWait[2];
extern int pmf_Trmp2Mario_StateKnockedOut[2];
extern int pmf_Trmp2Mario_StateAppearWait_2[2];
extern int pmf_Trmp2Mario_StateBounce_2[2];
extern W2 pmf_Trmp2Mario_StateBounce;
extern W2 pmf_Trmp2Mario_StateRespawnWait_2;
extern double pmf_Trmp2Mario_StateMissed_2;
extern long long pmf_Trmp2Mario_StateWallBounce;
extern W2 pmf_Trmp2Mario_StateFall;
extern W2 pmf_Trmp2Mario_StateAppearWait;
extern double pmf_Trmp2Mario_StateHidden_3;
extern int pmf_Trmp2Mario_StateHidden_2[2];
extern int pmf_Trmp2Mario_StateHidden[2];

/* 3D state records (8 bytes each) */
extern int pmf_Trmp3DMario_StateMissed[];
extern int pmf_Trmp3DMario_StateOffstage_4[2];
extern int pmf_Trmp3DMario_StateGoal_2[2];
extern int pmf_Trmp3DMario_StateKnockedOut[2];
extern W2 pmf_Trmp3DMario_StateGoal;
extern W2 pmf_Trmp3DMario_StateLand;
extern W2 pmf_Trmp3DMario_StateFallToLedge;
extern int pmf_Trmp3DMario_StateBounce_2[2];
extern double pmf_Trmp3DMario_StateMissed_2;
extern W2 pmf_Trmp3DMario_StateFall;
extern double pmf_Trmp3DMario_StateWalkBack;
extern int pmf_Trmp3DMario_StateFallToLedge_2[2];
extern W2 pmf_Trmp3DMario_StateOnLedge;
extern W2 pmf_Trmp3DMario_StateOnGround;
extern W2 pmf_Trmp3DMario_StateWalkIn;
extern double pmf_Trmp3DMario_StateOnGround_2;
extern int pmf_Trmp3DMario_StateOffstage_3[];
extern W2 pmf_Trmp3DMario_StateOffstage_2;
extern int pmf_Trmp3DMario_StateOffstage[];

/* module state, defined at the bottom of this file */
extern int sTrmp2MarioCount;
extern int sTrmp2MarioTarget;
extern char *sTrmp2Marios;
extern int sTrmp3DMarioCount;
extern char *sTrmp3DMarios;
extern int sTrmp2FallSpeed;
extern int sTrmp2GravityStep;
extern int sTrmp2DriftScale;
extern int sTrmp2RiseSpeedMax;
extern int sTrmpSpeedScale;
extern int sTrmp3DFallSpeed;
extern int sTrmp3DGravityStep;
extern int sTrmp3DJumpOffSpeed;
extern int sTrmp3DBounceScale;
extern int sTrmp3DHardBounceScale;
extern int sTrmpRowHeights[2];
extern int sTrmpComboCount;
extern int sTrmpMariosInPlay;
extern int sTrmpMariosOnLedges;
extern int sTrmpGoalSlotCursor;
extern int sTrmpItemHitsOff;
extern int data_ov006_02140598;
extern int data_ov006_0214053c;
extern void *sTrmpJumpers[];
extern void *sTrmp2MarioPtrs[];
extern int sTrmpAnyOnLedge;

/* the goal-slot bank StateGoal walks; a run of separate 12-byte globals */
extern TrmpGoal sTrmpGoalSlots;

/* C callees owned by other units; no shared header declares them yet. */
extern "C" void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void Sound_PlayBank1Panned(int bank, int id, int panX);
void Puff_Spawn(int x, int y);
void Puff_DrawAll();
void Puff_UpdateAll();
void TrmpFx_Spawn(int y, int combo);

Vector3 *dMgTrmpln2Mario_c::GetVelocity() {
    return &mVel;
}

/* Neither missed nor hidden: the drawn line only bounces a Mario in play. */
int dMgTrmpln2Mario_c::IsActive() {
    int *p;
    int *d;
    int result;
    int flag;

    p = mState;
    d = pmf_Trmp2Mario_StateMissed;
    result = 0;
    flag = 1;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (mState[0] != 0)
                goto after1;
        }
        flag = 0;
    }
after1:
    if (flag != 0) {
        p = mState;
        d = pmf_Trmp2Mario_StateHidden_4;
        flag = 1;
        if (p[0] == d[0]) {
            if (p[1] != d[1]) {
                if (mState[0] != 0)
                    goto after2;
            }
            flag = 0;
        }
    after2:
        if (flag != 0)
            result = 1;
    }
    return result;
}

Vector3 *dMgTrmpln2Mario_c::GetPrevPos() {
    return &mPrevPos;
}

Vector3 *dMgTrmpln2Mario_c::GetPos() {
    return &mPos;
}

void dMgTrmpln2Mario_c::Render() {
    if (mVisible == 0)
        return;
    dExtAnmModel_c *model = &mAnmModel;
    model->Render(&sTrmp2MarioScale);
}

void dMgTrmpln2Mario_c::UpdateMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mRotY);
    mAnmModel.mat4x3 = data_020a0e68;
}

void dMgTrmpln2Mario_c::Update() {
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    if (mState[0] != 0) {
        /* Inline member-pointer call; the typed form does not reproduce
           it (see the file header). */
        Closure *cl = (Closure *)mState;
        void *tobj = (char *)this + (cl->adj >> 1);
        void (*fn)();
        if (cl->adj & 1)
            fn = *(void (**)())(*(char **)tobj + cl->off);
        else
            fn = (void (*)())cl->off;
        ((void (*)(void *))fn)(tobj);
    }
    ApplyGravity();
    Fix12i v = FX_MUL(mBounceVy, 0xb4b);
    if (mVel.y > v) {
        if (mAnmModel.file == sTrmpAnimBounce) {
            mParticleId = Particle::System::New(mParticleId, kPtclMgTrampolineTrail, mPos.x << 3,
                                                mPos.y << 3, mPos.z << 3, 0, 0);
        }
    }
    UpdateMatrix();
    mAnmModel.Advance();
}

void dMgTrmpln2Mario_c::ApplyGravity() {
    ApproachLinear(mVel.y, sTrmp2FallSpeed, mGravityStep);
    VEC_Add(&mPos, &mVel, &mPos);
}

void dMgTrmpln2Mario_c::StateKnockedOut() {
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    EnterHidden();
}

void dMgTrmpln2Mario_c::EnterByKind() {
    int *p;
    int *d;

    p = mState;
    d = pmf_Trmp2Mario_StateHidden_5;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (mState[0] != 0)
                goto check2;
        }
        return;
    }
check2:
    p = mState;
    d = pmf_Trmp2Mario_StateRespawnWait;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (mState[0] != 0)
                goto body;
        }
        EnterHidden();
        return;
    }
body:
    mGravityStep = 0x100;
    mVel.x = 0;
    mVel.y = 0x2000;
    D3D_PlaySeAtX(0x1c9, mPos.x);
    mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    mAnmModel.currFrame = 0;
    *(W2 *)mState = *(W2 *)&pmf_Trmp2Mario_StateKnockedOut;
}

void dMgTrmpln2Mario_c::StateBounce() {
    int v24;

    if (mVel.y <= 0) {
        EnterFall();
        return;
    }

    if (mAnmModel.file == sTrmpAnimBounce) {
        if (mAnmModel.WillHitFrame(0xc) != 0 || mAnmModel.WillHitFrame(0x18) != 0) {
            D3D_PlaySeAtX(0x1b5, mPos.x);
        }
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v24 = mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    mPos.x = v;
}

    if (mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        int t;

        mAnim = 2;
        mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

        mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, mAnim, mPos.x);

        t = FX_MUL(-mVel.x, 0x1200);
        if (t < -0x1400)
            t = -0x1400;
        else if (t > 0x1400)
            t = 0x1400;
        mVel.x = t;

        mVel.y += FX_MUL(sTrmpSpeedScale, 0x400);

        {
            int v40 = sTrmp2RiseSpeedMax;
            if (mVel.y <= v40)
                v40 = mVel.y;
            mVel.y = v40;
        }
    }
    goto L260;

L1a4: {
    int t = FX_MUL(-mVel.x, 0xf00);
    int v40;
    int hi;
    int lo;

    mVel.x = t;

    t = mPos.x;
    if (t < -0x68000)
        t = -0x68000;
    else if (t > 0x68000)
        t = 0x68000;
    mPos.x = t;

    hi = sTrmp2RiseSpeedMax;
    lo = sTrmp2FallSpeed;
    v40 = mVel.y;
    if (v40 < lo) {
        v40 = lo;
    } else {
        if (v40 <= hi)
            hi = v40;
        v40 = hi;
    }
    mVel.y = v40;
}
    goto L260;

L230:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L260: {
    int v = mVel.x;
    if (v > 0x2000) {
        ApproachLinear(mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)v, 0xc00);
    }
}
}

void dMgTrmpln2Mario_c::BounceOffLineA() {
    Vector3 tmp, tmp2, tmp3;
    int alongLine, lineHalfLength, dot, scale, fdivr, v, fx, cur;

    alongLine = mContactOffset;
    lineHalfLength = mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)&mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = VEC_DotProduct(&mVel, &tmp);
    scale = FX_MUL(dot, 0x1400);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    VEC_Subtract(&mVel, &tmp2, &mVel);

    fdivr = FX_Div(lineHalfLength, lineHalfLength + alongLine);
    scale = FX_MUL(sTrmp2RiseSpeedMax, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    VEC_Add(&mVel, &tmp3, &mVel);

    v = mVel.x;
    if (v < -0x3000)
        v = -0x3000;
    else if (v > 0x3000)
        v = 0x3000;
    mVel.x = v;

    fx = FX_MUL(sTrmp2RiseSpeedMax, 0xc00);
    cur = mVel.y;
    if (cur >= fx)
        fx = cur;
    mVel.y = fx;

    mGravityStep = sTrmp2GravityStep;

    {
        Vector3_16 vec16;
        int y = tmp.y;
        int z = tmp.z;
        int x = tmp.x;
        vec16.x = (s16)x;
        vec16.y = (s16)y;
        char *bp = (char *)mState;
        int *s = (int *)(bp);
        vec16.z = (s16)(z + (int)((unsigned)bp - (unsigned)bp));

        int *q = pmf_Trmp2Mario_StateAppearWait_2;
        if (s[0] != q[0] || (s[1] != q[1] && mState[0] != 0)) {

            Particle::System::New(0, kPtclMgTrampolineBounceA, (mPos.x) << 3, (mPos.y) << 3,
                                  (mPos.z) << 3, (const Vector3_16f *)(&vec16), 0);
            Particle::System::New(0, kPtclMgTrampolineBounceB, (mPos.x) << 3, (mPos.y) << 3,
                                  (mPos.z) << 3, (const Vector3_16f *)(&vec16), 0);
            mParticleId = Particle::System::New(mParticleId, kPtclMgTrampolineTrail, (mPos.x) << 3,
                                                (mPos.y) << 3, (mPos.z) << 3, 0, 0);
        }
    }

    mBounceVy = mVel.y;
    Clear();
    mWallBumpTimer = 0;
    mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);
    mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, 4, mPos.x);
    D3D_PlaySeAtX(0x1b5, mPos.x);
    pad_68 = 0;
    *(W2 *)mState = *(W2 *)&pmf_Trmp2Mario_StateBounce_2;
    StateBounce();
}

void dMgTrmpln2Mario_c::StateWallBounce() {
    int v24;

    if (mVel.y <= 0) {
        EnterFall();
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v24 = mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    mPos.x = v;
}

    if (mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        int t = FX_MUL(-mVel.x, 0x1200);
        int v40;

        if (t < -0x1400)
            t = -0x1400;
        else if (t > 0x1400)
            t = 0x1400;
        mVel.x = t;

        mVel.y += FX_MUL(sTrmpSpeedScale, 0x400);

        v40 = sTrmp2RiseSpeedMax;
        if (mVel.y <= v40)
            v40 = mVel.y;
        mVel.y = v40;

        mAnim = 2;
        mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

        mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, mAnim, mPos.x);
    }
    goto L260;

L1a4: {
    int t = FX_MUL(-mVel.x, 0xf00);
    int pos;
    int hi;
    int lo;
    int v40;

    mVel.x = t;

    pos = mPos.x;
    if (pos < -0x68000)
        pos = -0x68000;
    else if (pos > 0x68000)
        pos = 0x68000;
    mPos.x = pos;

    hi = sTrmp2RiseSpeedMax;
    lo = sTrmp2FallSpeed;
    v40 = mVel.y;
    if (v40 < lo) {
        v40 = lo;
    } else {
        if (v40 <= hi)
            hi = v40;
        v40 = hi;
    }
    mVel.y = v40;
}
    goto L260;

L230:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L260: {
    int v = mVel.x;
    if (v > 0x2000) {
        ApproachLinear(mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)v, 0xc00);
    }
}
}

void dMgTrmpln2Mario_c::BounceOffLineB() {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp3;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;
    int vx;

    alongLine = mContactOffset;
    lineHalfLength = mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)&mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = VEC_DotProduct(&mVel, &tmp);
    scale = FX_MUL(dot, 0x1200);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    VEC_Subtract(&mVel, &tmp2, &mVel);

    fdivr = FX_Div(lineHalfLength, lineHalfLength + alongLine);
    scale = FX_MUL(sTrmpSpeedScale, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    VEC_Add(&mVel, &tmp3, &mVel);

    vx = mVel.x;
    if (vx < -0x2800)
        vx = -0x2800;
    else if (vx > 0x2800)
        vx = 0x2800;
    mVel.x = vx;

    {
        int fx = FX_MUL(sTrmpSpeedScale, 0x600);
        int cur = mVel.y;
        if (cur >= fx)
            fx = cur;
        mVel.y = fx;
    }

    mGravityStep = sTrmp2GravityStep;
    Clear();

    mWallBumpTimer = 0;
    if (mAnim != 0) {
        mAnim = 0;
    } else {
        mAnim = 1;
    }

    mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

    mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, mAnim, mPos.x);

    *(W2 *)mState = *(W2 *)&pmf_Trmp2Mario_StateWallBounce_2;
    StateWallBounce();
}

void dMgTrmpln2Mario_c::EnterBounce() {
    mGravityStep = sTrmp2GravityStep;
    mVel.y = FX_MUL(sTrmpSpeedScale, 0xc00);
    mBounceVy = mVel.y;
    Clear();
    mWallBumpTimer = 0;
    mAnim = 0;
    mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);
    mAnmModel.currFrame = 0;
    D3D_PlaySeAtX(0x110, mPos.x);
    D3D_PlaySeAtX(0x1b5, mPos.x);
    pad_68 = 0;
    *(W2 *)mState = pmf_Trmp2Mario_StateBounce;
    StateBounce();
}

void dMgTrmpln2Mario_c::StateRespawnWait() {
    unsigned int r;
    int tbl;
    mTimer -= 1;
    if (mTimer == 0) {
        mPos.y = 0x100000;
        r = ((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13;
        mPos.x = ((int)r - 0x800) * 0xc0;
        mVel.y = 0;
        tbl = sTrmp2DriftScale;
        r = ((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13;
        mVel.x = FX_MUL(((int)r - 0x800) << 1, tbl);
        mAnmModel.SetAnim(sTrmpAnimWallJump, 0x40000000, 0x800, 0);
        mPrevPos.x = mPos.x;
        mPrevPos.y = mPos.y;
        mPrevPos.z = mPos.z;
        EnterFall();
    } else {
        mPos.y = 0x100000;
        mVel.y = 0;
    }
}

void dMgTrmpln2Mario_c::EnterRespawnWait() {
    mTimer = 0x3c;
    mPos.y = 0x100000;
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    mVel.y = 0;
    mAnmModel.SetAnim(sTrmpAnimWallJump, 0x40000000, 0x800, 0);
    *(W2 *)mState = pmf_Trmp2Mario_StateRespawnWait_2;
}

    /* `mTimer = mTimer - 1` comes out 4 bytes short; the decrement matches. */
void dMgTrmpln2Mario_c::StateMissed() {
    /* `mTimer = mTimer - 1` comes out 4 bytes short; the decrement matches. */
    mTimer--;
    if (mTimer == 0) {
        Puff_Spawn(mScreenPos[0], 0xc0);
        data_ov006_02140598 = data_ov006_02140598 - 1;
        ApproachLinear(sTrmp2Lives, 0, 1);
        Sound::PlayBank2_2D(0x130);
        Clear();
        mWallBumpTimer = 0;
        EnterRespawnWait();
    } else {
        int v = mVel.x;
        mVel.x = FX_MUL(v, 0xc00);
        int x = mPos.x;
        if (x < -0x6c000)
            mPos.x = -0x6c000;
        else if (x > 0x6c000)
            mPos.x = 0x6c000;
    }
}

void dMgTrmpln2Mario_c::EnterMissed() {
    if (sTrmp2Lives > 1)
        D3D_PlaySeAtX(0x1ca, mPos.x);
    else
        Sound::PlayBank2_2D(0x1c9);
    mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    mTimer = 0x28;
    *(double *)mState = pmf_Trmp2Mario_StateMissed_2;
}

void dMgTrmpln2Mario_c::StateFall() {
    u16 state = mContact;
    int v24;

    if (state == 1) {
        BounceOffLineB();
        return;
    }
    if (state == 2) {
        BounceOffLineA();
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v24 = mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    mPos.x = v;
}

    mVel.x *= -1;
    if (mVel.y <= -0x800)
        goto L260;

    if (mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        mVel.y = 0x800;
        mAnim = 2;
        mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

        mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, mAnim, mPos.x);

        {
            long long ll = *(long long *)&pmf_Trmp2Mario_StateWallBounce;
            mState[0] = (int)ll;
            mState[1] = (int)((unsigned long long)ll >> 32);
        }
    }
    return;

L1a4: {
    int t = FX_MUL(-mVel.x, 0xf00);
    int pos;

    mVel.x = t;

    pos = mPos.x;
    if (pos < -0x68000)
        pos = -0x68000;
    else if (pos > 0x68000)
        pos = 0x68000;
    mPos.x = pos;
}
    goto L260;

L230:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L260: {
    int v = mVel.x;
    if (v > 0x2000) {
        ApproachLinear(mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)v, 0xc00);
    }
}

    if (mPos.y >= -0xc8000)
        return;

    EnterMissed();
}

void dMgTrmpln2Mario_c::EnterFall() {
    *(W2 *)mState = pmf_Trmp2Mario_StateFall;
    StateFall();
}

void dMgTrmpln2Mario_c::StateAppearWait() {
    if (ApproachLinear2(mTimer, 0, 1) == 0) {
        mPos.y = 0;
        return;
    }
    mPos.y = 0;
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    mVisible = 1;
    EnterBounce();
}

void dMgTrmpln2Mario_c::EnterAppearWait(short v) {
    mTimer = v;
    mGravityStep = 0;
    *(W2 *)mState = pmf_Trmp2Mario_StateAppearWait;
}

void dMgTrmpln2Mario_c::StateHidden() {
    mPos.y = 0;
}

void dMgTrmpln2Mario_c::EnterHidden() {
    mVisible = 0;
    mPos.y = 0;
    mPos.x = 0;
    mPos.z = 0;
    mVel.x = data_020a0ebc.x;
    mVel.y = data_020a0ebc.y;
    mVel.z = data_020a0ebc.z;
    mGravityStep = 0;
    mWallBumpTimer = 0;
    mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(double *)mState = pmf_Trmp2Mario_StateHidden_3;
}

int dMgTrmpln2Mario_c::Load() {
    BMD_File *f;
    int ret;

    f = dExtSimpleModel_c::LoadFile(sTrmpMarioModelFile);
    sTrmpAnimJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioJumpAnimFile);
    sTrmpAnimDoubleJump1 = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioDoubleJump1AnimFile);
    sTrmpAnimWallJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioWallJumpAnimFile);
    sTrmpAnimHangUp = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioHangUpAnimFile);
    sTrmpAnimRollJumpEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioRollJumpEndAnimFile);
    sTrmpAnimWalk = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioWalkAnimFile);
    sTrmpAnimDoubleJump2 = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioDoubleJump2AnimFile);
    sTrmpAnimJumpEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioJumpEndAnimFile);
    sTrmpAnimTurn = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioTurnAnimFile);
    sTrmpAnimTurnEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioTurnEndAnimFile);
    sTrmpAnimBounce = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioRollJumpAnimFile);
    sTrmpAnimStruggle = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioStruggleAnimFile);
    sTrmpAnimSdown = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioSdownAnimFile);
    ret = mAnmModel.SetFile(f, 1, -1);
    mAnim = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);
    mGravityStep = 0;
    func_02016a14(&mAnmModel, 0x73ff);
    func_02016a04(&mAnmModel, 0x3e52);
    EnterHidden();
    return ret;
}

/* mAnmModel, then the ring and base adapters (each storing its own vptr
   back), run after the body. */
dMgTrmpln2Mario_c::~dMgTrmpln2Mario_c() {
    sTrmpMarioModelFile.Release();
    sTrmpMarioJumpAnimFile.Release();
    sTrmpMarioDoubleJump1AnimFile.Release();
    sTrmpMarioWallJumpAnimFile.Release();
    sTrmpMarioHangUpAnimFile.Release();
    sTrmpMarioRollJumpEndAnimFile.Release();
    sTrmpMarioWalkAnimFile.Release();
    sTrmpMarioDoubleJump2AnimFile.Release();
    sTrmpMarioJumpEndAnimFile.Release();
    sTrmpMarioTurnAnimFile.Release();
    sTrmpMarioTurnEndAnimFile.Release();
    sTrmpMarioRollJumpAnimFile.Release();
    sTrmpMarioStruggleAnimFile.Release();
    sTrmpMarioSdownAnimFile.Release();
    sTrmpAnimJump = 0;
    sTrmpAnimDoubleJump1 = 0;
    sTrmpAnimWallJump = 0;
    sTrmpAnimHangUp = 0;
    sTrmpAnimRollJumpEnd = 0;
    sTrmpAnimWalk = 0;
    sTrmpAnimDoubleJump2 = 0;
    sTrmpAnimJumpEnd = 0;
    sTrmpAnimTurn = 0;
    sTrmpAnimTurnEnd = 0;
    sTrmpAnimBounce = 0;
    sTrmpAnimStruggle = 0;
    sTrmpAnimSdown = 0;
}

/* The ring adapter base and mAnmModel are constructed before the (empty)
   body. The ROM symbol for this constructor was misfiled as
   _ZN7daPly_c29TryExitCharacterDoorWithIntroEv; it stores the
   _ZTV17dMgTrmpln2Mario_c vptr and builds a dExtAnmModel_c at +0x78, so it
   is this class's C1, not a daPly_c method. */
dMgTrmpln2Mario_c::dMgTrmpln2Mario_c() {}

int Trmp2Mario_Hide(void) {
    int i;
    for (i = 0; i < sTrmp2MarioCount; i++) {
        int *v = ((dMgTrmpln2Mario_c *)sTrmp2Marios)[i].mState;
        volatile int *q = (volatile int *)pmf_Trmp2Mario_StateHidden_2;
        if (v[0] != pmf_Trmp2Mario_StateHidden_2[0] ||
            (v[1] != q[1] && ((dMgTrmpln2Mario_c *)sTrmp2Marios)[i].mState[0] != 0))
            return 0;
    }
    return 1;
}

void Trmp2Mario_EnterAll(void) {
    int i, n, off;
    n = sTrmp2MarioCount;
    sTrmp2GravityStep = 0x100;
    sTrmp2FallSpeed = -0x4800;
    sTrmpItemHitsOff = 1;

    i = 0;
    if (n > 0) {
        off = 0;
        do {
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->EnterByKind();
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->UpdateMatrix();
            n = sTrmp2MarioCount;
            i++;
            off += 0xdc;
        } while (i < n);
    }
}

void Trmp2Mario_MoveToStart(void) {
    char *base;
    char *e;
    int i;
    int q0;
    int count;

    ApproachLinear(sTrmp2MarioCount, sTrmp2MarioTarget, 1);
    i = 0;
    count = sTrmp2MarioCount;
    if (count <= 0)
        return;

    base = sTrmp2Marios;
    e = base;
    q0 = pmf_Trmp2Mario_StateHidden[0];
    for (;;) {

        int *v = MARIO2(e)->mState;
        volatile int *q = (volatile int *)pmf_Trmp2Mario_StateHidden;
        if (v[0] == q0 && (v[1] == q[1] || MARIO2(e)->mState[0] == 0)) {

            ((dMgTrmpln2Mario_c *)(base + i * 0xdc))->EnterAppearWait((short)(i * 0x14));
            MARIO2(sTrmp2Marios + i * 0xdc)->mPos.x = 0;
            MARIO2(sTrmp2Marios + i * 0xdc)->mVel.x = FX_MUL(sTrmp2DriftScale, (i << 11) - 0x1000);
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + i * 0xdc))->UpdateMatrix();
            return;
        }

        i++;
        e += 0xdc;
        if (i < count)
            continue;
        break;
    }
}

void Trmp2Mario_PlaceAtStart(void) {
    int n;
    int i;
    n = sTrmp2MarioCount;
    if (n > 3)
        n = 3;
    for (i = 0; i < n; i++) {
        ((dMgTrmpln2Mario_c *)(sTrmp2Marios + i * 0xdc))->EnterAppearWait((short)(i * 0x14));
        MARIO2(sTrmp2Marios + i * 0xdc)->mPos.x = i * 0x10000 - 0x20000;
        MARIO2(sTrmp2Marios + i * 0xdc)->mVel.x = FX_MUL(sTrmp2DriftScale, i * 0x800 - 0x1000);
        ((dMgTrmpln2Mario_c *)(sTrmp2Marios + i * 0xdc))->UpdateMatrix();
    }
}

void Trmp2Mario_DrawAll(void) {
    VecFx32 v;
    VecFx32 w;
    int i, n, off;
    v.x = 0;
    v.y = 0;
    v.z = -0xff8;
    func_0203cd80((int *)&v, -0x2000);
    func_0203ccd4((Vector3 *)&v, -0x3000);
    G3_LightVector(GX_LIGHTID_0, v.x, v.y, v.z);
    w.x = 0;
    w.y = 0;
    w.z = -0xff8;
    func_0203cd80((int *)&w, -0x2000);
    G3_LightVector(GX_LIGHTID_1, w.x, w.y, w.z);
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));

    n = sTrmp2MarioCount;
    i = 0;
    if (n > 0) {
        off = 0;
        do {
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->Render();
            n = sTrmp2MarioCount;
            i++;
            off += 0xdc;
        } while (i < n);
    }
    Puff_DrawAll();
}

/* Module-level update step; not a class method. */
void Trmp2Mario_UpdateAll(void) {
    int i = 0;
    if (sTrmp2MarioCount > 0) {
        int off = 0;
        do {
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->Update();
            TrmpItem::UpdateWithJumper((TrampolineObjectCallbacks *)(sTrmp2Marios + off), i);
            i++;
            off += 0xdc;
        } while (i < sTrmp2MarioCount);
    }
    Puff_UpdateAll();
}

void Trmp2Mario_ResetAll(u32) {
    int i;
    int off;
    Trmp2Mario_SetTuning(0x1000);
    sTrmp2Lives = 3;
    data_ov006_02140598 = 0;
    sTrmpItemHitsOff = 0;
    if (sTrmp2MarioTarget > 0) {
        i = 0;
        off = 0;
        do {
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->EnterHidden();
            ((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->UpdateMatrix();
            i++;
            off += 0xdc;
        } while (i < sTrmp2MarioTarget);
    }
    Puff_ClearAll(0, 0);
}

void Trmp2Mario_SetTuning(int a) {
    int v;
    if (a > 0x2800)
        a = 0x2800;
    v = FX_Sqrt(a);
    sTrmp2FallSpeed = FX_MUL(v, -0x1c00);
    sTrmp2GravityStep = FX_MUL(a, 0xc0);
    data_ov006_0214053c = FX_MUL(v, 0xe00);
    sTrmpSpeedScale = FX_MUL(v, 0x5800);
    sTrmp2RiseSpeedMax = FX_MUL(v, 0x6000);
    sTrmp2DriftScale = FX_MUL(v, 0x180);
}

int Trmp2Mario_InitPool(void *base, int n) {
    int i;
    int off;

    sTrmp2MarioCount = n;
    sTrmp2MarioTarget = n;
    sTrmp2Marios = (char *)base;

    i = 0;
    if (i < n) {
        off = i;
        do {
            char *p;
            if (((dMgTrmpln2Mario_c *)(sTrmp2Marios + off))->Load() == 0)
                return 0;
            p = sTrmp2Marios + off;
            if (i < 5)
                sTrmpJumpers[i] = p;
            if (i < 5)
                sTrmp2MarioPtrs[i] = p;
            off = off + 0xdc;
            n = sTrmp2MarioCount;
            i++;
        } while (i < n);
    }
    return 1;
}

Vector3 *dMgTrmpln3DMario_c::GetVelocity() {
    return &mVel;
}

/* Anything but missed: the drawn line only bounces a Mario in play. */
int dMgTrmpln3DMario_c::IsActive() {
    int *p = mState;
    int *g = pmf_Trmp3DMario_StateMissed;
    int lr = 1;
    if (p[0] == g[0]) {
        if (p[1] == g[1] || mState[0] == 0)
            lr = 0;
    }
    return lr;
}

Vector3 *dMgTrmpln3DMario_c::GetPrevPos() {
    return &mPrevPos;
}

Vector3 *dMgTrmpln3DMario_c::GetPos() {
    return &mPos;
}

void dMgTrmpln3DMario_c::Render() {
    dExtAnmModel_c *model = &mAnmModel;
    model->Render(&sTrmp3DMarioScale);
}

void dMgTrmpln3DMario_c::UpdateMatrix() {
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mRotY);
    mAnmModel.mat4x3 = data_020a0e68;
}

/* The ROM relocates this call to 0x020cb134 (the 3D ApplyGravity); a byte
       compare cannot see the difference because a bl is a wildcarded reloc. */
void dMgTrmpln3DMario_c::Update() {
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    if (mState[0] != 0) {
        /* Inline member-pointer call; the typed form does not reproduce
           it (see the file header). */
        Closure *cl = (Closure *)mState;
        void *tobj = (char *)this + (cl->adj >> 1);
        void (*fn)();
        if (cl->adj & 1)
            fn = *(void (**)())(*(char **)tobj + cl->off);
        else
            fn = (void (*)())cl->off;
        ((void (*)(void *))fn)(tobj);
    }
    ApplyGravity();
    Fix12i v = FX_MUL(mBounceVy, 0xb4b);
    if (mVel.y > v) {
        if (mAnmModel.file == sTrmpAnimBounce) {
            mParticleId = Particle::System::New(mParticleId, kPtclMgTrampolineTrail, mPos.x << 3,
                                                mPos.y << 3, mPos.z << 3, 0, 0);
        }
    }
    UpdateMatrix();
    mAnmModel.Advance();
}

void dMgTrmpln3DMario_c::ApplyGravity() {
    ApproachLinear(mVel.y, sTrmp3DFallSpeed, mGravityStep);
    VEC_Add(&mPos, &mVel, &mPos);
}

void dMgTrmpln3DMario_c::StateKnockedOut() {
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    EnterOffstage();
}

void dMgTrmpln3DMario_c::EnterByKind() {
    int *p;
    int *d;

    p = mState;
    d = pmf_Trmp3DMario_StateOffstage_4;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (mState[0] != 0)
                goto check2;
        }
        return;
    }
check2:
    p = mState;
    d = pmf_Trmp3DMario_StateGoal_2;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (mState[0] != 0)
                goto body;
        }
        return;
    }
body:
    mGravityStep = 0x100;
    mVel.x = 0;
    mVel.y = 0x2000;
    Sound::PlayBank2_2D(0x1c9);
    mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    mAnmModel.currFrame = 0;
    *(W2 *)mState = *(W2 *)&pmf_Trmp3DMario_StateKnockedOut;
}

void dMgTrmpln3DMario_c::StateGoal() {
    int va, vb;
    short idx;
    int lim;
    int u;
    int i;

    idx = mHeightRow;
    lim = sTrmpRowHeights[idx];
    if (mPos.y <= lim) {
        mPos.y = lim;
        mVel.y = 0;
    }

    if (mAnmModel.file == sTrmpAnimRollJumpEnd) {
        UpdateAngle(mRotY, 0, 2, 0x1000);
        if (mAnmModel.Finished() == 0)
            return;
        mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
        return;
    }

    if (mPos.x > 0) {
        ApproachLinear(mVel.x, 0x4000, 0x800);
        ApproachLinear2(mRotY, 0x4000, 0x1000);
    } else {
        ApproachLinear(mVel.x, -0x4000, 0x800);
        ApproachLinear2(mRotY, -0x4000, 0x1000);
    }

    u = mPos.x;
    if (u <= 0x90000 && u >= -0x90000)
        return;

    func_02012718(0x1be, 0x100000);
    ApproachLinear(sTrmpComboCount, 0x270f, 1);
    sTrmpMariosInPlay -= 1;
    sTrmpMariosOnLedges -= 1;

    if (mPos.y > 0) {
        Camera_ProjectToScreen(sMgSubCamera, &mPos, &va, &vb);
        vb = vb - (Mg_GetTopScreenYOffset() + 0xc0);
    } else {
        Camera_ProjectToScreen((MgCamera *)sMgMainCamera, &mPos, &va, &vb);
    }
    vb -= 0x20;
    TrmpFx_Spawn(vb << 12, (short)sTrmpComboCount);

    sTrmpGoalSlotCursor += 1;
    if (sTrmpGoalSlotCursor >= 6)
        sTrmpGoalSlotCursor = 0;

    i = sTrmpGoalSlotCursor;
    {
        TrmpGoal *p = &sTrmpGoalSlots + i;
        mPos.x = p->x;
        mPos.y = p->y;
        mPos.z = p->z;
    }
    mVel.x = data_020a0ebc.x;
    mVel.y = data_020a0ebc.y;
    mVel.z = data_020a0ebc.z;

    EnterOffstage();
}

void dMgTrmpln3DMario_c::EnterGoal() {
    mPos.y = sTrmpRowHeights[mHeightRow];
    mVel.x = 0;
    mVel.y = 0;
    Sound_PlayBank1Panned(0, 0x10, mPos.x);
    mAnmModel.SetAnim(sTrmpAnimRollJumpEnd, 0x40000000, 0x800, 0);
    sTrmpMariosOnLedges = sTrmpMariosOnLedges + 1;
    *(W2 *)mState = pmf_Trmp3DMario_StateGoal;
    StateGoal();
}

void dMgTrmpln3DMario_c::StateLand() {
    ApproachLinear(mPos.y, sTrmpRowHeights[mHeightRow], 0x4000);
    if (mPos.x > 0)
        ApproachLinear(mRotY, 0x4000, 0x1000);
    else
        ApproachLinear(mRotY, -0x4000, 0x1000);
    mVel.x = 0;
    mVel.y = 0;
    if (mAnmModel.file != sTrmpAnimHangUp)
        return;
    if (mAnmModel.Finished() == 0)
        return;
    if (mHeightRow == sTrmpGoalRow)
        EnterGoal();
    else
        EnterOnLedge();
}

void dMgTrmpln3DMario_c::EnterLand() {
    mVel.x = data_020a0ebc.x;
    mVel.y = data_020a0ebc.y;
    mVel.z = data_020a0ebc.z;
    mAnmModel.SetAnim(sTrmpAnimHangUp, 0x40000000, 0x800, 0);
    Sound_PlayBank1Panned(0, 0x17, mPos.x);
    *(W2 *)mState = pmf_Trmp3DMario_StateLand;
    int *p20 = &mPos.y;
    *p20 = *p20 + 0x20000;
    StateLand();
}

void dMgTrmpln3DMario_c::StateFallToLedge() {
    int v = mPos.x;
    if (v > 0) {
        short idx = mHeightRow;
        int cur = mPos.y;
        int *d = sTrmpRowHeights;
        if (cur <= d[idx]) {
            if (idx == sTrmpGoalRow) {
                EnterGoal();
            } else {
                EnterOnLedge();
            }
            D3D_PlaySeAtX(0x1b4, mPos.x);
        } else {
            int lim = d[1] + 0x30000;
            if ((cur >= lim) && (idx == 1)) {
                mPos.y = lim;
            }
        }
    } else {
        if ((mPos.y) <= sTrmpRowHeights[0]) {
            D3D_PlaySeAtX(0x1b4, v);
            EnterOnGround();
            return;
        }
    }
    {
        int w = mPos.x;
        if (w > 0x70000) {
            mPos.x = 0x70000;
            mVel.x = 0;
            return;
        }
        if (w < (-0x70000)) {
            mPos.x = -0x70000;
            mVel.x = 0;
        }
    }
}

void dMgTrmpln3DMario_c::EnterFallToLedge() {
    *(W2 *)mState = pmf_Trmp3DMario_StateFallToLedge;
    StateFallToLedge();
}

void dMgTrmpln3DMario_c::StateHardBounce() {
    int v;

    if (mVel.y <= 0) {
        EnterFall();
        return;
    }

    if (mAnmModel.file == sTrmpAnimBounce) {
        if (mAnmModel.WillHitFrame(0xc) || mAnmModel.WillHitFrame(0x18)) {
            D3D_PlaySeAtX(0x1b5, mPos.x);
        }
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v = mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L1ac;
    if (mVel.x <= 0)
        goto L1ac;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = mPos.y;

    if (p > f0) {
        mHeightRow = 0;
        EnterFallToLedge();
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L134;
    mHeightRow = 0;
    EnterLand();
    return;

Ld0:
    if (v <= 0)
        goto L134;
    {
        int f1 = sTrmpRowHeights[1];
        int hi1 = f1 + 0x30000;

        if (p >= hi1)
            goto L134;
        if (p <= f1)
            goto L10c;
        mHeightRow = 1;
        EnterFallToLedge();
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L134;
        mHeightRow = 1;
        EnterLand();
        return;
    }
}

L134:
    if (mVel.y > 0) {
        mAnim = 2;
        mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

        mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, mAnim, mPos.x);
    }

    func_02012718(0x1b3, mPos.x + 0x80000);
    mVel.x *= -1;
    goto L1dc;

L1ac:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L1dc: {
    int r = mVel.x;
    if (r > 0x4000) {
        ApproachLinear(mRotY, 0x6000, 0xc00);
    } else if (r < -0x4000) {
        ApproachLinear(mRotY, -0x6000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)r, 0xc00);
    }
}
}

void dMgTrmpln3DMario_c::BounceOffLineA() {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp4;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;

    alongLine = mContactOffset;
    lineHalfLength = mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)&mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = VEC_DotProduct(&mVel, &tmp);
    scale = FX_MUL(dot, 0x1400);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    VEC_Subtract(&mVel, &tmp2, &mVel);

    fdivr = FX_Div(lineHalfLength, lineHalfLength + alongLine);

    mGravityStep = sTrmp3DGravityStep;

    {
        Vector3_16 v;
        v.x = (s16)tmp.x;
        v.y = (s16)tmp.y;
        v.z = (s16)tmp.z;

        Particle::System::New(0, kPtclMgTrampolineBounceA, mPos.x << 3, mPos.y << 3, mPos.z << 3,
                              (const Vector3_16f *)(&v), 0);
        Particle::System::New(0, kPtclMgTrampolineBounceB, mPos.x << 3, mPos.y << 3, mPos.z << 3,
                              (const Vector3_16f *)(&v), 0);
    }

    mParticleId = Particle::System::New(mParticleId, kPtclMgTrampolineTrail, mPos.x << 3,
                                        mPos.y << 3, mPos.z << 3, 0, 0);

    scale = FX_MUL(sTrmp3DHardBounceScale, fdivr);
    Vec3_MulScalar(&tmp4, &tmp, scale);
    VEC_Add(&mVel, &tmp4, &mVel);

    {
        int v = mVel.x;
        if (v < -0x1000)
            v = -0x1000;
        else if (v > 0x1000)
            v = 0x1000;
        mVel.x = v;
    }

    {
        int half = sTrmp3DHardBounceScale >> 1;
        int cur = mVel.y;
        if (cur >= half)
            half = cur;
        mVel.y = half;
    }

    mBounceVy = mVel.y;
    Clear();

    if (mAnim != 0) {
        mAnim = 0;
    } else {
        mAnim = 1;
    }

    mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);

    mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, 4, mPos.x);

    D3D_PlaySeAtX(0x1b5, mPos.x);

    pad_5c = 0;
    *(W2 *)mState = *(W2 *)&pmf_Trmp3DMario_StateHardBounce;
    StateHardBounce();
}

void dMgTrmpln3DMario_c::StateBounce() {
    int v;

    if (mVel.y <= 0) {
        EnterFall();
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v = mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L1ac;
    if (mVel.x <= 0)
        goto L1ac;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = mPos.y;

    if (p > f0) {
        mHeightRow = 0;
        EnterFallToLedge();
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L134;
    mHeightRow = 0;
    EnterLand();
    return;

Ld0:
    if (v <= 0)
        goto L134;
    {
        int f1 = sTrmpRowHeights[1];
        int hi1 = f1 + 0x30000;

        if (p >= hi1)
            goto L134;
        if (p <= f1)
            goto L10c;
        mHeightRow = 1;
        EnterFallToLedge();
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L134;
        mHeightRow = 1;
        EnterLand();
        return;
    }
}

L134:
    if (mVel.y > 0) {
        mAnim = 2;
        mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

        mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, mAnim, mPos.x);
    }

    func_02012718(0x1b3, mPos.x + 0x80000);
    mVel.x *= -1;
    goto L1dc;

L1ac:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L1dc: {
    int r = mVel.x;
    if (r > 0x4000) {
        ApproachLinear(mRotY, 0x4000, 0xc00);
    } else if (r < -0x4000) {
        ApproachLinear(mRotY, -0x4000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)r, 0xc00);
    }
}
}

void dMgTrmpln3DMario_c::BounceOffLineB() {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp3;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;
    int vx;

    alongLine = mContactOffset;
    lineHalfLength = mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;

    {
        int *src = (int *)&mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = VEC_DotProduct(&mVel, &tmp);
    scale = FX_MUL(dot, 0x1200);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    VEC_Subtract(&mVel, &tmp2, &mVel);

    fdivr = FX_Div(lineHalfLength, lineHalfLength + alongLine);
    mGravityStep = sTrmp3DGravityStep;

    scale = FX_MUL(sTrmp3DBounceScale, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    VEC_Add(&mVel, &tmp3, &mVel);

    vx = mVel.x;
    if (vx < -0x1000)
        vx = -0x1000;
    else if (vx > 0x1000)
        vx = 0x1000;
    mVel.x = vx;

    {
        int half = sTrmp3DBounceScale >> 2;
        int cur = mVel.y;
        if (cur >= half)
            half = cur;
        mVel.y = half;
    }

    Clear();

    if (mAnim != 0) {
        mAnim = 0;
    } else {
        mAnim = 1;
    }

    mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

    mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, mAnim, mPos.x);

    *(W2 *)mState = *(W2 *)&pmf_Trmp3DMario_StateBounce;

    StateBounce();
}

void dMgTrmpln3DMario_c::JumpOff() {
    int *p38;
    int b;

    mGravityStep = sTrmp3DGravityStep;
    mVel.x = sTrmp3DJumpOffSpeed +
             (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 19);

    b = mPos.x;
    if (b > 0)
        mVel.x = -mVel.x;

    p38 = &mVel.y;
    *p38 += (int)((((long long)sTrmp3DBounceScale << 11) + 0x800) >> 12);

    Clear();

    mAnim = 0;

    mAnmModel.SetAnim(*sTrmpBounceAnimTable[mAnim], 0x40000000, 0x800, 0);

    mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, mAnim, mPos.x);

    *(W2 *)mState = *(W2 *)&pmf_Trmp3DMario_StateBounce_2;

    StateBounce();
}

void dMgTrmpln3DMario_c::StateMissed() {
    s16 *p = &mTimer;
    *p = *p - 1;
    if (mTimer == 0) {
        Puff_Spawn(mScreenPos[0], 0xc0);
        sTrmpMariosInPlay -= 1;
        ApproachLinear(sTrmp3DLives, 0, 1);
        Sound::PlayBank2_2D(0x130);
        EnterOffstage();
        return;
    }
    mVel.x = FX_MUL(mVel.x, 0xc00);
    {
        int v = mPos.x;
        if (v < -0x6c000) {
            mPos.x = -0x6c000;
            return;
        }
        if (v > 0x6c000)
            mPos.x = 0x6c000;
    }
}

void dMgTrmpln3DMario_c::EnterMissed() {
    if (sTrmp3DLives > 1)
        D3D_PlaySeAtX(0x1ca, mPos.x);
    else
        Sound::PlayBank2_2D(0x1c9);
    mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    mTimer = 0x28;
    *(double *)mState = pmf_Trmp3DMario_StateMissed_2;
}

void dMgTrmpln3DMario_c::StateFall() {
    int v;
    u16 state = mContact;

    if (state == 1) {
        BounceOffLineB();
        return;
    }
    if (state == 2) {
        BounceOffLineA();
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);

    v = mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L150;
    if (mVel.x <= 0)
        goto L150;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = mPos.y;

    if (p > f0) {
        mHeightRow = 0;
        EnterFallToLedge();
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L130;
    mHeightRow = 0;
    EnterLand();
    return;

Ld0:
    if (v <= 0)
        goto L130;
    {
        int f1 = sTrmpRowHeights[1];
        int hi1 = f1 + 0x30000;

        if (p >= hi1)
            goto L130;
        if (p <= f1)
            goto L10c;
        mHeightRow = 1;
        EnterFallToLedge();
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L130;
        mHeightRow = 1;
        EnterLand();
        return;
    }
}

L130:
    D3D_PlaySeAtX(0x1b3, v);
    mVel.x *= -1;
    goto L180;

L150:
    if (mContact == 3) {
        Clear();
        mVel.x *= -1;
    }

L180: {
    int r = mVel.x;
    if (r > 0x4000) {
        ApproachLinear(mRotY, 0x4000, 0xc00);
    } else if (r < -0x8000) {
        ApproachLinear(mRotY, -0x4000, 0xc00);
    } else {
        ApproachLinear(mRotY, (short)r, 0xc00);
    }
}

    if (mPos.y >= -0xc8000)
        return;

    EnterMissed();
}

void dMgTrmpln3DMario_c::EnterFall() {
    *(W2 *)mState = pmf_Trmp3DMario_StateFall;
    StateFall();
}

void dMgTrmpln3DMario_c::StateWalkBack() {
    short idx = mHeightRow;
    mPos.y = sTrmpRowHeights[idx];
    if (mPos.x >= 0x5bff8)
        return;
    sTrmpMariosOnLedges--;
    JumpOff();
}

void dMgTrmpln3DMario_c::EnterWalkBack() {
    mGravityStep = 0;
    mPos.y = sTrmpRowHeights[mHeightRow];
    mPos.z = 0;
    mVel.x = -0x1800;
    mVel.y = 0;
    mVel.z = 0;
    mRotY = -0x4000;
    mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(double *)mState = pmf_Trmp3DMario_StateWalkBack;
}

void dMgTrmpln3DMario_c::StateOnLedge() {
    mPos.y = sTrmpRowHeights[mHeightRow];

    if (mAnmModel.file == sTrmpAnimJumpEnd) {
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
            return;
        }
    }

    if (mAnmModel.file == sTrmpAnimWalk) {
        if (mVel.x > 0) {
            if (mPos.x > 0x70000) {
                mPos.x = 0x70000;
                mVel.x = 0;
                mAnmModel.SetAnim(sTrmpAnimTurn, 0x40000000, 0x800, 0);
            }
            return;
        }
        if (ApproachLinear(mRotY, 0x4000, 0x800) != 0)
            mVel.x = 0x1000;
        return;
    }

    if (mAnmModel.file == sTrmpAnimTurn) {
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(sTrmpAnimTurnEnd, 0x40000000, 0x800, 0);
            return;
        }
    }

    if (mAnmModel.file == sTrmpAnimTurnEnd) {
        if (mAnmModel.Finished() != 0)
            EnterWalkBack();
    }
}

void dMgTrmpln3DMario_c::EnterOnLedge() {
    int *cur;
    int *want;
    int z = 0;
    mGravityStep = z;
    mPos.y = sTrmpRowHeights[mHeightRow];
    mPos.z = z;
    mVel.x = z;
    mVel.y = z;
    mVel.z = z;
    cur = mState;
    want = pmf_Trmp3DMario_StateFallToLedge_2;
    if (cur[0] == want[0] && (cur[1] == want[1] || mState[0] == 0)) {
        mAnmModel.SetAnim(sTrmpAnimJumpEnd, 0x40000000, 0x800, 0);
    } else {
        mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    }
    {
        int *ctr = &sTrmpMariosOnLedges;
        int c = *ctr;
        *ctr = c + 1;
        *(W2 *)mState = pmf_Trmp3DMario_StateOnLedge;
    }
}

void dMgTrmpln3DMario_c::StateWalkIn() {
    mPos.y = sTrmpRowHeights[0];
    if (mPos.x <= -0x5c008)
        return;
    JumpOff();
}

void Trmp3DMario_EnterWalkIn(char *c) {
    MARIO3D(c)->mGravityStep = 0;
    MARIO3D(c)->mPos.y = sTrmpRowHeights[0];
    MARIO3D(c)->mPos.z = 0;
    MARIO3D(c)->mPrevPos.x = MARIO3D(c)->mPos.x;
    MARIO3D(c)->mPrevPos.y = MARIO3D(c)->mPos.y;
    MARIO3D(c)->mPrevPos.z = MARIO3D(c)->mPos.z;
    MARIO3D(c)->mVel.x = 0x1800;
    MARIO3D(c)->mVel.y = 0;
    MARIO3D(c)->mVel.z = 0;
    MARIO3D(c)->mRotY = 0x4000;
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    int *s = MARIO3D(c)->mState;
    W2 *d = &pmf_Trmp3DMario_StateOnGround;
    if (s[0] != d->w[0] || (s[1] != d->w[1] && MARIO3D(c)->mState[0] != 0)) {
        sTrmpMariosInPlay += 1;
    }
    *(W2 *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateWalkIn;
}

void dMgTrmpln3DMario_c::StateOnGround() {
    mPos.y = sTrmpRowHeights[0];

    if (mAnmModel.file == sTrmpAnimJumpEnd) {
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
            return;
        }
    }

    if (mAnmModel.file == sTrmpAnimWalk) {
        if (mVel.x < 0) {
            if (mPos.x < -0x70000) {
                mPos.x = -0x70000;
                mVel.x = 0;
                mAnmModel.SetAnim(sTrmpAnimTurn, 0x40000000, 0x800, 0);
            }
            return;
        }
        if (ApproachLinear(mRotY, -0x4000, 0x800) != 0)
            mVel.x = -0x1000;
        return;
    }

    if (mAnmModel.file == sTrmpAnimTurn) {
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(sTrmpAnimTurnEnd, 0x40000000, 0x800, 0);
            return;
        }
    }

    if (mAnmModel.file == sTrmpAnimTurnEnd) {
        if (mAnmModel.Finished() != 0)
            Trmp3DMario_EnterWalkIn((char *)this);
    }
}

void dMgTrmpln3DMario_c::EnterOnGround() {
    mGravityStep = 0;
    mPos.y = sTrmpRowHeights[0];
    mPos.z = 0;
    mVel.x = 0;
    mVel.y = 0;
    mVel.z = 0;
    mAnmModel.SetAnim(sTrmpAnimJumpEnd, 0x40000000, 0x800, 0);
    *(double *)mState = pmf_Trmp3DMario_StateOnGround_2;
}

int Trmp3DMario_IsOffstage(int *r0) {
    int *p = MARIO3D(r0)->mState;
    int *g = pmf_Trmp3DMario_StateOffstage_3;
    int ret = 1;
    if (p[0] == g[0]) {
        if (p[1] == g[1])
            goto zero;
        if (MARIO3D(r0)->mState[0] != 0)
            goto out;
    zero:
        ret = 0;
    }
out:
    return ret == 0;
}

void dMgTrmpln3DMario_c::StateOffstage() {
    mPos.y = sTrmpRowHeights[0];
}

void dMgTrmpln3DMario_c::EnterOffstage() {
    mPos.y = sTrmpRowHeights[0];
    mPos.x = -0x90000;
    mPos.z = 0;
    mVel.x = data_020a0ebc.x;
    mVel.y = data_020a0ebc.y;
    mVel.z = data_020a0ebc.z;
    mGravityStep = 0;
    mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(W2 *)mState = pmf_Trmp3DMario_StateOffstage_2;
}

int dMgTrmpln3DMario_c::Load() {
    BMD_File *f;
    int ret;

    f = dExtSimpleModel_c::LoadFile(sTrmpMarioModelFile);
    sTrmpAnimJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioJumpAnimFile);
    sTrmpAnimDoubleJump1 = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioDoubleJump1AnimFile);
    sTrmpAnimWallJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioWallJumpAnimFile);
    sTrmpAnimHangUp = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioHangUpAnimFile);
    sTrmpAnimRollJumpEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioRollJumpEndAnimFile);
    sTrmpAnimWalk = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioWalkAnimFile);
    sTrmpAnimDoubleJump2 = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioDoubleJump2AnimFile);
    sTrmpAnimJumpEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioJumpEndAnimFile);
    sTrmpAnimTurn = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioTurnAnimFile);
    sTrmpAnimTurnEnd = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioTurnEndAnimFile);
    sTrmpAnimBounce = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioRollJumpAnimFile);
    sTrmpAnimStruggle = (BCA_File *)dExtFrameCtrl_c::LoadFile(sTrmpMarioStruggleAnimFile);
    ret = mAnmModel.SetFile(f, 1, -1);
    mAnim = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &mPos, mScreenPos);
    mGravityStep = 0;
    func_02016a14(&mAnmModel, 0x6bff);
    func_02016a04(&mAnmModel, 0x21d);
    EnterOffstage();
    return ret;
}

/* mAnmModel, then the base adapter (storing its vptr back), run after the
   body. */
dMgTrmpln3DMario_c::~dMgTrmpln3DMario_c() {
    sTrmpMarioModelFile.Release();
    sTrmpMarioJumpAnimFile.Release();
    sTrmpMarioDoubleJump1AnimFile.Release();
    sTrmpMarioWallJumpAnimFile.Release();
    sTrmpMarioHangUpAnimFile.Release();
    sTrmpMarioRollJumpEndAnimFile.Release();
    sTrmpMarioWalkAnimFile.Release();
    sTrmpMarioDoubleJump2AnimFile.Release();
    sTrmpMarioJumpEndAnimFile.Release();
    sTrmpMarioTurnAnimFile.Release();
    sTrmpMarioTurnEndAnimFile.Release();
    sTrmpMarioRollJumpAnimFile.Release();
    sTrmpMarioStruggleAnimFile.Release();
    sTrmpAnimJump = 0;
    sTrmpAnimDoubleJump1 = 0;
    sTrmpAnimWallJump = 0;
    sTrmpAnimHangUp = 0;
    sTrmpAnimRollJumpEnd = 0;
    sTrmpAnimWalk = 0;
    sTrmpAnimDoubleJump2 = 0;
    sTrmpAnimJumpEnd = 0;
    sTrmpAnimTurn = 0;
    sTrmpAnimTurnEnd = 0;
    sTrmpAnimBounce = 0;
    sTrmpAnimStruggle = 0;
}

/* The base adapter and mAnmModel are constructed before the (empty) body. */
dMgTrmpln3DMario_c::dMgTrmpln3DMario_c() {}

int Trmp3DMario_CountFinished(void) {
    int i = 0;
    char *e;
    int first;
    int n;
    int *b;
    n = sTrmp3DMarioCount;
    if (n > 0) {
        e = sTrmp3DMarios;
        b = pmf_Trmp3DMario_StateOffstage;
        first = b[0];
        do {
            int *p = MARIO3D(e)->mState;
            if (p[0] == first) {
                if (p[1] == b[1])
                    goto next;
                if (*(volatile int *)MARIO3D(e)->mState == 0)
                    goto next;
            }
            return 0;
        next:
            e += 0xd0;
            i++;
        } while (i < n);
    }
    return 1;
}

void Trmp3DMario_StartAll(void) {
    int i;
    int off;
    i = 0;
    sTrmp3DGravityStep = 0x100;
    sTrmp3DFallSpeed = -0x4800;
    if (sTrmp3DMarioCount <= 0)
        return;
    off = 0;
    do {
        ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + off))->EnterByKind();
        ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + off))->UpdateMatrix();
        i++;
        off += 0xd0;
    } while (i < sTrmp3DMarioCount);
}

void Trmp3DMario_DrawAll(void) {
    int v[3];
    int w[3];

    v[0] = 0;
    v[1] = 0;
    v[2] = -0xff8;
    func_0203cd80((int *)v, 0x2000);
    func_0203ccd4((Vector3 *)v, 0);
    G3_LightVector(GX_LIGHTID_0, v[0], v[1], v[2]);

    w[0] = 0;
    w[1] = 0;
    w[2] = -0xff8;
    func_0203cd80((int *)w, -0x2000);
    G3_LightVector(GX_LIGHTID_1, w[0], w[1], w[2]);

    {
        int i;
        for (i = 0; i < sTrmp3DMarioCount; i++)
            ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + i * 0xd0))->Render();
    }

    Puff_DrawAll();
}

void Trmp3DMario_UpdateAll(void) {
    int i;
    for (i = 0; i < sTrmp3DMarioCount; i++)
        ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + i * 0xd0))->Update();
    if (sTrmpMariosOnLedges != 0)
        sTrmpAnyOnLedge = 1;
    else
        sTrmpAnyOnLedge = 0;
    Puff_UpdateAll();
}

void Trmp3DMario_ResetAll(unsigned int n, int arg1) {
    int i;
    Trmp3DMario_SetTuning((n % 5 + n / 5) * 0x150 + 0x1000);
    Trmp3DMario_SetSpawnHeight(arg1);
    sTrmp3DLives = 3;
    sTrmpComboCount = 0;
    sTrmpMariosInPlay = 0;
    sTrmpMariosOnLedges = 0;
    for (i = 0; i < sTrmp3DMarioCount; i++) {
        ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + i * 0xd0))->EnterOffstage();
        ((dMgTrmpln3DMario_c *)(sTrmp3DMarios + i * 0xd0))->UpdateMatrix();
    }
    Puff_ClearAll(0, 0);
}

void Trmp3DMario_SetTuning(int a) {
    int v;
    if (a > 0x2800)
        a = 0x2800;
    v = FX_Sqrt(a);
    sTrmp3DFallSpeed = FX_MUL(v, -0x2400);
    sTrmp3DGravityStep = FX_MUL(a, 0xe0);
    sTrmp3DJumpOffSpeed = FX_MUL(v, 0xc00);
    sTrmp3DBounceScale = FX_MUL(v, 0x4800);
    sTrmp3DHardBounceScale = FX_MUL(v, 0x5400);
}

void Trmp3DMario_SetSpawnHeight(int n) {
    sTrmpRowHeights[0] = sTrmpRowBaseHeights[0] + (n << 12);
    sTrmpRowHeights[1] = sTrmpRowBaseHeights[1] + (n << 12);
}

int Trmp3DMario_InitPool(unsigned char *a0, int a1) {
    int i = 0;
    int off;
    char *x;
    sTrmp3DMarioCount = a1;
    sTrmp3DMarios = (char *)a0;
    if (a1 > 0) {
        off = 0;
        do {
            if (((dMgTrmpln3DMario_c *)(sTrmp3DMarios + off))->Load() == 0)
                return 0;
            x = sTrmp3DMarios + off;
            if (i < 5)
                sTrmpJumpers[i] = x;
            i++;
            off += 0xd0;
        } while (i < sTrmp3DMarioCount);
    }
    return 1;
}

/* The Clear slot each bounce runs also restarts the combo
   TrmpItem::UpdateWithJumper scores from. */
void dMgTrmpRingObjAdapter_c::Clear() {
    dMgTrmpObjAdapter_c::Clear();
    mItemHitCount = 0;
}

/* The base adapter is constructed before the body. */
dMgTrmpRingObjAdapter_c::dMgTrmpRingObjAdapter_c() {
    mItemHitCount = 0;
}

void dMgTrmpObjAdapter_c::Clear() {
    mContact = 0;
}

dMgTrmpObjAdapter_c::dMgTrmpObjAdapter_c() {
    mContact = 0;
}

/* Zero-filled .bss in the same span as the handles; none of it is dynamic.
 * The anim pointers are declared above. */
BCA_File *sTrmpAnimJump;
BCA_File *sTrmpAnimDoubleJump1;
BCA_File *sTrmpAnimWallJump;
BCA_File *sTrmpAnimHangUp;
BCA_File *sTrmpAnimRollJumpEnd;
BCA_File *sTrmpAnimWalk;
BCA_File *sTrmpAnimDoubleJump2;
BCA_File *sTrmpAnimJumpEnd;
BCA_File *sTrmpAnimTurn;
BCA_File *sTrmpAnimTurnEnd;
BCA_File *sTrmpAnimBounce;
BCA_File *sTrmpAnimStruggle;
BCA_File *sTrmpAnimSdown;
char *sTrmp3DMarios;
char *sTrmp2Marios;
int sTrmpMariosOnLedges;
int sTrmpGoalSlotCursor;
int sTrmpComboCount;
int sTrmpMariosInPlay;
int sTrmp3DMarioCount;
int data_ov006_02140598;
int sTrmpItemHitsOff;
int sTrmp2MarioTarget;
int sTrmp2MarioCount;

/* Definition order is the retail __sinit order. The rodata words are copied
 * at startup, so these stay dynamic. */
extern int data_ov006_0212e000;
extern int data_ov006_0212e020;
extern int data_ov006_0212dffc;
extern int data_ov006_0212e008;
extern int data_ov006_0212e004;
extern int data_ov006_0212e010;
extern int data_ov006_0212e00c;
extern int data_ov006_0212e018;
extern int data_ov006_0212e014;
extern int data_ov006_0212dff8;
extern int data_ov006_0212e01c;

Trmpln2ModelFilePtr sTrmpMarioModelFile(0x216);
Trmpln2MarioAnimFilePtr sTrmpMarioJumpAnimFile(0x8078);
Trmpln2MarioAnimFilePtr sTrmpMarioDoubleJump1AnimFile(0x801d);
Trmpln2MarioAnimFilePtr sTrmpMarioWallJumpAnimFile(0x80be);
Trmpln2MarioAnimFilePtr sTrmpMarioHangUpAnimFile(0x8071);
Trmpln2MarioAnimFilePtr sTrmpMarioRollJumpEndAnimFile(0x808f);
Trmpln2MarioAnimFilePtr sTrmpMarioWalkAnimFile(0x80b4);
Trmpln2MarioAnimFilePtr sTrmpMarioDoubleJump2AnimFile(0x801e);
Trmpln2MarioAnimFilePtr sTrmpMarioJumpEndAnimFile(0x8077);
Trmpln2MarioAnimFilePtr sTrmpMarioTurnAnimFile(0x80ac);
Trmpln2MarioAnimFilePtr sTrmpMarioTurnEndAnimFile(0x80ab);
Trmpln2MarioAnimFilePtr sTrmpMarioRollJumpAnimFile(0x808e);
Trmpln2MarioAnimFilePtr sTrmpMarioStruggleAnimFile(0x80a4);
Trmpln2MarioAnimFilePtr sTrmpMarioSdownAnimFile(0x8092);

int sTrmpRowHeights[2] = {sTrmpRowBaseHeights[0], sTrmpRowBaseHeights[1]};
int sTrmp3DFallSpeed = data_ov006_0212e000;
int sTrmp3DGravityStep = data_ov006_0212e020;
int sTrmp3DJumpOffSpeed = data_ov006_0212dffc;
int sTrmp3DBounceScale = data_ov006_0212e008;
int sTrmp3DHardBounceScale = data_ov006_0212e004;

TrmpGoal sTrmpGoalSlots(-0x50000, 0x100000, 0, 0);
TrmpGoal data_ov006_02140784(-0x30000, 0x100000, 0);
TrmpGoal data_ov006_02140790(0x10000, 0x100000, 0);
TrmpGoal data_ov006_0214079c(0x10000, 0x100000, 0);
TrmpGoal data_ov006_021407a8(0x30000, 0x100000, 0);
TrmpGoal data_ov006_021407b4(0x50000, 0x100000, 0);

int sTrmp2FallSpeed = data_ov006_0212e010;
int sTrmp2GravityStep = data_ov006_0212e00c;
int data_ov006_0214053c = data_ov006_0212e018;
int sTrmpSpeedScale = data_ov006_0212e014;
int sTrmp2RiseSpeedMax = data_ov006_0212dff8;
int sTrmp2DriftScale = data_ov006_0212e01c;

TrmpGoal data_ov006_021407c0(-0x50000, 0x100000, 0, 0);
TrmpGoal data_ov006_021407cc(-0x30000, 0x100000, 0);
TrmpGoal data_ov006_021407d8(0x10000, 0x100000, 0);
TrmpGoal data_ov006_021407e4(0x10000, 0x100000, 0);
TrmpGoal data_ov006_021407f0(0x30000, 0x100000, 0);
TrmpGoal data_ov006_021407fc(0x50000, 0x100000, 0);
