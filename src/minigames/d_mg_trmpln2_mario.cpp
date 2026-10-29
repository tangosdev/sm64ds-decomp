//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
/*
 * The two player-controlled Mario classes of the trampoline minigames,
 * dMgTrmpln2Mario_c and dMgTrmpln3DMario_c, with their helpers and the
 * object-adapter functions that follow them: 88 functions,
 * ov006 .text 0x020c8dd4..0x020cd744.
 *
 * ROM evidence: D1 0x020ca604 and constructor 0x020ca78c of
 * dMgTrmpln2Mario_c (the constructor was misfiled as
 * _ZN6Player29TryExitCharacterDoorWithIntroEv: it stores vptr
 * _ZTV17dMgTrmpln2Mario_c and constructs a dExtAnmModel_c at +0x78);
 * D1 0x020ccfc8 and constructor 0x020cd12c of dMgTrmpln3DMario_c;
 * _ZTI17dMgTrmpln2Mario_c 0x0213b250, _ZTI18dMgTrmpln3DMario_c 0x0213b244,
 * beside dMgTrmpObjAdapter_c / dMgTrmpRingObjAdapter_c, whose base-object
 * constructors 0x020cd72c / 0x020cd6f4 close the unit. No call runs from
 * one class's methods into the other's. Functions are written in ROM order
 * under defer_codegen off; do not reorder.
 *
 * Each function keeps its own namespace (ns_<address>) because the shards
 * declare the same names with different shapes; mangled-name callees are
 * declared once at global scope. Five two-word struct copies go through
 * the W2 array-member struct so the build keeps the load-load-store-store
 * order; the two file-global pragmas stay bracketed around their function.
 */

// The hierarchy is real (include/dMgTrmpObjAdapter_c.h): both Marios'
// constructors and non-virtual destructors, the two adapters' base-object
// constructors and their Clear slots are methods. The adapters' Clear are their
// key functions, so this TU emits those two vtables and typeinfos (the manifest
// discards them against the cartridge's copies); the Mario vtables stay imported.
//
// leftovers: the rest of both classes stays address-named free functions over
// char* -- their four slot functions included, which is why the Marios' slot
// overrides are declared and not defined. The per-shard namespaces stay
// (independent decl shapes would collide). The records are read through
// dMgTrmpln2Mario_c.h / dMgTrmpln3DMario_c.h (MARIO2 / MARIO3D). Each embeds a
// real dExtAnmModel_c at +0x78/+0x6c, so the model, frame-controller and
// anim-file reaches are member calls on mAnmModel (SetFile, SetAnim, Render,
// Advance, WillHitFrame, Finished, file, mat4x3, currFrame). The thirteen
// anim-file globals are BCA_File *, declared once below. Shared resource,
// model, frame-controller and sound calls use their genuine API headers. Every
// Trmp2Mario_* / Trmp3DMario_* record access spells a MARIO2 / MARIO3D member
// except Trmp2 +0x68 and Trmp3D +0x5c, which stay raw: each is only ever stored
// 0 (BounceOffLineA, EnterBounce), and no reader gives it a role.
#pragma defer_codegen off
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dExtAnmModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "Sound.h"
#include "dMgTrmpln2Mario_c.h"
#include "dMgTrmpln3DMario_c.h"
#include "math/Approach.h"
/* Typed access to the Mario records from the char* helpers below. */
#define MARIO2(p) ((dMgTrmpln2Mario_c *)(p))
#define MARIO3D(p) ((dMgTrmpln3DMario_c *)(p))
/* The animation files, loaded by Trmp2Mario_Load / Trmp3DMario_Load and cleared by
 * the destructors; the states pass them to SetAnim and compare them with
 * mAnmModel.file. sTrmpBounceAnimTable picks one by mAnim. */
extern "C" BCA_File *sTrmpAnimJump, *sTrmpAnimDoubleJump1, *sTrmpAnimWallJump, *sTrmpAnimHangUp,
    *sTrmpAnimRollJumpEnd, *sTrmpAnimWalk, *sTrmpAnimDoubleJump2, *sTrmpAnimJumpEnd, *sTrmpAnimTurn,
    *sTrmpAnimTurnEnd, *sTrmpAnimBounce, *sTrmpAnimStruggle, *sTrmpAnimSdown;
extern "C" BCA_File **sTrmpBounceAnimTable[];
struct W2 {
    int w[2];
};
namespace cstd {
int fdiv(s32, s32);
}

// ---- Trmp2Mario_GetVelocity
namespace ns_020c8dd4 {
extern "C" {
// @symbol Trmp2Mario_GetVelocity
int Trmp2Mario_GetVelocity(int p) {
    return p + 60;
}
}
}
// ---- Trmp2Mario_Reset
namespace ns_020c8ddc {
extern "C" {
extern "C" int pmf_Trmp2Mario_StateMissed[2];
extern "C" int pmf_Trmp2Mario_StateHidden_4[2];

// @symbol Trmp2Mario_Reset
int Trmp2Mario_Reset(char *c) {
    int *p;
    int *d;
    int result;
    int flag;

    p = MARIO2(c)->mState;
    d = pmf_Trmp2Mario_StateMissed;
    result = 0;
    flag = 1;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (MARIO2(c)->mState[0] != 0)
                goto after1;
        }
        flag = 0;
    }
after1:
    if (flag != 0) {
        p = MARIO2(c)->mState;
        d = pmf_Trmp2Mario_StateHidden_4;
        flag = 1;
        if (p[0] == d[0]) {
            if (p[1] != d[1]) {
                if (MARIO2(c)->mState[0] != 0)
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
}
}
// ---- Trmp2Mario_GetPrevPos
namespace ns_020c8e80 {
extern "C" {
// @symbol Trmp2Mario_GetPrevPos
int Trmp2Mario_GetPrevPos(int p) {
    return p + 48;
}
}
}
// ---- Trmp2Mario_GetPos
namespace ns_020c8e88 {
extern "C" {
// @symbol Trmp2Mario_GetPos
int Trmp2Mario_GetPos(int p) {
    return p + 36;
}
}
}
// ---- Trmp2Mario_Render
namespace ns_020c8e90 {
extern "C" {
extern int sTrmp2MarioScale[];
// @symbol Trmp2Mario_Render
void Trmp2Mario_Render(char *self) {
    if (MARIO2(self)->mVisible == 0)
        return;
    dExtAnmModel_c *model = &MARIO2(self)->mAnmModel;
    model->Render((const Vector3 *)sTrmp2MarioScale);
}
}
}
// ---- Trmp2Mario_UpdateMatrix
namespace ns_020c8ecc {
/* recovered: shared common types */
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
// @symbol Trmp2Mario_UpdateMatrix
void Trmp2Mario_UpdateMatrix(char *self) {
    Matrix4x3_FromTranslation(&data_020a0e68, MARIO2(self)->mPos.x, MARIO2(self)->mPos.y,
                              MARIO2(self)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, MARIO2(self)->mRotY);
    MARIO2(self)->mAnmModel.mat4x3 = data_020a0e68;
}
}
}
// ---- Trmp2Mario_Update
namespace ns_020c8f20 {
struct Vector3_16f;
struct Callback;
typedef int Fix12;
struct System {
    static System *New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f *, Callback *);
};

extern "C" {
extern void Trmp2Mario_ApplyGravity(char *o);
extern void Trmp2Mario_UpdateMatrix(char *o);
}
System *System::New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f *, Callback *);

typedef void (*PMF)(void *);
struct Closure {
    int off;
    int adj;
};

// @symbol Trmp2Mario_Update
extern "C" void Trmp2Mario_Update(char *o) {
    MARIO2(o)->mPrevPos.x = MARIO2(o)->mPos.x;
    MARIO2(o)->mPrevPos.y = MARIO2(o)->mPos.y;
    MARIO2(o)->mPrevPos.z = MARIO2(o)->mPos.z;
    if (MARIO2(o)->mState[0] != 0) {
        Closure *cl = (Closure *)MARIO2(o)->mState;
        void *tobj = o + (cl->adj >> 1);
        void (*fn)();
        if (cl->adj & 1)
            fn = *(void (**)())(*(char **)tobj + cl->off);
        else
            fn = (void (*)())cl->off;
        ((void (*)(void *))fn)(tobj);
    }
    Trmp2Mario_ApplyGravity(o);
    Fix12 v = FX_MUL(MARIO2(o)->mBounceVy, 0xb4b);
    if (MARIO2(o)->mVel.y > v) {
        if (MARIO2(o)->mAnmModel.file == sTrmpAnimBounce) {
            MARIO2(o)->mParticleId =
                (int)Particle::System::New(MARIO2(o)->mParticleId, 0xf5, MARIO2(o)->mPos.x << 3,
                                           MARIO2(o)->mPos.y << 3, MARIO2(o)->mPos.z << 3, 0, 0);
        }
    }
    Trmp2Mario_UpdateMatrix(o);
    MARIO2(o)->mAnmModel.Advance();
}
}
// ---- Trmp2Mario_ApplyGravity
namespace ns_020c9024 {
extern "C" {
extern int sTrmp2FallSpeed[];
void AddVec3(void *dst, void *a, void *b);
// @symbol Trmp2Mario_ApplyGravity
void Trmp2Mario_ApplyGravity(char *o) {
    ApproachLinear(MARIO2(o)->mVel.y, sTrmp2FallSpeed[0], MARIO2(o)->mGravityStep);
    AddVec3(&MARIO2(o)->mPos, &MARIO2(o)->mVel, &MARIO2(o)->mPos);
}
}
}
// ---- Trmp2Mario_StateKnockedOut
namespace ns_020c905c {
extern "C" {
extern void Trmp2Mario_EnterHidden(char *o);
// @symbol Trmp2Mario_StateKnockedOut
void Trmp2Mario_StateKnockedOut(char *o) {
    if (MARIO2(o)->mPos.y >= -0x120000)
        return;
    MARIO2(o)->mVel.y = 0;
    Trmp2Mario_EnterHidden(o);
}
}
}
// ---- Trmp2Mario_EnterByKind
namespace ns_020c9098 {

extern "C" {
extern int pmf_Trmp2Mario_StateHidden_5[2];
extern int pmf_Trmp2Mario_StateRespawnWait[2];
extern int pmf_Trmp2Mario_StateKnockedOut[2];
extern void Trmp2Mario_EnterHidden(char *c);
extern void D3D_PlaySeAtX(int a, int b);
}

// @symbol Trmp2Mario_EnterByKind
extern "C" void Trmp2Mario_EnterByKind(char *c) {
    int *p;
    int *d;

    p = MARIO2(c)->mState;
    d = pmf_Trmp2Mario_StateHidden_5;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (MARIO2(c)->mState[0] != 0)
                goto check2;
        }
        return;
    }
check2:
    p = MARIO2(c)->mState;
    d = pmf_Trmp2Mario_StateRespawnWait;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (MARIO2(c)->mState[0] != 0)
                goto body;
        }
        Trmp2Mario_EnterHidden(c);
        return;
    }
body:
    MARIO2(c)->mGravityStep = 0x100;
    MARIO2(c)->mVel.x = 0;
    MARIO2(c)->mVel.y = 0x2000;
    D3D_PlaySeAtX(0x1c9, MARIO2(c)->mPos.x);
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    MARIO2(c)->mAnmModel.currFrame = 0;
    {
        int a = pmf_Trmp2Mario_StateKnockedOut[0];
        int b = pmf_Trmp2Mario_StateKnockedOut[1];
        MARIO2(c)->mState[0] = b ? a : a;
        MARIO2(c)->mState[1] = b;
    }
}
}
// ---- Trmp2Mario_StateBounce
namespace ns_020c91ac {

extern "C" {
void Trmp2Mario_EnterFall(void *c);
void D3D_PlaySeAtX(int a0, int a1);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
}

extern "C" void *sMgMainCamera;
extern "C" int sTrmpSpeedScale;
extern "C" int sTrmp2RiseSpeedMax;
extern "C" int sTrmp2FallSpeed;

// @symbol Trmp2Mario_StateBounce
extern "C" void Trmp2Mario_StateBounce(char *c) {
    int v24;

    if (MARIO2(c)->mVel.y <= 0) {
        Trmp2Mario_EnterFall(c);
        return;
    }

    if (MARIO2(c)->mAnmModel.file == sTrmpAnimBounce) {
        if (MARIO2(c)->mAnmModel.WillHitFrame(0xc) != 0 ||
            MARIO2(c)->mAnmModel.WillHitFrame(0x18) != 0) {
            D3D_PlaySeAtX(0x1b5, MARIO2(c)->mPos.x);
        }
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO2(c)->mPos, MARIO2(c)->mScreenPos);

    v24 = MARIO2(c)->mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (MARIO2(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (MARIO2(c)->mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    MARIO2(c)->mPos.x = v;
}

    if (MARIO2(c)->mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(MARIO2(c)->mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        int t;

        MARIO2(c)->mAnim = 2;
        MARIO2(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO2(c)->mAnim], 0x40000000, 0x800, 0);

        MARIO2(c)->mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, MARIO2(c)->mAnim, MARIO2(c)->mPos.x);

        t = FX_MUL(-MARIO2(c)->mVel.x, 0x1200);
        if (t < -0x1400)
            t = -0x1400;
        else if (t > 0x1400)
            t = 0x1400;
        MARIO2(c)->mVel.x = t;

        MARIO2(c)->mVel.y += FX_MUL(sTrmpSpeedScale, 0x400);

        {
            int v40 = sTrmp2RiseSpeedMax;
            if (MARIO2(c)->mVel.y <= v40)
                v40 = MARIO2(c)->mVel.y;
            MARIO2(c)->mVel.y = v40;
        }
    }
    goto L260;

L1a4: {
    int t = FX_MUL(-MARIO2(c)->mVel.x, 0xf00);
    int v40;
    int hi;
    int lo;

    MARIO2(c)->mVel.x = t;

    t = MARIO2(c)->mPos.x;
    if (t < -0x68000)
        t = -0x68000;
    else if (t > 0x68000)
        t = 0x68000;
    MARIO2(c)->mPos.x = t;

    hi = sTrmp2RiseSpeedMax;
    lo = sTrmp2FallSpeed;
    v40 = MARIO2(c)->mVel.y;
    if (v40 < lo) {
        v40 = lo;
    } else {
        if (v40 <= hi)
            hi = v40;
        v40 = hi;
    }
    MARIO2(c)->mVel.y = v40;
}
    goto L260;

L230:
    if (MARIO2(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO2(c)->mVel.x *= -1;
    }

L260: {
    int v = MARIO2(c)->mVel.x;
    if (v > 0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(MARIO2(c)->mRotY, (short)v, 0xc00);
    }
}
}
}
// ---- Trmp2Mario_BounceOffLineA
namespace ns_020c94e0 {

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void *d_mg_trmpln2_marioNewHold(unsigned int a, unsigned int b, int fx1, int fx2, int fx3,
                                const void *vec, void *cb);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
int D3D_PlaySeAtX(int a, int b);
void Trmp2Mario_StateBounce(char *c);
}

extern "C" int sTrmp2RiseSpeedMax;
extern "C" int sTrmp2GravityStep;
extern "C" int pmf_Trmp2Mario_StateAppearWait_2[2];
extern "C" int pmf_Trmp2Mario_StateBounce_2[2];

// @symbol Trmp2Mario_BounceOffLineA
extern "C" void Trmp2Mario_BounceOffLineA(char *c) {
    Vector3 tmp, tmp2, tmp3;
    int alongLine, lineHalfLength, dot, scale, fdivr, v, fx, cur;

    alongLine = MARIO2(c)->mContactOffset;
    lineHalfLength = MARIO2(c)->mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)&MARIO2(c)->mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3(&MARIO2(c)->mVel, &tmp);
    scale = FX_MUL(dot, 0x1400);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3(&MARIO2(c)->mVel, &tmp2, &MARIO2(c)->mVel);

    fdivr = cstd::fdiv(lineHalfLength, lineHalfLength + alongLine);
    scale = FX_MUL(sTrmp2RiseSpeedMax, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    AddVec3(&MARIO2(c)->mVel, &tmp3, &MARIO2(c)->mVel);

    v = MARIO2(c)->mVel.x;
    if (v < -0x3000)
        v = -0x3000;
    else if (v > 0x3000)
        v = 0x3000;
    MARIO2(c)->mVel.x = v;

    fx = FX_MUL(sTrmp2RiseSpeedMax, 0xc00);
    cur = MARIO2(c)->mVel.y;
    if (cur >= fx)
        fx = cur;
    MARIO2(c)->mVel.y = fx;

    MARIO2(c)->mGravityStep = sTrmp2GravityStep;

    {
        Vector3_16 vec16;
        int y = tmp.y;
        int z = tmp.z;
        int x = tmp.x;
        vec16.x = (s16)x;
        vec16.y = (s16)y;
        char *bp = (char *)MARIO2(c)->mState;
        int *s = (int *)(bp);
        vec16.z = (s16)(z + (int)((unsigned)bp - (unsigned)bp));

        int *q = pmf_Trmp2Mario_StateAppearWait_2;
        if (s[0] != q[0] || (s[1] != q[1] && MARIO2(c)->mState[0] != 0)) {

            Particle::System::New(0, 0xf3, (MARIO2(c)->mPos.x) << 3, (MARIO2(c)->mPos.y) << 3,
                                  (MARIO2(c)->mPos.z) << 3, (const Vector3_16f *)(&vec16), 0);
            Particle::System::New(0, 0xf4, (MARIO2(c)->mPos.x) << 3, (MARIO2(c)->mPos.y) << 3,
                                  (MARIO2(c)->mPos.z) << 3, (const Vector3_16f *)(&vec16), 0);
            MARIO2(c)->mParticleId = (int)Particle::System::New(
                MARIO2(c)->mParticleId, 0xf5, (MARIO2(c)->mPos.x) << 3, (MARIO2(c)->mPos.y) << 3,
                (MARIO2(c)->mPos.z) << 3, 0, 0);
        }
    }

    MARIO2(c)->mBounceVy = MARIO2(c)->mVel.y;
    ((dMgTrmpObjAdapter_c *)c)->Clear();
    MARIO2(c)->mWallBumpTimer = 0;
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);
    MARIO2(c)->mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, 4, MARIO2(c)->mPos.x);
    D3D_PlaySeAtX(0x1b5, MARIO2(c)->mPos.x);
    *(int *)(c + 0x68) = 0; /* stays raw: only ever stored 0, no reader */
    {
        int a = pmf_Trmp2Mario_StateBounce_2[0];
        int b = pmf_Trmp2Mario_StateBounce_2[1];
        a = b ? a : a;
        MARIO2(c)->mState[0] = a;
        MARIO2(c)->mState[1] = b;
    }
    Trmp2Mario_StateBounce(c);
}
}
// ---- Trmp2Mario_StateWallBounce
namespace ns_020c97bc {

extern "C" {
void Trmp2Mario_EnterFall(void *c);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
}

extern "C" void *sMgMainCamera;
extern "C" int sTrmpSpeedScale;
extern "C" int sTrmp2RiseSpeedMax;
extern "C" int sTrmp2FallSpeed;

// @symbol Trmp2Mario_StateWallBounce
extern "C" void Trmp2Mario_StateWallBounce(char *c) {
    int v24;

    if (MARIO2(c)->mVel.y <= 0) {
        Trmp2Mario_EnterFall(c);
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO2(c)->mPos, MARIO2(c)->mScreenPos);

    v24 = MARIO2(c)->mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (MARIO2(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (MARIO2(c)->mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    MARIO2(c)->mPos.x = v;
}

    if (MARIO2(c)->mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(MARIO2(c)->mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        int t = FX_MUL(-MARIO2(c)->mVel.x, 0x1200);
        int v40;

        if (t < -0x1400)
            t = -0x1400;
        else if (t > 0x1400)
            t = 0x1400;
        MARIO2(c)->mVel.x = t;

        MARIO2(c)->mVel.y += FX_MUL(sTrmpSpeedScale, 0x400);

        v40 = sTrmp2RiseSpeedMax;
        if (MARIO2(c)->mVel.y <= v40)
            v40 = MARIO2(c)->mVel.y;
        MARIO2(c)->mVel.y = v40;

        MARIO2(c)->mAnim = 2;
        MARIO2(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO2(c)->mAnim], 0x40000000, 0x800, 0);

        MARIO2(c)->mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, MARIO2(c)->mAnim, MARIO2(c)->mPos.x);
    }
    goto L260;

L1a4: {
    int t = FX_MUL(-MARIO2(c)->mVel.x, 0xf00);
    int pos;
    int hi;
    int lo;
    int v40;

    MARIO2(c)->mVel.x = t;

    pos = MARIO2(c)->mPos.x;
    if (pos < -0x68000)
        pos = -0x68000;
    else if (pos > 0x68000)
        pos = 0x68000;
    MARIO2(c)->mPos.x = pos;

    hi = sTrmp2RiseSpeedMax;
    lo = sTrmp2FallSpeed;
    v40 = MARIO2(c)->mVel.y;
    if (v40 < lo) {
        v40 = lo;
    } else {
        if (v40 <= hi)
            hi = v40;
        v40 = hi;
    }
    MARIO2(c)->mVel.y = v40;
}
    goto L260;

L230:
    if (MARIO2(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO2(c)->mVel.x *= -1;
    }

L260: {
    int v = MARIO2(c)->mVel.x;
    if (v > 0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(MARIO2(c)->mRotY, (short)v, 0xc00);
    }
}
}
}
// ---- Trmp2Mario_BounceOffLineB
namespace ns_020c9aa0 {
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void Trmp2Mario_StateWallBounce(char *c);
}

// @symbol Trmp2Mario_BounceOffLineB
extern "C" void Trmp2Mario_BounceOffLineB(char *c) {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp3;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;
    int vx;

    alongLine = MARIO2(c)->mContactOffset;
    lineHalfLength = MARIO2(c)->mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)&MARIO2(c)->mContactNormal;
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3(&MARIO2(c)->mVel, &tmp);
    scale = FX_MUL(dot, 0x1200);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3(&MARIO2(c)->mVel, &tmp2, &MARIO2(c)->mVel);

    fdivr = cstd::fdiv(lineHalfLength, lineHalfLength + alongLine);
    scale = FX_MUL(sTrmpSpeedScale, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    AddVec3(&MARIO2(c)->mVel, &tmp3, &MARIO2(c)->mVel);

    vx = MARIO2(c)->mVel.x;
    if (vx < -0x2800)
        vx = -0x2800;
    else if (vx > 0x2800)
        vx = 0x2800;
    MARIO2(c)->mVel.x = vx;

    {
        int fx = FX_MUL(sTrmpSpeedScale, 0x600);
        int cur = MARIO2(c)->mVel.y;
        if (cur >= fx)
            fx = cur;
        MARIO2(c)->mVel.y = fx;
    }

    MARIO2(c)->mGravityStep = sTrmp2GravityStep;
    ((dMgTrmpObjAdapter_c *)c)->Clear();

    MARIO2(c)->mWallBumpTimer = 0;
    if (MARIO2(c)->mAnim != 0) {
        MARIO2(c)->mAnim = 0;
    } else {
        MARIO2(c)->mAnim = 1;
    }

    MARIO2(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO2(c)->mAnim], 0x40000000, 0x800, 0);

    MARIO2(c)->mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, MARIO2(c)->mAnim, MARIO2(c)->mPos.x);

    {
        int a = pmf_Trmp2Mario_StateWallBounce_2[0];
        int b = pmf_Trmp2Mario_StateWallBounce_2[1];
        a = b ? a : a;
        MARIO2(c)->mState[0] = a;
        MARIO2(c)->mState[1] = b;
    }
    Trmp2Mario_StateWallBounce(c);
}
}
// ---- Trmp2Mario_EnterBounce
namespace ns_020c9c8c {

struct Pair {
    int a, b;
};

extern "C" {
extern int sTrmp2GravityStep;
extern int sTrmpSpeedScale;
extern Pair pmf_Trmp2Mario_StateBounce;
extern void D3D_PlaySeAtX(int a, int b);
extern void Trmp2Mario_StateBounce(char *c);
}

// @symbol Trmp2Mario_EnterBounce
extern "C" void Trmp2Mario_EnterBounce(char *c) {
    dMgTrmpObjAdapter_c *callbacks = (dMgTrmpObjAdapter_c *)c;
    int a, b;
    MARIO2(c)->mGravityStep = sTrmp2GravityStep;
    MARIO2(c)->mVel.y = FX_MUL(sTrmpSpeedScale, 0xc00);
    MARIO2(c)->mBounceVy = MARIO2(c)->mVel.y;
    callbacks->Clear();
    MARIO2(c)->mWallBumpTimer = 0;
    MARIO2(c)->mAnim = 0;
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);
    MARIO2(c)->mAnmModel.currFrame = 0;
    D3D_PlaySeAtX(0x110, MARIO2(c)->mPos.x);
    D3D_PlaySeAtX(0x1b5, MARIO2(c)->mPos.x);
    *(int *)(c + 0x68) = 0; /* stays raw: only ever stored 0, no reader */
    a = pmf_Trmp2Mario_StateBounce.a;
    b = pmf_Trmp2Mario_StateBounce.b;
    MARIO2(c)->mState[0] = b ? a : a;
    MARIO2(c)->mState[1] = b;
    Trmp2Mario_StateBounce(c);
}
}
// ---- Trmp2Mario_StateRespawnWait
namespace ns_020c9d7c {
extern "C" {
/* best near-miss div=2 @ mwccarm 1.2/sp2p3 */
extern int RandomIntInternal(int *seed);
extern void Trmp2Mario_EnterFall(void *c);
extern "C" int data_0209e650;
extern "C" int sTrmp2DriftScale;

// @symbol Trmp2Mario_StateRespawnWait
void Trmp2Mario_StateRespawnWait(char *c) {
    unsigned int r;
    int tbl;
    MARIO2(c)->mTimer -= 1;
    if (MARIO2(c)->mTimer == 0) {
        MARIO2(c)->mPos.y = 0x100000;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        MARIO2(c)->mPos.x = ((int)r - 0x800) * 0xc0;
        MARIO2(c)->mVel.y = 0;
        tbl = sTrmp2DriftScale;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        MARIO2(c)->mVel.x = FX_MUL(((int)r - 0x800) << 1, tbl);
        MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimWallJump, 0x40000000, 0x800, 0);
        MARIO2(c)->mPrevPos.x = MARIO2(c)->mPos.x;
        MARIO2(c)->mPrevPos.y = MARIO2(c)->mPos.y;
        MARIO2(c)->mPrevPos.z = MARIO2(c)->mPos.z;
        Trmp2Mario_EnterFall(c);
    } else {
        MARIO2(c)->mPos.y = 0x100000;
        MARIO2(c)->mVel.y = 0;
    }
}
}
}
// ---- Trmp2Mario_EnterRespawnWait
namespace ns_020c9e7c {
extern "C" {

struct Vec2i {
    int x;
    int y;
};
extern "C" struct Vec2i pmf_Trmp2Mario_StateRespawnWait_2;
// @symbol Trmp2Mario_EnterRespawnWait
void Trmp2Mario_EnterRespawnWait(unsigned char *c) {
    MARIO2(c)->mTimer = 0x3c;
    MARIO2(c)->mPos.y = 0x100000;
    MARIO2(c)->mPrevPos.x = MARIO2(c)->mPos.x;
    MARIO2(c)->mPrevPos.y = MARIO2(c)->mPos.y;
    MARIO2(c)->mPrevPos.z = MARIO2(c)->mPos.z;
    MARIO2(c)->mVel.y = 0;
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimWallJump, 0x40000000, 0x800, 0);
    *(W2 *)MARIO2(c)->mState = *(W2 *)&pmf_Trmp2Mario_StateRespawnWait_2;
}
}
}
// ---- Trmp2Mario_StateMissed
namespace ns_020c9efc {
typedef long long s64;
extern "C" {
extern void Puff_Spawn(int a, int b);
}
extern "C" {
extern void Trmp2Mario_EnterRespawnWait(void *p);
}
extern "C" int data_ov006_02140598;
extern "C" int sTrmp2Lives;

// @symbol Trmp2Mario_StateMissed
extern "C" void Trmp2Mario_StateMissed(char *c) {
    /* `mTimer = mTimer - 1` comes out 4 bytes short; the decrement matches. */
    MARIO2(c)->mTimer--;
    if (MARIO2(c)->mTimer == 0) {
        Puff_Spawn(MARIO2(c)->mScreenPos[0], 0xc0);
        data_ov006_02140598 = data_ov006_02140598 - 1;
        ApproachLinear(sTrmp2Lives, 0, 1);
        Sound::PlayBank2_2D(0x130);
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO2(c)->mWallBumpTimer = 0;
        Trmp2Mario_EnterRespawnWait(c);
    } else {
        int v = MARIO2(c)->mVel.x;
        MARIO2(c)->mVel.x = FX_MUL(v, 0xc00);
        int x = MARIO2(c)->mPos.x;
        if (x < -0x6c000)
            MARIO2(c)->mPos.x = -0x6c000;
        else if (x > 0x6c000)
            MARIO2(c)->mPos.x = 0x6c000;
    }
}
}
// ---- Trmp2Mario_EnterMissed
namespace ns_020c9fe4 {
extern "C" {
extern void D3D_PlaySeAtX(int a, int b);
extern int sTrmp2Lives[];
extern double pmf_Trmp2Mario_StateMissed_2;
void Trmp2Mario_EnterMissed(char *c);
// @symbol Trmp2Mario_EnterMissed
void Trmp2Mario_EnterMissed(char *c) {
    if (sTrmp2Lives[0] > 1)
        D3D_PlaySeAtX(0x1ca, MARIO2(c)->mPos.x);
    else
        Sound::PlayBank2_2D(0x1c9);
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    MARIO2(c)->mTimer = 0x28;
    *(double *)MARIO2(c)->mState = pmf_Trmp2Mario_StateMissed_2;
}
}
}
// ---- Trmp2Mario_StateFall
namespace ns_020ca070 {

extern "C" {
void Trmp2Mario_BounceOffLineB(char *c);
void Trmp2Mario_BounceOffLineA(char *c);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void Trmp2Mario_EnterMissed(char *c);
}

extern "C" void *sMgMainCamera;

struct S8 {
    int a, b;
};
extern "C" S8 pmf_Trmp2Mario_StateWallBounce;

// @symbol Trmp2Mario_StateFall
extern "C" void Trmp2Mario_StateFall(char *c) {
    u16 state = MARIO2(c)->mContact;
    int v24;

    if (state == 1) {
        Trmp2Mario_BounceOffLineB(c);
        return;
    }
    if (state == 2) {
        Trmp2Mario_BounceOffLineA(c);
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO2(c)->mPos, MARIO2(c)->mScreenPos);

    v24 = MARIO2(c)->mPos.x;

    if (v24 >= -0x68000)
        goto L5c;
    if (MARIO2(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v24 <= 0x68000)
        goto L230;
    if (MARIO2(c)->mVel.x <= 0)
        goto L230;

L70: {
    int v = v24;
    if (v < -0x90000)
        v = -0x90000;
    else if (v > 0x90000)
        v = 0x90000;
    MARIO2(c)->mPos.x = v;
}

    MARIO2(c)->mVel.x *= -1;
    if (MARIO2(c)->mVel.y <= -0x800)
        goto L260;

    if (MARIO2(c)->mItemPushCount != 0)
        goto L1a4;
    if (ApproachLinear(MARIO2(c)->mWallBumpTimer, 5, 1) != 0)
        goto L1a4;
    {
        MARIO2(c)->mVel.y = 0x800;
        MARIO2(c)->mAnim = 2;
        MARIO2(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO2(c)->mAnim], 0x40000000, 0x800, 0);

        MARIO2(c)->mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, MARIO2(c)->mAnim, MARIO2(c)->mPos.x);

        {
            long long ll = *(long long *)&pmf_Trmp2Mario_StateWallBounce;
            MARIO2(c)->mState[0] = (int)ll;
            MARIO2(c)->mState[1] = (int)((unsigned long long)ll >> 32);
        }
    }
    return;

L1a4: {
    int t = FX_MUL(-MARIO2(c)->mVel.x, 0xf00);
    int pos;

    MARIO2(c)->mVel.x = t;

    pos = MARIO2(c)->mPos.x;
    if (pos < -0x68000)
        pos = -0x68000;
    else if (pos > 0x68000)
        pos = 0x68000;
    MARIO2(c)->mPos.x = pos;
}
    goto L260;

L230:
    if (MARIO2(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO2(c)->mVel.x *= -1;
    }

L260: {
    int v = MARIO2(c)->mVel.x;
    if (v > 0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, 0x2000, 0xc00);
    } else if (v < -0x2000) {
        ApproachLinear(MARIO2(c)->mRotY, -0x2000, 0xc00);
    } else {
        ApproachLinear(MARIO2(c)->mRotY, (short)v, 0xc00);
    }
}

    if (MARIO2(c)->mPos.y >= -0xc8000)
        return;

    Trmp2Mario_EnterMissed(c);
}
}
// ---- Trmp2Mario_EnterFall
namespace ns_020ca2ec {
extern "C" {
typedef struct {
    int a, b;
} S8;
extern "C" S8 pmf_Trmp2Mario_StateFall;
extern void Trmp2Mario_StateFall(void *self);

// @symbol Trmp2Mario_EnterFall
void Trmp2Mario_EnterFall(void *self) {
    *(W2 *)MARIO2(self)->mState = *(W2 *)&pmf_Trmp2Mario_StateFall;
    Trmp2Mario_StateFall(self);
}
}
}
// ---- Trmp2Mario_StateAppearWait
namespace ns_020ca310 {
extern "C" void Trmp2Mario_EnterBounce(char *, int);
// @symbol Trmp2Mario_StateAppearWait
extern "C" void Trmp2Mario_StateAppearWait(char *c) {
    if (ApproachLinear2(MARIO2(c)->mTimer, 0, 1) == 0) {
        MARIO2(c)->mPos.y = 0;
        return;
    }
    MARIO2(c)->mPos.y = 0;
    MARIO2(c)->mPrevPos.x = MARIO2(c)->mPos.x;
    MARIO2(c)->mPrevPos.y = MARIO2(c)->mPos.y;
    MARIO2(c)->mPrevPos.z = MARIO2(c)->mPos.z;
    MARIO2(c)->mVisible = 1;
    Trmp2Mario_EnterBounce(c, 1);
}
}
// ---- Trmp2Mario_EnterAppearWait
namespace ns_020ca374 {
extern "C" {
struct S2 {
    int w[2];
};
extern "C" struct S2 pmf_Trmp2Mario_StateAppearWait;
// @symbol Trmp2Mario_EnterAppearWait
void Trmp2Mario_EnterAppearWait(char *o, short v) {
    MARIO2(o)->mTimer = v;
    MARIO2(o)->mGravityStep = 0;
    *(struct S2 *)MARIO2(o)->mState = pmf_Trmp2Mario_StateAppearWait;
}
}
}
// ---- Trmp2Mario_StateHidden
namespace ns_020ca39c {
extern "C" {
// @symbol Trmp2Mario_StateHidden
void Trmp2Mario_StateHidden(int *p) {
    MARIO2(p)->mPos.y = 0;
}
}
}
// ---- Trmp2Mario_EnterHidden
namespace ns_020ca3a8 {

extern "C" {
extern int data_020a0ebc[];
extern double pmf_Trmp2Mario_StateHidden_3;
}

// @symbol Trmp2Mario_EnterHidden
extern "C" void Trmp2Mario_EnterHidden(char *c) {
    MARIO2(c)->mVisible = 0;
    MARIO2(c)->mPos.y = 0;
    MARIO2(c)->mPos.x = 0;
    MARIO2(c)->mPos.z = 0;
    MARIO2(c)->mVel.x = data_020a0ebc[0];
    MARIO2(c)->mVel.y = data_020a0ebc[1];
    MARIO2(c)->mVel.z = data_020a0ebc[2];
    MARIO2(c)->mGravityStep = 0;
    MARIO2(c)->mWallBumpTimer = 0;
    MARIO2(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(double *)MARIO2(c)->mState = pmf_Trmp2Mario_StateHidden_3;
}
}
// ---- Trmp2Mario_Load
namespace ns_020ca430 {
extern "C" {
typedef struct BMD_File BMD_File;

extern "C" SharedFilePtr sTrmpMarioModelFile;
extern "C" SharedFilePtr sTrmpMarioJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioDoubleJump1AnimFile;
extern "C" SharedFilePtr sTrmpMarioWallJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioHangUpAnimFile;
extern "C" SharedFilePtr sTrmpMarioRollJumpEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioWalkAnimFile;
extern "C" SharedFilePtr sTrmpMarioDoubleJump2AnimFile;
extern "C" SharedFilePtr sTrmpMarioJumpEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioTurnAnimFile;
extern "C" SharedFilePtr sTrmpMarioTurnEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioRollJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioStruggleAnimFile;
extern "C" SharedFilePtr sTrmpMarioSdownAnimFile;

extern "C" char *sMgMainCamera;

extern void Camera_ProjectToScreen16(char *p, void *q, short *s);
extern void func_02016a14(void *self, int a, int b);
extern void Trmp2Mario_EnterHidden(char *c);

// @symbol Trmp2Mario_Load
int Trmp2Mario_Load(char *c) {
    BMD_File *f;
    int ret;

    f = (BMD_File *)dExtSimpleModel_c::LoadFile(sTrmpMarioModelFile);
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
    ret = MARIO2(c)->mAnmModel.SetFile(f, 1, -1);
    MARIO2(c)->mAnim = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &MARIO2(c)->mPos, MARIO2(c)->mScreenPos);
    MARIO2(c)->mGravityStep = 0;
    func_02016a14(&MARIO2(c)->mAnmModel, 0x73ff, 0);
    func_02016a04(&MARIO2(c)->mAnmModel, 0x3e52);
    Trmp2Mario_EnterHidden(c);
    return ret;
}
}
}
// ---- Trmp2Mario_Destruct
namespace ns_020ca604 {
extern "C" SharedFilePtr sTrmpMarioModelFile, sTrmpMarioJumpAnimFile, sTrmpMarioDoubleJump1AnimFile,
    sTrmpMarioWallJumpAnimFile, sTrmpMarioHangUpAnimFile, sTrmpMarioRollJumpEndAnimFile,
    sTrmpMarioWalkAnimFile, sTrmpMarioDoubleJump2AnimFile, sTrmpMarioJumpEndAnimFile,
    sTrmpMarioTurnAnimFile, sTrmpMarioTurnEndAnimFile, sTrmpMarioRollJumpAnimFile,
    sTrmpMarioStruggleAnimFile, sTrmpMarioSdownAnimFile;
}

/* mAnmModel's destructor, then the ring and base adapters' (each storing its
   own vptr back), run after the body. */
// @symbol _ZN17dMgTrmpln2Mario_cD1Ev
dMgTrmpln2Mario_c::~dMgTrmpln2Mario_c() {
    using namespace ns_020ca604;
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
// ---- Trmp2Mario_Construct
/* dMgTrmpln2Mario_c's constructor, ov006 0x020ca78c. It was filed as
 * _ZN6Player29TryExitCharacterDoorWithIntroEv; it is not a Player method.
 *
 * The shape is a constructor, not a state handler: it calls a base routine,
 * stores a vptr into [this+0], then runs a member constructor at this+0x78.
 * Player's own constructors are _ZN6PlayerC1Ev / C3Ev in ov002, and Player's
 * dExtAnmModel_c members live at 0xf0 and 0x174 -- not 0x78, which on a real Player is
 * dActor_c's mCamSpacePosY.
 *
 * This file was the only evidence for Player.h's `mAnmModel2` at 0x078, so
 * that field can now be dropped and the offset returned to dActor_c.
 *
 * The vptr it stores, 0x0213b2e0, is dMgTrmpln2Mario_c's vtable address point
 * (_ZTV17dMgTrmpln2Mario_c): the two words before it are the offset 0 and
 * _ZTI17dMgTrmpln2Mario_c, and its slots are Trmp2Mario_GetPos, GetPrevPos,
 * GetVelocity, Reset and dMgTrmpRingObjAdapter_c::Clear. 0x0213b2c4 is the same
 * for dMgTrmpln3DMario_c.
 *
 * Detached from Player.h; see SlotB2_Hide.cpp.
 */

/* The ring adapter base and mAnmModel are constructed before the (empty) body. */
// @symbol _ZN17dMgTrmpln2Mario_cC1Ev
dMgTrmpln2Mario_c::dMgTrmpln2Mario_c() {}
// ---- Trmp2Mario_Hide
namespace ns_020ca7b8 {
extern "C" {
// Scans sTrmp2MarioCount entries (stride 0xdc) from sTrmp2Marios:
// returns 0 if any entry's mState pair mismatches pmf_Trmp2Mario_StateHidden_2
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x70];  // 0x00
    int mState[2];     // 0x70, dMgTrmpln2Mario_c::mState
    char _pad78[0x64]; // 0x78 (stride 0xdc)
} Ent;

extern "C" int sTrmp2MarioCount;
extern "C" Ent *sTrmp2Marios;
extern "C" int pmf_Trmp2Mario_StateHidden_2[2];

// @symbol Trmp2Mario_Hide
int Trmp2Mario_Hide(void) {
    int i;
    for (i = 0; i < sTrmp2MarioCount; i++) {
        int *v = sTrmp2Marios[i].mState;
        volatile int *q = (volatile int *)pmf_Trmp2Mario_StateHidden_2;
        if (v[0] != pmf_Trmp2Mario_StateHidden_2[0] ||
            (v[1] != q[1] && sTrmp2Marios[i].mState[0] != 0))
            return 0;
    }
    return 1;
}
}
}
// ---- Trmp2Mario_EnterAll
namespace ns_020ca840 {
extern "C" {
extern void Trmp2Mario_EnterByKind(char *o);
extern void Trmp2Mario_UpdateMatrix(char *o);
extern "C" int sTrmp2MarioCount;
extern "C" int sTrmp2FallSpeed;
extern "C" int sTrmp2GravityStep;
extern "C" int sTrmpItemHitsOff;
extern "C" char *sTrmp2Marios;

// @symbol Trmp2Mario_EnterAll
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
            Trmp2Mario_EnterByKind(sTrmp2Marios + off);
            Trmp2Mario_UpdateMatrix(sTrmp2Marios + off);
            n = sTrmp2MarioCount;
            i++;
            off += 0xdc;
        } while (i < n);
    }
}
}
}
// ---- Trmp2Mario_MoveToStart
namespace ns_020ca8e0 {
extern "C" int sTrmp2MarioTarget;
extern "C" int sTrmp2MarioCount;
extern "C" char *sTrmp2Marios;
extern "C" int pmf_Trmp2Mario_StateHidden[2];
extern "C" int sTrmp2DriftScale;
extern "C" void Trmp2Mario_EnterAppearWait(char *o, short v);
extern "C" void Trmp2Mario_UpdateMatrix(char *o);

// @symbol Trmp2Mario_MoveToStart
extern "C" void Trmp2Mario_MoveToStart(void) {
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

            Trmp2Mario_EnterAppearWait(base + i * 0xdc, (short)(i * 0x14));
            MARIO2(sTrmp2Marios + i * 0xdc)->mPos.x = 0;
            MARIO2(sTrmp2Marios + i * 0xdc)->mVel.x = FX_MUL(sTrmp2DriftScale, (i << 11) - 0x1000);
            Trmp2Mario_UpdateMatrix(sTrmp2Marios + i * 0xdc);
            return;
        }

        i++;
        e += 0xdc;
        if (i < count)
            continue;
        break;
    }
}
}
// ---- Trmp2Mario_PlaceAtStart
namespace ns_020caa08 {
extern "C" {
extern "C" int sTrmp2MarioCount;
extern "C" char *sTrmp2Marios;
extern "C" int sTrmp2DriftScale;
extern void Trmp2Mario_EnterAppearWait(char *o, short v);
extern void Trmp2Mario_UpdateMatrix(char *o);

// @symbol Trmp2Mario_PlaceAtStart
void Trmp2Mario_PlaceAtStart(void) {
    int n;
    int i;
    n = sTrmp2MarioCount;
    if (n > 3)
        n = 3;
    for (i = 0; i < n; i++) {
        Trmp2Mario_EnterAppearWait(sTrmp2Marios + i * 0xdc, (short)(i * 0x14));
        MARIO2(sTrmp2Marios + i * 0xdc)->mPos.x = i * 0x10000 - 0x20000;
        MARIO2(sTrmp2Marios + i * 0xdc)->mVel.x = FX_MUL(sTrmp2DriftScale, i * 0x800 - 0x1000);
        Trmp2Mario_UpdateMatrix(sTrmp2Marios + i * 0xdc);
    }
}
}
}
// ---- Trmp2Mario_DrawAll
namespace ns_020caadc {
extern "C" {
typedef struct {
    int x, y, z;
} Vec3;
extern void func_0203cd80(Vec3 *m, short angle);
extern void func_0203ccd4(Vec3 *m, short angle);
extern void Trmp2Mario_Render(char *o);
extern void Puff_DrawAll(void);
extern "C" int sTrmp2MarioCount;
extern "C" char *sTrmp2Marios;

// @symbol Trmp2Mario_DrawAll
void Trmp2Mario_DrawAll(void) {
    Vec3 v;
    Vec3 w;
    int i, n, off;
    v.x = 0;
    v.y = 0;
    v.z = 0xfffff008;
    func_0203cd80(&v, -0x2000);
    func_0203ccd4(&v, -0x3000);
    *(volatile unsigned *)0x40004c8 = (((short)v.x >> 3) & 0x3ff) |
                                      ((((short)v.y >> 3) & 0x3ff) << 10) |
                                      ((((short)v.z >> 3) & 0x3ff) << 20);
    w.x = 0;
    w.y = 0;
    w.z = 0xfffff008;
    func_0203cd80(&w, -0x2000);
    *(volatile unsigned *)0x40004c8 =
        ((((short)w.x >> 3) & 0x3ff) | ((((short)w.y >> 3) & 0x3ff) << 10) |
         ((((short)w.z >> 3) & 0x3ff) << 20)) |
        0x40000000;
    *(volatile unsigned *)0x40004cc = 0x7fff;
    *(volatile unsigned *)0x40004cc = 0x40007fff;

    n = sTrmp2MarioCount;
    i = 0;
    if (n > 0) {
        off = 0;
        do {
            Trmp2Mario_Render(sTrmp2Marios + off);
            n = sTrmp2MarioCount;
            i++;
            off += 0xdc;
        } while (i < n);
    }
    Puff_DrawAll();
}
}
}
// ---- Trmp2Mario_UpdateAll
namespace ns_020cac30 {
/* ov006, 0x020cac30, size 0x6c. Held the name
 * _ZN6Player12St_Null_InitEv until that symbol was moved to ov002, where an
 * eight-byte `return 1` sits at the same shared address and is reached from
 * ov002's Player::State pointer-to-member table. This function is reached
 * instead by two direct arm_calls from ov006 (module:overlay(6)) and touches
 * no Player field, so nothing here was ever Player's. Back to a placeholder
 * name until its own class is identified.
 */
extern "C" {
extern int sTrmp2MarioCount;
extern char *sTrmp2Marios;
void Trmp2Mario_Update(void *);
void TrmpItem_UpdateWithJumper(void *, int);
void Puff_UpdateAll(void);
// @symbol Trmp2Mario_UpdateAll
void Trmp2Mario_UpdateAll(void) {
    int i = 0;
    if (sTrmp2MarioCount > 0) {
        int off = 0;
        do {
            Trmp2Mario_Update(sTrmp2Marios + off);
            TrmpItem_UpdateWithJumper(sTrmp2Marios + off, i);
            i++;
            off += 0xdc;
        } while (i < sTrmp2MarioCount);
    }
    Puff_UpdateAll();
}
}
}
// ---- Trmp2Mario_ResetAll
namespace ns_020cac9c {
extern "C" {
void Trmp2Mario_SetTuning(int sz);
void Trmp2Mario_EnterHidden(char *c);
void Trmp2Mario_UpdateMatrix(char *o);
void Puff_ClearAll(int a0, int a1);
extern "C" int sTrmp2MarioTarget;
extern "C" int sTrmp2Lives;
extern "C" int data_ov006_02140598;
extern "C" int sTrmpItemHitsOff;
extern "C" char *sTrmp2Marios;

// @symbol Trmp2Mario_ResetAll
void Trmp2Mario_ResetAll(void) {
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
            Trmp2Mario_EnterHidden(sTrmp2Marios + off);
            Trmp2Mario_UpdateMatrix(sTrmp2Marios + off);
            i++;
            off += 0xdc;
        } while (i < sTrmp2MarioTarget);
    }
    Puff_ClearAll(0, 0);
}
}
}
// ---- Trmp2Mario_SetTuning
namespace ns_020cad3c {
extern "C" {
extern int func_020531a4(int a);
extern "C" int sTrmp2FallSpeed;
extern "C" int sTrmp2GravityStep;
extern "C" int data_ov006_0214053c;
extern "C" int sTrmpSpeedScale;
extern "C" int sTrmp2RiseSpeedMax;
extern "C" int sTrmp2DriftScale;

// @symbol Trmp2Mario_SetTuning
void Trmp2Mario_SetTuning(int a) {
    int v;
    if (a > 0x2800)
        a = 0x2800;
    v = func_020531a4(a);
    sTrmp2FallSpeed = FX_MUL(v, -0x1c00);
    sTrmp2GravityStep = FX_MUL(a, 0xc0);
    data_ov006_0214053c = FX_MUL(v, 0xe00);
    sTrmpSpeedScale = FX_MUL(v, 0x5800);
    sTrmp2RiseSpeedMax = FX_MUL(v, 0x6000);
    sTrmp2DriftScale = FX_MUL(v, 0x180);
}
}
}
// ---- Trmp2Mario_InitPool
namespace ns_020cae9c {
extern "C" {
extern int Trmp2Mario_Load(int p);
extern "C" int sTrmp2MarioCount;
extern "C" int sTrmp2MarioTarget;
extern "C" int sTrmp2Marios;
extern "C" int sTrmpJumpers[];
extern "C" int sTrmp2MarioPtrs[];

// @symbol Trmp2Mario_InitPool
int Trmp2Mario_InitPool(int base, int n) {
    int i;
    int off;

    sTrmp2MarioCount = n;
    sTrmp2MarioTarget = n;
    sTrmp2Marios = base;

    i = 0;
    if (i < n) {
        off = i;
        do {
            int p;
            if (Trmp2Mario_Load(sTrmp2Marios + off) == 0)
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
}
}
// ---- Trmp3DMario_GetVelocity
namespace ns_020caf44 {
extern "C" {
// @symbol Trmp3DMario_GetVelocity
int Trmp3DMario_GetVelocity(int p) {
    return p + 52;
}
}
}
// ---- Trmp3DMario_IsMissed
namespace ns_020caf4c {
extern "C" {
extern "C" int pmf_Trmp3DMario_StateMissed[];
// @symbol Trmp3DMario_IsMissed
int Trmp3DMario_IsMissed(int *r0) {
    int *p = MARIO3D(r0)->mState;
    int *g = pmf_Trmp3DMario_StateMissed;
    int lr = 1;
    if (p[0] == g[0]) {
        if (p[1] == g[1] || MARIO3D(r0)->mState[0] == 0)
            lr = 0;
    }
    return lr;
}
}
}
// ---- Trmp3DMario_GetPrevPos
namespace ns_020cafa4 {
extern "C" {
// @symbol Trmp3DMario_GetPrevPos
int Trmp3DMario_GetPrevPos(int p) {
    return p + 40;
}
}
}
// ---- Trmp3DMario_GetPos
namespace ns_020cafac {
extern "C" {
// @symbol Trmp3DMario_GetPos
int Trmp3DMario_GetPos(int p) {
    return p + 28;
}
}
}
// ---- Trmp3DMario_Render
namespace ns_020cafb4 {
extern "C" {
extern int sTrmp3DMarioScale[];
// @symbol Trmp3DMario_Render
void Trmp3DMario_Render(char *self) {
    dExtAnmModel_c *model = &MARIO3D(self)->mAnmModel;
    model->Render((const Vector3 *)sTrmp3DMarioScale);
}
}
}
// ---- Trmp3DMario_UpdateMatrix
namespace ns_020cafdc {
/* recovered: shared common types */
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, short angY);
// @symbol Trmp3DMario_UpdateMatrix
void Trmp3DMario_UpdateMatrix(char *self) {
    Matrix4x3_FromTranslation(&data_020a0e68, MARIO3D(self)->mPos.x, MARIO3D(self)->mPos.y,
                              MARIO3D(self)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, MARIO3D(self)->mRotY);
    MARIO3D(self)->mAnmModel.mat4x3 = data_020a0e68;
}
}
}
// ---- Trmp3DMario_Update
namespace ns_020cb030 {
struct Vector3_16f;
struct Callback;
typedef int Fix12;
struct System {
    static System *New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f *, Callback *);
};

extern "C" {
/* The ROM's relocation at 0x020cb088 targets 0x020cb134, not 0x020c9024.
   The byte gate could not object: a bl is a relocation, which match.py
   compares as a wildcard, so either callee passes. */
extern void Trmp3DMario_ApplyGravity(char *o);
extern void Trmp3DMario_UpdateMatrix(char *o);
}
System *System::New(unsigned, unsigned, Fix12, Fix12, Fix12, const Vector3_16f *, Callback *);

typedef void (*PMF)(void *);
struct Closure {
    int off;
    int adj;
};

// @symbol Trmp3DMario_Update
extern "C" void Trmp3DMario_Update(char *o) {
    MARIO3D(o)->mPrevPos.x = MARIO3D(o)->mPos.x;
    MARIO3D(o)->mPrevPos.y = MARIO3D(o)->mPos.y;
    MARIO3D(o)->mPrevPos.z = MARIO3D(o)->mPos.z;
    if (MARIO3D(o)->mState[0] != 0) {
        Closure *cl = (Closure *)MARIO3D(o)->mState;
        void *tobj = o + (cl->adj >> 1);
        void (*fn)();
        if (cl->adj & 1)
            fn = *(void (**)())(*(char **)tobj + cl->off);
        else
            fn = (void (*)())cl->off;
        ((void (*)(void *))fn)(tobj);
    }
    Trmp3DMario_ApplyGravity(o);
    Fix12 v = FX_MUL(MARIO3D(o)->mBounceVy, 0xb4b);
    if (MARIO3D(o)->mVel.y > v) {
        if (MARIO3D(o)->mAnmModel.file == sTrmpAnimBounce) {
            MARIO3D(o)->mParticleId =
                (int)Particle::System::New(MARIO3D(o)->mParticleId, 0xf5, MARIO3D(o)->mPos.x << 3,
                                           MARIO3D(o)->mPos.y << 3, MARIO3D(o)->mPos.z << 3, 0, 0);
        }
    }
    Trmp3DMario_UpdateMatrix(o);
    MARIO3D(o)->mAnmModel.Advance();
}
}
// ---- Trmp3DMario_ApplyGravity
namespace ns_020cb134 {
extern "C" {
extern int sTrmp3DFallSpeed[];
void AddVec3(void *dst, void *a, void *b);
// @symbol Trmp3DMario_ApplyGravity
void Trmp3DMario_ApplyGravity(char *o) {
    ApproachLinear(MARIO3D(o)->mVel.y, sTrmp3DFallSpeed[0], MARIO3D(o)->mGravityStep);
    AddVec3(&MARIO3D(o)->mPos, &MARIO3D(o)->mVel, &MARIO3D(o)->mPos);
}
}
}
// ---- Trmp3DMario_StateKnockedOut
namespace ns_020cb16c {
extern "C" {
extern int Trmp3DMario_EnterOffstage(void *c);
// @symbol Trmp3DMario_StateKnockedOut
void Trmp3DMario_StateKnockedOut(int *c) {
    if (MARIO3D(c)->mPos.y >= -0x120000)
        return;
    MARIO3D(c)->mVel.y = 0;
    Trmp3DMario_EnterOffstage(c);
}
}
}
// ---- Trmp3DMario_EnterByKind
namespace ns_020cb1a8 {

extern "C" {
extern int pmf_Trmp3DMario_StateOffstage_4[2];
extern int pmf_Trmp3DMario_StateGoal_2[2];
extern int pmf_Trmp3DMario_StateKnockedOut[2];
}

// @symbol Trmp3DMario_EnterByKind
extern "C" void Trmp3DMario_EnterByKind(char *c) {
    int *p;
    int *d;

    p = MARIO3D(c)->mState;
    d = pmf_Trmp3DMario_StateOffstage_4;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (MARIO3D(c)->mState[0] != 0)
                goto check2;
        }
        return;
    }
check2:
    p = MARIO3D(c)->mState;
    d = pmf_Trmp3DMario_StateGoal_2;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (MARIO3D(c)->mState[0] != 0)
                goto body;
        }
        return;
    }
body:
    MARIO3D(c)->mGravityStep = 0x100;
    MARIO3D(c)->mVel.x = 0;
    MARIO3D(c)->mVel.y = 0x2000;
    Sound::PlayBank2_2D(0x1c9);
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    MARIO3D(c)->mAnmModel.currFrame = 0;
    {
        int a = pmf_Trmp3DMario_StateKnockedOut[0];
        int b = pmf_Trmp3DMario_StateKnockedOut[1];
        MARIO3D(c)->mState[0] = b ? a : a;
        MARIO3D(c)->mState[1] = b;
    }
}
}
// ---- Trmp3DMario_StateGoal
namespace ns_020cb2b4 {
extern "C" {
typedef struct {
    int m0, m4, m8;
} Triple;

extern "C" int sTrmpRowHeights[];
extern "C" int sTrmpComboCount;
extern "C" int sTrmpMariosInPlay;
extern "C" int sTrmpMariosOnLedges;
extern "C" char *sMgSubCamera;
extern "C" char *sMgMainCamera;
extern "C" int sTrmpGoalSlotCursor;
extern "C" Triple sTrmpGoalSlots[];
extern "C" Triple data_020a0ebc;

extern void func_02012718(int, int);
extern void Camera_ProjectToScreen(char *, void *, int *, int *);
extern int Mg_GetTopScreenYOffset(void);
extern void TrmpFx_Spawn(int, int);
extern void Trmp3DMario_EnterOffstage(char *);

// @symbol Trmp3DMario_StateGoal
void Trmp3DMario_StateGoal(void *self) {
    char *c = (char *)self;
    int va, vb;
    short idx;
    int lim;
    int u;
    int i;

    idx = MARIO3D(c)->mHeightRow;
    lim = sTrmpRowHeights[idx];
    if (MARIO3D(c)->mPos.y <= lim) {
        MARIO3D(c)->mPos.y = lim;
        MARIO3D(c)->mVel.y = 0;
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimRollJumpEnd) {
        UpdateAngle(MARIO3D(c)->mRotY, 0, 2, 0x1000);
        if (MARIO3D(c)->mAnmModel.Finished() == 0)
            return;
        MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
        return;
    }

    if (MARIO3D(c)->mPos.x > 0) {
        ApproachLinear(MARIO3D(c)->mVel.x, 0x4000, 0x800);
        ApproachLinear2(MARIO3D(c)->mRotY, 0x4000, 0x1000);
    } else {
        ApproachLinear(MARIO3D(c)->mVel.x, -0x4000, 0x800);
        ApproachLinear2(MARIO3D(c)->mRotY, -0x4000, 0x1000);
    }

    u = MARIO3D(c)->mPos.x;
    if (u <= 0x90000 && u >= -0x90000)
        return;

    func_02012718(0x1be, 0x100000);
    ApproachLinear(sTrmpComboCount, 0x270f, 1);
    sTrmpMariosInPlay -= 1;
    sTrmpMariosOnLedges -= 1;

    if (MARIO3D(c)->mPos.y > 0) {
        Camera_ProjectToScreen(sMgSubCamera, &MARIO3D(c)->mPos, &va, &vb);
        vb = vb - (Mg_GetTopScreenYOffset() + 0xc0);
    } else {
        Camera_ProjectToScreen(sMgMainCamera, &MARIO3D(c)->mPos, &va, &vb);
    }
    vb -= 0x20;
    TrmpFx_Spawn(vb << 12, (short)sTrmpComboCount);

    sTrmpGoalSlotCursor += 1;
    if (sTrmpGoalSlotCursor >= 6)
        sTrmpGoalSlotCursor = 0;

    i = sTrmpGoalSlotCursor;
    {
        Triple *p = &sTrmpGoalSlots[i];
        MARIO3D(c)->mPos.x = p->m0;
        MARIO3D(c)->mPos.y = p->m4;
        MARIO3D(c)->mPos.z = p->m8;
    }
    MARIO3D(c)->mVel.x = data_020a0ebc.m0;
    MARIO3D(c)->mVel.y = data_020a0ebc.m4;
    MARIO3D(c)->mVel.z = data_020a0ebc.m8;

    Trmp3DMario_EnterOffstage(c);
}
}
}
// ---- Trmp3DMario_EnterGoal
namespace ns_020cb528 {
typedef short s16;

extern "C" {
void Sound_PlayBank1Panned(int a0, char *a1, void *a2);
void Trmp3DMario_StateGoal(char *c);
extern int sTrmpRowHeights[];
extern int pmf_Trmp3DMario_StateGoal[];
extern int sTrmpMariosOnLedges[];
}
// @symbol Trmp3DMario_EnterGoal
extern "C" void Trmp3DMario_EnterGoal(char *c) {
    int a, b;
    MARIO3D(c)->mPos.y = sTrmpRowHeights[MARIO3D(c)->mHeightRow];
    MARIO3D(c)->mVel.x = 0;
    MARIO3D(c)->mVel.y = 0;
    Sound_PlayBank1Panned(0, (char *)0x10, (void *)MARIO3D(c)->mPos.x);
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimRollJumpEnd, 0x40000000, 0x800, 0);
    sTrmpMariosOnLedges[0] = sTrmpMariosOnLedges[0] + 1;
    a = pmf_Trmp3DMario_StateGoal[0];
    b = pmf_Trmp3DMario_StateGoal[1];
    MARIO3D(c)->mState[0] = b ? a : a;
    MARIO3D(c)->mState[1] = b;
    Trmp3DMario_StateGoal(c);
}
}
// ---- Trmp3DMario_StateLand
namespace ns_020cb5c4 {
extern "C" {
extern "C" int sTrmpRowHeights[];
extern "C" short sTrmpGoalRow;
extern void Trmp3DMario_EnterGoal(char *c);
extern void Trmp3DMario_EnterOnLedge(char *c);

// @symbol Trmp3DMario_StateLand
void Trmp3DMario_StateLand(char *c) {
    ApproachLinear(MARIO3D(c)->mPos.y, sTrmpRowHeights[MARIO3D(c)->mHeightRow], 0x4000);
    if (MARIO3D(c)->mPos.x > 0)
        ApproachLinear(MARIO3D(c)->mRotY, 0x4000, 0x1000);
    else
        ApproachLinear(MARIO3D(c)->mRotY, -0x4000, 0x1000);
    MARIO3D(c)->mVel.x = 0;
    MARIO3D(c)->mVel.y = 0;
    if (MARIO3D(c)->mAnmModel.file != sTrmpAnimHangUp)
        return;
    if (MARIO3D(c)->mAnmModel.Finished() == 0)
        return;
    if (MARIO3D(c)->mHeightRow == sTrmpGoalRow)
        Trmp3DMario_EnterGoal(c);
    else
        Trmp3DMario_EnterOnLedge(c);
}
}
}
// ---- Trmp3DMario_EnterLand
namespace ns_020cb690 {

extern "C" {
void Sound_PlayBank1Panned(int a0, char *a1, void *a2);
void Trmp3DMario_StateLand(char *c);
extern int data_020a0ebc[];
struct W2 {
    int a, b;
};
extern struct W2 pmf_Trmp3DMario_StateLand;
void Trmp3DMario_EnterLand(char *c);
}
// @symbol Trmp3DMario_EnterLand
void Trmp3DMario_EnterLand(char *c) {
    MARIO3D(c)->mVel.x = data_020a0ebc[0];
    MARIO3D(c)->mVel.y = data_020a0ebc[1];
    MARIO3D(c)->mVel.z = data_020a0ebc[2];
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimHangUp, 0x40000000, 0x800, 0);
    Sound_PlayBank1Panned(0, (char *)0x17, (void *)MARIO3D(c)->mPos.x);
    {
        int a = pmf_Trmp3DMario_StateLand.a;
        int b = pmf_Trmp3DMario_StateLand.b;
        MARIO3D(c)->mState[0] = b ? a : a;
        MARIO3D(c)->mState[1] = b;
    }
    int *p20 = &MARIO3D(c)->mPos.y;
    *p20 = *p20 + 0x20000;
    Trmp3DMario_StateLand(c);
}
}
// ---- Trmp3DMario_StateFallToLedge
namespace ns_020cb72c {
extern "C" {

extern void Trmp3DMario_EnterGoal(char *c);
extern void Trmp3DMario_EnterOnLedge(char *c);
extern int D3D_PlaySeAtX(int a, int b);
extern void Trmp3DMario_EnterOnGround(char *c);
extern "C" int sTrmpRowHeights[];
extern "C" short sTrmpGoalRow;
// @symbol Trmp3DMario_StateFallToLedge
void Trmp3DMario_StateFallToLedge(char *c) {
    int v = MARIO3D(c)->mPos.x;
    if (v > 0) {
        short idx = MARIO3D(c)->mHeightRow;
        int cur = MARIO3D(c)->mPos.y;
        int *d = sTrmpRowHeights;
        if (cur <= d[idx]) {
            if (idx == sTrmpGoalRow) {
                Trmp3DMario_EnterGoal(c);
            } else {
                Trmp3DMario_EnterOnLedge(c);
            }
            D3D_PlaySeAtX(0x1b4, MARIO3D(c)->mPos.x);
        } else {
            int lim = d[1] + 0x30000;
            if ((cur >= lim) && (idx == 1)) {
                MARIO3D(c)->mPos.y = lim;
            }
        }
    } else {
        if ((MARIO3D(c)->mPos.y) <= sTrmpRowHeights[0]) {
            D3D_PlaySeAtX(0x1b4, v);
            Trmp3DMario_EnterOnGround(c);
            return;
        }
    }
    {
        int w = MARIO3D(c)->mPos.x;
        if (w > 0x70000) {
            MARIO3D(c)->mPos.x = 0x70000;
            MARIO3D(c)->mVel.x = 0;
            return;
        }
        if (w < (-0x70000)) {
            MARIO3D(c)->mPos.x = -0x70000;
            MARIO3D(c)->mVel.x = 0;
        }
    }
}
}
}
// ---- Trmp3DMario_EnterFallToLedge
namespace ns_020cb814 {
extern "C" {
typedef struct {
    int a, b;
} Pair2;
extern "C" Pair2 pmf_Trmp3DMario_StateFallToLedge;
extern void Trmp3DMario_StateFallToLedge(char *c);

// @symbol Trmp3DMario_EnterFallToLedge
void Trmp3DMario_EnterFallToLedge(char *c) {
    *(W2 *)MARIO3D(c)->mState = *(W2 *)&pmf_Trmp3DMario_StateFallToLedge;
    Trmp3DMario_StateFallToLedge(c);
}
}
}
// ---- Trmp3DMario_StateHardBounce
namespace ns_020cb838 {

extern "C" {
void Trmp3DMario_EnterFall(void *c);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Trmp3DMario_EnterFallToLedge(void *c);
void Trmp3DMario_EnterLand(void *c);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void func_02012718(int a, int b);
int D3D_PlaySeAtX(int a, int b);
}

extern "C" void *sMgMainCamera;
extern "C" int sTrmpRowHeights[2];

// @symbol Trmp3DMario_StateHardBounce
extern "C" void Trmp3DMario_StateHardBounce(char *c) {
    int v;

    if (MARIO3D(c)->mVel.y <= 0) {
        Trmp3DMario_EnterFall(c);
        return;
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimBounce) {
        if (MARIO3D(c)->mAnmModel.WillHitFrame(0xc) || MARIO3D(c)->mAnmModel.WillHitFrame(0x18)) {
            D3D_PlaySeAtX(0x1b5, MARIO3D(c)->mPos.x);
        }
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO3D(c)->mPos, MARIO3D(c)->mScreenPos);

    v = MARIO3D(c)->mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (MARIO3D(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L1ac;
    if (MARIO3D(c)->mVel.x <= 0)
        goto L1ac;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = MARIO3D(c)->mPos.y;

    if (p > f0) {
        MARIO3D(c)->mHeightRow = 0;
        Trmp3DMario_EnterFallToLedge(c);
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L134;
    MARIO3D(c)->mHeightRow = 0;
    Trmp3DMario_EnterLand(c);
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
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterFallToLedge(c);
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L134;
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterLand(c);
        return;
    }
}

L134:
    if (MARIO3D(c)->mVel.y > 0) {
        MARIO3D(c)->mAnim = 2;
        MARIO3D(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO3D(c)->mAnim], 0x40000000, 0x800,
                                      0);

        MARIO3D(c)->mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, MARIO3D(c)->mAnim, MARIO3D(c)->mPos.x);
    }

    func_02012718(0x1b3, MARIO3D(c)->mPos.x + 0x80000);
    MARIO3D(c)->mVel.x *= -1;
    goto L1dc;

L1ac:
    if (MARIO3D(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO3D(c)->mVel.x *= -1;
    }

L1dc: {
    int r = MARIO3D(c)->mVel.x;
    if (r > 0x4000) {
        ApproachLinear(MARIO3D(c)->mRotY, 0x6000, 0xc00);
    } else if (r < -0x4000) {
        ApproachLinear(MARIO3D(c)->mRotY, -0x6000, 0xc00);
    } else {
        ApproachLinear(MARIO3D(c)->mRotY, (short)r, 0xc00);
    }
}
}
}
// ---- Trmp3DMario_BounceOffLineA
namespace ns_020cbaec {
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
typedef short s16;

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void *d_mg_trmpln2_marioNewHold2(unsigned int a, unsigned int b, int c, int d, int e, const void *f,
                                 void *g);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
int D3D_PlaySeAtX(int a, int b);
void Trmp3DMario_StateHardBounce(char *c);
}

// @symbol Trmp3DMario_BounceOffLineA
extern "C" void Trmp3DMario_BounceOffLineA(char *c) {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp4;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;

    alongLine = MARIO3D(c)->mContactOffset;
    lineHalfLength = MARIO3D(c)->mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;
    {
        int *src = (int *)(c + 4);
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3(&MARIO3D(c)->mVel, &tmp);
    scale = FX_MUL(dot, 0x1400);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3(&MARIO3D(c)->mVel, &tmp2, &MARIO3D(c)->mVel);

    fdivr = cstd::fdiv(lineHalfLength, lineHalfLength + alongLine);

    MARIO3D(c)->mGravityStep = sTrmp3DGravityStep;

    {
        Vector3_16 v;
        v.x = (s16)tmp.x;
        v.y = (s16)tmp.y;
        v.z = (s16)tmp.z;

        Particle::System::New(0, 0xf3, MARIO3D(c)->mPos.x << 3, MARIO3D(c)->mPos.y << 3,
                              MARIO3D(c)->mPos.z << 3, (const Vector3_16f *)(&v), 0);
        Particle::System::New(0, 0xf4, MARIO3D(c)->mPos.x << 3, MARIO3D(c)->mPos.y << 3,
                              MARIO3D(c)->mPos.z << 3, (const Vector3_16f *)(&v), 0);
    }

    MARIO3D(c)->mParticleId =
        (int)Particle::System::New(MARIO3D(c)->mParticleId, 0xf5, MARIO3D(c)->mPos.x << 3,
                                   MARIO3D(c)->mPos.y << 3, MARIO3D(c)->mPos.z << 3, 0, 0);

    scale = FX_MUL(sTrmp3DHardBounceScale, fdivr);
    Vec3_MulScalar(&tmp4, &tmp, scale);
    AddVec3(&MARIO3D(c)->mVel, &tmp4, &MARIO3D(c)->mVel);

    {
        int v = MARIO3D(c)->mVel.x;
        if (v < -0x1000)
            v = -0x1000;
        else if (v > 0x1000)
            v = 0x1000;
        MARIO3D(c)->mVel.x = v;
    }

    {
        int half = sTrmp3DHardBounceScale >> 1;
        int cur = MARIO3D(c)->mVel.y;
        if (cur >= half)
            half = cur;
        MARIO3D(c)->mVel.y = half;
    }

    MARIO3D(c)->mBounceVy = MARIO3D(c)->mVel.y;
    ((dMgTrmpObjAdapter_c *)c)->Clear();

    if (MARIO3D(c)->mAnim != 0) {
        MARIO3D(c)->mAnim = 0;
    } else {
        MARIO3D(c)->mAnim = 1;
    }

    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimBounce, 0x40000000, 0x800, 0);

    MARIO3D(c)->mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, 4, MARIO3D(c)->mPos.x);

    D3D_PlaySeAtX(0x1b5, MARIO3D(c)->mPos.x);

    *(int *)(c + 0x5c) = 0; /* stays raw: only ever stored 0, no reader */
    {
        int a = pmf_Trmp3DMario_StateHardBounce[0];
        int b = pmf_Trmp3DMario_StateHardBounce[1];
        a = b ? a : a;
        MARIO3D(c)->mState[0] = a;
        MARIO3D(c)->mState[1] = b;
    }
    Trmp3DMario_StateHardBounce(c);
}
}
// ---- Trmp3DMario_StateBounce
namespace ns_020cbd7c {

extern "C" {
void Trmp3DMario_EnterFall(void *c);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Trmp3DMario_EnterFallToLedge(void *c);
void Trmp3DMario_EnterLand(void *c);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void func_02012718(int a, int b);
}

extern "C" void *sMgMainCamera;
extern "C" int sTrmpRowHeights[2];

// @symbol Trmp3DMario_StateBounce
extern "C" void Trmp3DMario_StateBounce(char *c) {
    int v;

    if (MARIO3D(c)->mVel.y <= 0) {
        Trmp3DMario_EnterFall(c);
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO3D(c)->mPos, MARIO3D(c)->mScreenPos);

    v = MARIO3D(c)->mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (MARIO3D(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L1ac;
    if (MARIO3D(c)->mVel.x <= 0)
        goto L1ac;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = MARIO3D(c)->mPos.y;

    if (p > f0) {
        MARIO3D(c)->mHeightRow = 0;
        Trmp3DMario_EnterFallToLedge(c);
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L134;
    MARIO3D(c)->mHeightRow = 0;
    Trmp3DMario_EnterLand(c);
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
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterFallToLedge(c);
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L134;
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterLand(c);
        return;
    }
}

L134:
    if (MARIO3D(c)->mVel.y > 0) {
        MARIO3D(c)->mAnim = 2;
        MARIO3D(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO3D(c)->mAnim], 0x40000000, 0x800,
                                      0);

        MARIO3D(c)->mAnmModel.currFrame = 0;
        Sound_PlayBank1Panned(0, MARIO3D(c)->mAnim, MARIO3D(c)->mPos.x);
    }

    func_02012718(0x1b3, MARIO3D(c)->mPos.x + 0x80000);
    MARIO3D(c)->mVel.x *= -1;
    goto L1dc;

L1ac:
    if (MARIO3D(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO3D(c)->mVel.x *= -1;
    }

L1dc: {
    int r = MARIO3D(c)->mVel.x;
    if (r > 0x4000) {
        ApproachLinear(MARIO3D(c)->mRotY, 0x4000, 0xc00);
    } else if (r < -0x4000) {
        ApproachLinear(MARIO3D(c)->mRotY, -0x4000, 0xc00);
    } else {
        ApproachLinear(MARIO3D(c)->mRotY, (short)r, 0xc00);
    }
}
}
}
// ---- Trmp3DMario_BounceOffLineB
namespace ns_020cbfd8 {
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void Trmp3DMario_StateBounce(char *c);
}

// @symbol Trmp3DMario_BounceOffLineB
extern "C" void Trmp3DMario_BounceOffLineB(char *c) {
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp3;
    int alongLine;
    int lineHalfLength;
    int dot;
    int scale;
    int fdivr;
    int vx;

    alongLine = MARIO3D(c)->mContactOffset;
    lineHalfLength = MARIO3D(c)->mContactHalfWidth;
    if (alongLine < 0)
        alongLine = -alongLine;

    {
        int *src = (int *)(c + 4);
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3(&MARIO3D(c)->mVel, &tmp);
    scale = FX_MUL(dot, 0x1200);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3(&MARIO3D(c)->mVel, &tmp2, &MARIO3D(c)->mVel);

    fdivr = cstd::fdiv(lineHalfLength, lineHalfLength + alongLine);
    MARIO3D(c)->mGravityStep = sTrmp3DGravityStep;

    scale = FX_MUL(sTrmp3DBounceScale, fdivr);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    AddVec3(&MARIO3D(c)->mVel, &tmp3, &MARIO3D(c)->mVel);

    vx = MARIO3D(c)->mVel.x;
    if (vx < -0x1000)
        vx = -0x1000;
    else if (vx > 0x1000)
        vx = 0x1000;
    MARIO3D(c)->mVel.x = vx;

    {
        int half = sTrmp3DBounceScale >> 2;
        int cur = MARIO3D(c)->mVel.y;
        if (cur >= half)
            half = cur;
        MARIO3D(c)->mVel.y = half;
    }

    ((dMgTrmpObjAdapter_c *)c)->Clear();

    if (MARIO3D(c)->mAnim != 0) {
        MARIO3D(c)->mAnim = 0;
    } else {
        MARIO3D(c)->mAnim = 1;
    }

    MARIO3D(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO3D(c)->mAnim], 0x40000000, 0x800, 0);

    MARIO3D(c)->mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, MARIO3D(c)->mAnim, MARIO3D(c)->mPos.x);

    {
        int a = pmf_Trmp3DMario_StateBounce[0];
        int b = pmf_Trmp3DMario_StateBounce[1];
        a = b ? a : a;
        MARIO3D(c)->mState[0] = a;
        MARIO3D(c)->mState[1] = b;
    }

    Trmp3DMario_StateBounce(c);
}
}
// ---- Trmp3DMario_JumpOff
namespace ns_020cc198 {
extern "C" {
int RandomIntInternal(int *seed);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
void Trmp3DMario_StateBounce(char *c);
}

extern "C" int sTrmp3DGravityStep;
extern "C" int data_0209e650;
extern "C" int sTrmp3DJumpOffSpeed;
extern "C" int sTrmp3DBounceScale;
extern "C" int pmf_Trmp3DMario_StateBounce_2[2];

// @symbol Trmp3DMario_JumpOff
extern "C" void Trmp3DMario_JumpOff(char *c) {
    int *p38;
    int b;

    MARIO3D(c)->mGravityStep = sTrmp3DGravityStep;
    MARIO3D(c)->mVel.x =
        sTrmp3DJumpOffSpeed +
        (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19);

    b = MARIO3D(c)->mPos.x;
    if (b > 0)
        MARIO3D(c)->mVel.x = -MARIO3D(c)->mVel.x;

    p38 = &MARIO3D(c)->mVel.y;
    *p38 += (int)((((long long)sTrmp3DBounceScale << 11) + 0x800) >> 12);

    ((dMgTrmpObjAdapter_c *)c)->Clear();

    MARIO3D(c)->mAnim = 0;

    MARIO3D(c)->mAnmModel.SetAnim(*sTrmpBounceAnimTable[MARIO3D(c)->mAnim], 0x40000000, 0x800, 0);

    MARIO3D(c)->mAnmModel.currFrame = 0;
    Sound_PlayBank1Panned(0, MARIO3D(c)->mAnim, MARIO3D(c)->mPos.x);

    {
        int a = pmf_Trmp3DMario_StateBounce_2[0];
        int b2 = pmf_Trmp3DMario_StateBounce_2[1];
        a = b2 ? a : a;
        MARIO3D(c)->mState[0] = a;
        MARIO3D(c)->mState[1] = b2;
    }

    Trmp3DMario_StateBounce(c);
}
}
// ---- Trmp3DMario_StateMissed
namespace ns_020cc2ac {
extern "C" {
typedef short s16;
extern void Puff_Spawn(int a, int b);
extern void Trmp3DMario_EnterOffstage(char *c);
extern "C" int sTrmpMariosInPlay;
extern "C" int sTrmp3DLives;

// @symbol Trmp3DMario_StateMissed
void Trmp3DMario_StateMissed(char *c) {
    s16 *p = &MARIO3D(c)->mTimer;
    *p = *p - 1;
    if (MARIO3D(c)->mTimer == 0) {
        Puff_Spawn(MARIO3D(c)->mScreenPos[0], 0xc0);
        sTrmpMariosInPlay -= 1;
        ApproachLinear(sTrmp3DLives, 0, 1);
        Sound::PlayBank2_2D(0x130);
        Trmp3DMario_EnterOffstage(c);
        return;
    }
    MARIO3D(c)->mVel.x = FX_MUL(MARIO3D(c)->mVel.x, 0xc00);
    {
        int v = MARIO3D(c)->mPos.x;
        if (v < -0x6c000) {
            MARIO3D(c)->mPos.x = -0x6c000;
            return;
        }
        if (v > 0x6c000)
            MARIO3D(c)->mPos.x = 0x6c000;
    }
}
}
}
// ---- Trmp3DMario_EnterMissed
namespace ns_020cc37c {
extern "C" {
extern void D3D_PlaySeAtX(int a, int b);
extern int sTrmp3DLives[];
extern double pmf_Trmp3DMario_StateMissed_2;
void Trmp3DMario_EnterMissed(char *c);
// @symbol Trmp3DMario_EnterMissed
void Trmp3DMario_EnterMissed(char *c) {
    if (sTrmp3DLives[0] > 1)
        D3D_PlaySeAtX(0x1ca, MARIO3D(c)->mPos.x);
    else
        Sound::PlayBank2_2D(0x1c9);
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimStruggle, 0, 0x800, 0);
    MARIO3D(c)->mTimer = 0x28;
    *(double *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateMissed_2;
}
}
}
// ---- Trmp3DMario_StateFall
namespace ns_020cc408 {

extern "C" {
void Trmp3DMario_BounceOffLineB(void *c);
void Trmp3DMario_BounceOffLineA(void *c);
void Camera_ProjectToScreen16(void *a0, void *a1, short *a2);
void Trmp3DMario_EnterFallToLedge(void *c);
void Trmp3DMario_EnterLand(void *c);
int D3D_PlaySeAtX(int a, int b);
void Trmp3DMario_EnterMissed(void *c);
}

extern "C" void *sMgMainCamera;
extern "C" int sTrmpRowHeights[2];

// @symbol Trmp3DMario_StateFall
extern "C" void Trmp3DMario_StateFall(char *c) {
    int v;
    u16 state = MARIO3D(c)->mContact;

    if (state == 1) {
        Trmp3DMario_BounceOffLineB(c);
        return;
    }
    if (state == 2) {
        Trmp3DMario_BounceOffLineA(c);
        return;
    }

    Camera_ProjectToScreen16(sMgMainCamera, &MARIO3D(c)->mPos, MARIO3D(c)->mScreenPos);

    v = MARIO3D(c)->mPos.x;

    if (v >= -0x5c000)
        goto L5c;
    if (MARIO3D(c)->mVel.x < 0)
        goto L70;

L5c:
    if (v <= 0x5c000)
        goto L150;
    if (MARIO3D(c)->mVel.x <= 0)
        goto L150;

L70: {
    int f0 = sTrmpRowHeights[0];
    int p = MARIO3D(c)->mPos.y;

    if (p > f0) {
        MARIO3D(c)->mHeightRow = 0;
        Trmp3DMario_EnterFallToLedge(c);
        return;
    }

    f0 -= 0x20000;
    if (p <= f0)
        goto Ld0;
    if (v <= 0)
        goto L130;
    MARIO3D(c)->mHeightRow = 0;
    Trmp3DMario_EnterLand(c);
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
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterFallToLedge(c);
        return;

    L10c:
        f1 -= 0x20000;
        if (p <= f1)
            goto L130;
        MARIO3D(c)->mHeightRow = 1;
        Trmp3DMario_EnterLand(c);
        return;
    }
}

L130:
    D3D_PlaySeAtX(0x1b3, v);
    MARIO3D(c)->mVel.x *= -1;
    goto L180;

L150:
    if (MARIO3D(c)->mContact == 3) {
        ((dMgTrmpObjAdapter_c *)c)->Clear();
        MARIO3D(c)->mVel.x *= -1;
    }

L180: {
    int r = MARIO3D(c)->mVel.x;
    if (r > 0x4000) {
        ApproachLinear(MARIO3D(c)->mRotY, 0x4000, 0xc00);
    } else if (r < -0x8000) {
        ApproachLinear(MARIO3D(c)->mRotY, -0x4000, 0xc00);
    } else {
        ApproachLinear(MARIO3D(c)->mRotY, (short)r, 0xc00);
    }
}

    if (MARIO3D(c)->mPos.y >= -0xc8000)
        return;

    Trmp3DMario_EnterMissed(c);
}
}
// ---- Trmp3DMario_EnterFall
namespace ns_020cc618 {
extern "C" {
typedef struct {
    u32 a, b;
} Pair;

extern "C" Pair pmf_Trmp3DMario_StateFall;
extern void Trmp3DMario_StateFall(void *self);

// @symbol Trmp3DMario_EnterFall
void Trmp3DMario_EnterFall(void *self) {
    *(W2 *)MARIO3D(self)->mState = *(W2 *)&pmf_Trmp3DMario_StateFall;
    Trmp3DMario_StateFall(self);
}
}
}
// ---- Trmp3DMario_StateWalkBack
namespace ns_020cc63c {
extern "C" {
extern "C" int sTrmpRowHeights[];
extern "C" int sTrmpMariosOnLedges;
extern void Trmp3DMario_JumpOff(int *c);
// @symbol Trmp3DMario_StateWalkBack
void Trmp3DMario_StateWalkBack(int *c) {
    short idx = MARIO3D(c)->mHeightRow;
    MARIO3D(c)->mPos.y = sTrmpRowHeights[idx];
    if (MARIO3D(c)->mPos.x >= 0x5bff8)
        return;
    sTrmpMariosOnLedges--;
    Trmp3DMario_JumpOff(c);
}
}
}
// ---- Trmp3DMario_EnterWalkBack
namespace ns_020cc698 {
extern "C" {
extern int sTrmpRowHeights[];
extern double pmf_Trmp3DMario_StateWalkBack;
void Trmp3DMario_EnterWalkBack(char *c);
// @symbol Trmp3DMario_EnterWalkBack
void Trmp3DMario_EnterWalkBack(char *c) {
    MARIO3D(c)->mGravityStep = 0;
    MARIO3D(c)->mPos.y = sTrmpRowHeights[MARIO3D(c)->mHeightRow];
    MARIO3D(c)->mPos.z = 0;
    MARIO3D(c)->mVel.x = -0x1800;
    MARIO3D(c)->mVel.y = 0;
    MARIO3D(c)->mVel.z = 0;
    MARIO3D(c)->mRotY = -0x4000;
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(double *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateWalkBack;
}
}
}
// ---- Trmp3DMario_StateOnLedge
namespace ns_020cc724 {
extern "C" {
extern void Trmp3DMario_EnterWalkBack(char *c);

extern "C" void *sTrmpRowHeights[];

// @symbol Trmp3DMario_StateOnLedge
void Trmp3DMario_StateOnLedge(char *c) {
    MARIO3D(c)->mPos.y = (int)sTrmpRowHeights[MARIO3D(c)->mHeightRow];

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimJumpEnd) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0) {
            MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
            return;
        }
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimWalk) {
        if (MARIO3D(c)->mVel.x > 0) {
            if (MARIO3D(c)->mPos.x > 0x70000) {
                MARIO3D(c)->mPos.x = 0x70000;
                MARIO3D(c)->mVel.x = 0;
                MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimTurn, 0x40000000, 0x800, 0);
            }
            return;
        }
        if (ApproachLinear(MARIO3D(c)->mRotY, 0x4000, 0x800) != 0)
            MARIO3D(c)->mVel.x = 0x1000;
        return;
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimTurn) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0) {
            MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimTurnEnd, 0x40000000, 0x800, 0);
            return;
        }
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimTurnEnd) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0)
            Trmp3DMario_EnterWalkBack(c);
    }
}
}
}
// ---- Trmp3DMario_EnterOnLedge
namespace ns_020cc8c8 {
extern "C" {

typedef short s16;
typedef struct {
    int x, y;
} G2;
extern "C" int pmf_Trmp3DMario_StateFallToLedge_2[2];
extern "C" G2 pmf_Trmp3DMario_StateOnLedge;
extern "C" void *sTrmpRowHeights[];
extern "C" int sTrmpMariosOnLedges;

// @symbol Trmp3DMario_EnterOnLedge
void Trmp3DMario_EnterOnLedge(char *p) {
    int *cur;
    int *want;
    int z = 0;
    MARIO3D(p)->mGravityStep = z;
    MARIO3D(p)->mPos.y = (int)sTrmpRowHeights[MARIO3D(p)->mHeightRow];
    MARIO3D(p)->mPos.z = z;
    MARIO3D(p)->mVel.x = z;
    MARIO3D(p)->mVel.y = z;
    MARIO3D(p)->mVel.z = z;
    cur = MARIO3D(p)->mState;
    want = pmf_Trmp3DMario_StateFallToLedge_2;
    if (cur[0] == want[0] && (cur[1] == want[1] || MARIO3D(p)->mState[0] == 0)) {
        MARIO3D(p)->mAnmModel.SetAnim(sTrmpAnimJumpEnd, 0x40000000, 0x800, 0);
    } else {
        MARIO3D(p)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    }
    {
        int *ctr = &sTrmpMariosOnLedges;
        int c = *ctr;
        *ctr = c + 1;
        *(W2 *)MARIO3D(p)->mState = *(W2 *)&pmf_Trmp3DMario_StateOnLedge;
    }
}
}
}
// ---- Trmp3DMario_StateWalkIn
namespace ns_020cc9b8 {
extern "C" {
extern "C" int sTrmpRowHeights;
extern void Trmp3DMario_JumpOff(int *c);
// @symbol Trmp3DMario_StateWalkIn
void Trmp3DMario_StateWalkIn(int *c) {
    MARIO3D(c)->mPos.y = sTrmpRowHeights;
    if (MARIO3D(c)->mPos.x <= -0x5c008)
        return;
    Trmp3DMario_JumpOff(c);
}
}
}
#pragma push
#pragma optimize_for_size on
// ---- Trmp3DMario_EnterWalkIn
namespace ns_020cc9fc {
extern "C" {}
struct G2 {
    int w[2];
};
extern "C" int sTrmpRowHeights;
extern "C" struct G2 pmf_Trmp3DMario_StateOnGround;
extern "C" int sTrmpMariosInPlay;
extern "C" struct G2 pmf_Trmp3DMario_StateWalkIn;

// @symbol Trmp3DMario_EnterWalkIn
extern "C" void Trmp3DMario_EnterWalkIn(char *c) {
    MARIO3D(c)->mGravityStep = 0;
    MARIO3D(c)->mPos.y = sTrmpRowHeights;
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
    struct G2 *d = &pmf_Trmp3DMario_StateOnGround;
    if (s[0] != d->w[0] || (s[1] != d->w[1] && MARIO3D(c)->mState[0] != 0)) {
        sTrmpMariosInPlay += 1;
    }
    *(struct G2 *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateWalkIn;
}
}
#pragma pop
// ---- Trmp3DMario_StateOnGround
namespace ns_020ccae0 {
extern "C" {
extern void Trmp3DMario_EnterWalkIn(char *c);

extern "C" void *sTrmpRowHeights;

// @symbol Trmp3DMario_StateOnGround
void Trmp3DMario_StateOnGround(char *c) {
    MARIO3D(c)->mPos.y = (int)sTrmpRowHeights;

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimJumpEnd) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0) {
            MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
            return;
        }
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimWalk) {
        if (MARIO3D(c)->mVel.x < 0) {
            if (MARIO3D(c)->mPos.x < -0x70000) {
                MARIO3D(c)->mPos.x = -0x70000;
                MARIO3D(c)->mVel.x = 0;
                MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimTurn, 0x40000000, 0x800, 0);
            }
            return;
        }
        if (ApproachLinear(MARIO3D(c)->mRotY, -0x4000, 0x800) != 0)
            MARIO3D(c)->mVel.x = -0x1000;
        return;
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimTurn) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0) {
            MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimTurnEnd, 0x40000000, 0x800, 0);
            return;
        }
    }

    if (MARIO3D(c)->mAnmModel.file == sTrmpAnimTurnEnd) {
        if (MARIO3D(c)->mAnmModel.Finished() != 0)
            Trmp3DMario_EnterWalkIn(c);
    }
}
}
}
// ---- Trmp3DMario_EnterOnGround
namespace ns_020ccc8c {
extern "C" {
extern int sTrmpRowHeights;
extern double pmf_Trmp3DMario_StateOnGround_2;
}
// @symbol Trmp3DMario_EnterOnGround
extern "C" void Trmp3DMario_EnterOnGround(char *c) {
    MARIO3D(c)->mGravityStep = 0;
    MARIO3D(c)->mPos.y = sTrmpRowHeights;
    MARIO3D(c)->mPos.z = 0;
    MARIO3D(c)->mVel.x = 0;
    MARIO3D(c)->mVel.y = 0;
    MARIO3D(c)->mVel.z = 0;
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimJumpEnd, 0x40000000, 0x800, 0);
    *(double *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateOnGround_2;
}
}
// ---- Trmp3DMario_IsOffstage
namespace ns_020ccd04 {
extern "C" {
extern "C" int pmf_Trmp3DMario_StateOffstage_3[];
// @symbol Trmp3DMario_IsOffstage
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
}
}
// ---- Trmp3DMario_StateOffstage
namespace ns_020ccd64 {
extern "C" {
struct S {
    int w[1];
};
extern "C" struct S sTrmpRowHeights;
// @symbol Trmp3DMario_StateOffstage
void Trmp3DMario_StateOffstage(char *p) {
    MARIO3D(p)->mPos.y = sTrmpRowHeights.w[0];
}
}
}
// ---- Trmp3DMario_EnterOffstage
namespace ns_020ccd78 {
extern "C" {
extern int sTrmpRowHeights;
struct V3 {
    int a, b, c;
};
extern struct V3 data_020a0ebc;
struct G2 {
    int w[2];
};
extern struct G2 pmf_Trmp3DMario_StateOffstage_2;
// @symbol Trmp3DMario_EnterOffstage
void Trmp3DMario_EnterOffstage(char *c) {
    MARIO3D(c)->mPos.y = sTrmpRowHeights;
    MARIO3D(c)->mPos.x = -0x90000;
    MARIO3D(c)->mPos.z = 0;
    MARIO3D(c)->mVel.x = data_020a0ebc.a;
    MARIO3D(c)->mVel.y = data_020a0ebc.b;
    MARIO3D(c)->mVel.z = data_020a0ebc.c;
    MARIO3D(c)->mGravityStep = 0;
    MARIO3D(c)->mAnmModel.SetAnim(sTrmpAnimWalk, 0, 0x800, 0);
    *(struct G2 *)MARIO3D(c)->mState = pmf_Trmp3DMario_StateOffstage_2;
}
}
}
// ---- Trmp3DMario_Load
namespace ns_020cce0c {
extern "C" {
typedef struct BMD_File BMD_File;

extern "C" SharedFilePtr sTrmpMarioModelFile;
extern "C" SharedFilePtr sTrmpMarioJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioDoubleJump1AnimFile;
extern "C" SharedFilePtr sTrmpMarioWallJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioHangUpAnimFile;
extern "C" SharedFilePtr sTrmpMarioRollJumpEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioWalkAnimFile;
extern "C" SharedFilePtr sTrmpMarioDoubleJump2AnimFile;
extern "C" SharedFilePtr sTrmpMarioJumpEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioTurnAnimFile;
extern "C" SharedFilePtr sTrmpMarioTurnEndAnimFile;
extern "C" SharedFilePtr sTrmpMarioRollJumpAnimFile;
extern "C" SharedFilePtr sTrmpMarioStruggleAnimFile;

extern "C" char *sMgMainCamera;

extern void Camera_ProjectToScreen16(char *p, void *q, short *s);
extern void func_02016a14(void *self, int a, int b);
extern void Trmp3DMario_EnterOffstage(char *c);

// @symbol Trmp3DMario_Load
int Trmp3DMario_Load(char *c) {
    BMD_File *f;
    int ret;

    f = (BMD_File *)dExtSimpleModel_c::LoadFile(sTrmpMarioModelFile);
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
    ret = MARIO3D(c)->mAnmModel.SetFile(f, 1, -1);
    MARIO3D(c)->mAnim = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &MARIO3D(c)->mPos, MARIO3D(c)->mScreenPos);
    MARIO3D(c)->mGravityStep = 0;
    func_02016a14(&MARIO3D(c)->mAnmModel, 0x6bff, 0);
    func_02016a04(&MARIO3D(c)->mAnmModel, 0x21d);
    Trmp3DMario_EnterOffstage(c);
    return ret;
}
}
}
// ---- Trmp3DMario_Destruct
namespace ns_020ccfc8 {
extern "C" SharedFilePtr sTrmpMarioModelFile, sTrmpMarioJumpAnimFile, sTrmpMarioDoubleJump1AnimFile,
    sTrmpMarioWallJumpAnimFile, sTrmpMarioHangUpAnimFile, sTrmpMarioRollJumpEndAnimFile,
    sTrmpMarioWalkAnimFile, sTrmpMarioDoubleJump2AnimFile, sTrmpMarioJumpEndAnimFile,
    sTrmpMarioTurnAnimFile, sTrmpMarioTurnEndAnimFile, sTrmpMarioRollJumpAnimFile,
    sTrmpMarioStruggleAnimFile;
}

/* mAnmModel's destructor, then the base adapter's (storing its vptr back),
   run after the body. */
// @symbol _ZN18dMgTrmpln3DMario_cD1Ev
dMgTrmpln3DMario_c::~dMgTrmpln3DMario_c() {
    using namespace ns_020ccfc8;
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
// ---- Trmp3DMario_Construct
/* The base adapter and mAnmModel are constructed before the (empty) body. */
// @symbol _ZN18dMgTrmpln3DMario_cC1Ev
dMgTrmpln3DMario_c::dMgTrmpln3DMario_c() {}
// ---- Trmp3DMario_CountFinished
namespace ns_020cd158 {
extern "C" {
extern "C" int sTrmp3DMarioCount;
extern "C" char *sTrmp3DMarios;
extern "C" int pmf_Trmp3DMario_StateOffstage[];

// @symbol Trmp3DMario_CountFinished
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
                if (p[1] == *(volatile int *)(b + 1))
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
}
}
// ---- Trmp3DMario_StartAll
namespace ns_020cd1e0 {
extern "C" {
extern void Trmp3DMario_EnterByKind(void *p);
extern void Trmp3DMario_UpdateMatrix(char *o);
extern "C" int sTrmp3DMarioCount[];
extern "C" int sTrmp3DFallSpeed[];
extern "C" int sTrmp3DGravityStep[];
extern "C" char *sTrmp3DMarios[];

// @symbol Trmp3DMario_StartAll
void Trmp3DMario_StartAll(void) {
    int i;
    int off;
    i = 0;
    sTrmp3DGravityStep[0] = 0x100;
    sTrmp3DFallSpeed[0] = -0x4800;
    if (sTrmp3DMarioCount[0] <= 0)
        return;
    off = 0;
    do {
        Trmp3DMario_EnterByKind(sTrmp3DMarios[0] + off);
        Trmp3DMario_UpdateMatrix(sTrmp3DMarios[0] + off);
        i++;
        off += 0xd0;
    } while (i < sTrmp3DMarioCount[0]);
}
}
}
#pragma push
#pragma opt_common_subs off
// ---- Trmp3DMario_DrawAll
namespace ns_020cd270 {
extern "C" {

extern void func_0203cd80(int *v, int a);
extern void func_0203ccd4(int *v, int a);
extern "C" int sTrmp3DMarioCount;
extern "C" char *sTrmp3DMarios;
extern void Trmp3DMario_Render(char *p);
extern void Puff_DrawAll(void);

// @symbol Trmp3DMario_DrawAll
void Trmp3DMario_DrawAll(void) {
    int v[3];
    int w[3];

    v[0] = 0;
    v[1] = 0;
    v[2] = 0xfffff008;
    func_0203cd80(v, 0x2000);
    func_0203ccd4(v, 0);
    *(volatile int *)0x040004c8 = (((short)v[0] >> 3) & 0x3ff) |
                                  ((((short)v[1] >> 3) & 0x3ff) << 10) |
                                  ((((short)v[2] >> 3) & 0x3ff) << 20);

    w[0] = 0;
    w[1] = 0;
    w[2] = 0xfffff008;
    func_0203cd80(w, -0x2000);
    *(volatile int *)0x040004c8 = (((short)w[0] >> 3) & 0x3ff) |
                                  ((((short)w[1] >> 3) & 0x3ff) << 10) |
                                  ((((short)w[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    {
        int i;
        for (i = 0; i < sTrmp3DMarioCount; i++)
            Trmp3DMario_Render(sTrmp3DMarios + i * 0xd0);
    }

    Puff_DrawAll();
}
}
}
#pragma pop
// ---- Trmp3DMario_UpdateAll
namespace ns_020cd39c {
extern "C" {
extern void Trmp3DMario_Update(char *p);
extern void Puff_UpdateAll(void);
extern "C" int sTrmp3DMarioCount;
extern "C" char *sTrmp3DMarios;
extern "C" int sTrmpMariosOnLedges;
extern "C" int sTrmpAnyOnLedge;

// @symbol Trmp3DMario_UpdateAll
void Trmp3DMario_UpdateAll(void) {
    int i;
    for (i = 0; i < sTrmp3DMarioCount; i++)
        Trmp3DMario_Update(sTrmp3DMarios + i * 0xd0);
    if (sTrmpMariosOnLedges != 0)
        sTrmpAnyOnLedge = 1;
    else
        sTrmpAnyOnLedge = 0;
    Puff_UpdateAll();
}
}
}
// ---- Trmp3DMario_ResetAll
namespace ns_020cd424 {
extern "C" {
extern int Trmp3DMario_SetTuning(int sz);
extern void Trmp3DMario_SetSpawnHeight(int n);
extern void Trmp3DMario_EnterOffstage(char *c);
extern void Trmp3DMario_UpdateMatrix(char *o);
extern void Puff_ClearAll(int a0, int a1);

extern "C" int sTrmp3DMarioCount;
extern "C" int sTrmp3DLives;
extern "C" int sTrmpComboCount;
extern "C" int sTrmpMariosInPlay;
extern "C" int sTrmpMariosOnLedges;
extern "C" char *sTrmp3DMarios;

// @symbol Trmp3DMario_ResetAll
void Trmp3DMario_ResetAll(unsigned int n, int arg1) {
    int i;
    Trmp3DMario_SetTuning((n % 5 + n / 5) * 0x150 + 0x1000);
    Trmp3DMario_SetSpawnHeight(arg1);
    sTrmp3DLives = 3;
    sTrmpComboCount = 0;
    sTrmpMariosInPlay = 0;
    sTrmpMariosOnLedges = 0;
    for (i = 0; i < sTrmp3DMarioCount; i++) {
        Trmp3DMario_EnterOffstage(sTrmp3DMarios + i * 0xd0);
        Trmp3DMario_UpdateMatrix(sTrmp3DMarios + i * 0xd0);
    }
    Puff_ClearAll(0, 0);
}
}
}
// ---- Trmp3DMario_SetTuning
namespace ns_020cd510 {
extern "C" {
extern int func_020531a4(int a);
extern "C" int sTrmp3DFallSpeed;
extern "C" int sTrmp3DGravityStep;
extern "C" int sTrmp3DJumpOffSpeed;
extern "C" int sTrmp3DBounceScale;
extern "C" int sTrmp3DHardBounceScale;

// @symbol Trmp3DMario_SetTuning
void Trmp3DMario_SetTuning(int a) {
    int v;
    if (a > 0x2800)
        a = 0x2800;
    v = func_020531a4(a);
    sTrmp3DFallSpeed = FX_MUL(v, -0x2400);
    sTrmp3DGravityStep = FX_MUL(a, 0xe0);
    sTrmp3DJumpOffSpeed = FX_MUL(v, 0xc00);
    sTrmp3DBounceScale = FX_MUL(v, 0x4800);
    sTrmp3DHardBounceScale = FX_MUL(v, 0x5400);
}
}
}
// ---- Trmp3DMario_SetSpawnHeight
namespace ns_020cd62c {
extern "C" {
extern "C" int sTrmpRowBaseHeights[];
extern "C" int sTrmpRowHeights[];
// @symbol Trmp3DMario_SetSpawnHeight
void Trmp3DMario_SetSpawnHeight(int n) {
    sTrmpRowHeights[0] = sTrmpRowBaseHeights[0] + (n << 12);
    sTrmpRowHeights[1] = sTrmpRowBaseHeights[1] + (n << 12);
}
}
}
// ---- Trmp3DMario_InitPool
namespace ns_020cd658 {
extern "C" {
extern "C" int sTrmp3DMarioCount;
extern "C" unsigned char *sTrmp3DMarios;
extern "C" unsigned char *sTrmpJumpers[];
extern int Trmp3DMario_Load(unsigned char *);
// @symbol Trmp3DMario_InitPool
int Trmp3DMario_InitPool(unsigned char *a0, int a1) {
    int i = 0;
    int off;
    unsigned char *x;
    sTrmp3DMarioCount = a1;
    sTrmp3DMarios = a0;
    if (a1 > 0) {
        off = 0;
        do {
            if (Trmp3DMario_Load(sTrmp3DMarios + off) == 0)
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
}
}
// ---- TrmpRingObjAdapter_Clear
/* The Clear slot each bounce runs also restarts the combo
   TrmpItem_UpdateWithJumper scores from. */
// @symbol _ZN23dMgTrmpRingObjAdapter_c5ClearEv
void dMgTrmpRingObjAdapter_c::Clear() {
    dMgTrmpObjAdapter_c::Clear();
    mItemHitCount = 0;
}
// ---- TrmpRingObjAdapter_Construct
/* The base adapter is constructed before the body. */
// @symbol _ZN23dMgTrmpRingObjAdapter_cC2Ev
dMgTrmpRingObjAdapter_c::dMgTrmpRingObjAdapter_c() {
    mItemHitCount = 0;
}
// ---- TrmpObjAdapter_Clear
// @symbol _ZN19dMgTrmpObjAdapter_c5ClearEv
void dMgTrmpObjAdapter_c::Clear() {
    mContact = 0;
}
// ---- TrmpObjAdapter_Construct
// @symbol _ZN19dMgTrmpObjAdapter_cC2Ev
dMgTrmpObjAdapter_c::dMgTrmpObjAdapter_c() {
    mContact = 0;
}
