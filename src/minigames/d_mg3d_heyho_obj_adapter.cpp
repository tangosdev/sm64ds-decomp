//cpp
namespace Particle {
struct System {
    static void *NewSimple(unsigned, int, int, int);
};
}
/* dMg3DHeyhoObjAdapter_c translation unit, ov006 0x020c4048..0x020c6f8c, text only: the class's
   base constructor (its vtable and RTTI stay at their ROM homes in the module's unowned .data)
   plus 49 Heyho-minigame helper functions (see config/tu_manifest.d/ov006/dMg3DHeyhoObjAdapter_c.json).
   Function order is the reverse of the ROM: mwccarm 2004/b56 emits .text in reverse source order.
   Each former one-function C source sits in its own namespace so the per-file local types and
   extern declarations, which disagree with one another, stay as they were.

   leftovers:
   - The 49 Heyho_* helpers keep their per-function n<addr> namespaces. The
     record they operate on (0xf0 stride) is laid out in include/Heyho.h and read
     through the HEYHO() cast; the per-file typedef/extern universes disagree, so
     the helpers keep their char* parameters rather than a shared typed one.
   - Heyho::mAnmModel is a real dExtAnmModel_c, so its SetFile/SetAnim/Render/
     Advance calls and its mat4x3 store are member calls (the helpers that
     used to reach the matrix and the frame controller as separate byte views
     were reading inside that one object). Heyho's own constructor and
     destructor (ROM 0x020c6f70 and 0x020c6f3c) construct and destroy it.
   - The model and animation handles are file-scope objects; mwcc emits their
     sinit and destructor registration. The six ±256 side offsets are
     function-local static Vector3s, so their guards and registrations are
     inline in the three starters.
   - Volatile and long-long launders (e.g. Heyho_SeparateAll's (long long)(int)
     cast) are codegen-pinned; Heyho_MotionHover's const view is kept only for
     the TU's @NNN numbering. The inline comments at each site say what the
     ROM does.
   - The local struct views (Obj, Self, Ent, dActor_c) inside the n<addr>
     namespaces stay (they are parameter types of the helpers' local
     declarations), but their fields carry the role of the Heyho or
     dMg3DHeyhoObjAdapter_c member they alias: pos, vel, waypoint, speed,
     lean, size, separated, startVel*, range*, and CheckHitA's
     anchor/command. */
#include "common.h"
#include "decl_common.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "SharedFilePtr.h"
#include "dMgJump3DMario_c.h"
#include "Heyho.h"
#include "math/Approach.h"
/* Typed access to a Heyho record from the char* helpers below. */
#define HEYHO(p) ((Heyho *)(p))

/* File 0x217 / 0x218 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * func_02017acc / func_02017ab4 and SharedFilePtr::Construct /
 * SharedFilePtr_Destruct_Anim. */
struct HeyhoAdapterModelFilePtr : SharedFilePtr {
    HeyhoAdapterModelFilePtr(u32 fileID);
    ~HeyhoAdapterModelFilePtr();
};

struct HeyhoAdapterAnimationFileHandle : SharedFilePtr {
    HeyhoAdapterAnimationFileHandle(u32 fileID);
    ~HeyhoAdapterAnimationFileHandle();
};

extern HeyhoAdapterModelFilePtr gMgPHeyhoMiniModelFile;
extern HeyhoAdapterAnimationFileHandle gMgPHeyhoWaitAnimFile;

/* ---- Heyho::Heyho ---- */
// @symbol _ZN5HeyhoC1Ev
Heyho::Heyho() {}

/* ---- Heyho::~Heyho ---- */
/* Each Heyho releases both shared resource handles; mAnmModel's destructor
   runs after the body. */
// @symbol _ZN5HeyhoD1Ev
Heyho::~Heyho() {
    gMgPHeyhoMiniModelFile.Release();
    gMgPHeyhoWaitAnimFile.Release();
}

/* ---- Heyho_LoadResources ---- */
struct BMD_File; struct BCA_File;
extern HeyhoAdapterModelFilePtr gMgPHeyhoMiniModelFile;
extern HeyhoAdapterAnimationFileHandle gMgPHeyhoWaitAnimFile;
/* data_0209f5c0 is the current scene. This TU does not include
 * include/fBase_c.h, so fBaseView reads it up to actorID. */
struct fBase_c;
extern fBase_c *data_0209f5c0;
struct fBaseView {
    u8 pad_00[0xc];
    u16 actorID; /* 0x0c */
};
namespace n020c6e4c {
extern "C" void func_02016acc(dExtSimpleModel_c *m, unsigned int a);
extern "C" void func_02016b24(char *m, int a);
extern "C" void func_02016a14(char *m, int a);
extern "C" void func_02016a04(char *m, int a);
extern "C" void Heyho_UpdateMatrix(char *c);
extern "C" void Heyho_SetIdle(char *p);

extern "C" int Heyho_LoadResources(char *c) {
    BMD_File *m = (BMD_File *)dExtSimpleModel_c::LoadFile(gMgPHeyhoMiniModelFile);
    BCA_File *a = (BCA_File *)dExtFrameCtrl_c::LoadFile(gMgPHeyhoWaitAnimFile);
    if (HEYHO(c)->mAnmModel.SetFile(m, 1, -1) == 0) {
        return 0;
    }
    HEYHO(c)->mAnmModel.SetAnim(a, 0, 0x800, 0);
    func_02016acc(&HEYHO(c)->mAnmModel, 1u);
    func_02016b24((char *)&HEYHO(c)->mAnmModel, 2);
    int b = (((fBaseView *)data_0209f5c0)->actorID == 0x175);
    if (b != 0) {
        func_02016a14((char *)&HEYHO(c)->mAnmModel, 0x7fff);
        func_02016a04((char *)&HEYHO(c)->mAnmModel, 0x7d40);
    }
    Heyho_UpdateMatrix(c);
    Heyho_SetIdle(c);
    return 1;
}

}

/* ---- Heyho_StartMotionFlyIn ---- */
namespace n020c6ca4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[3]; } Blk3;
typedef struct { int w[4]; } Blk4;
typedef struct { int x, y, z, w; } Vec4;

extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
typedef struct { int w[2]; } Pair;
extern Pair pmf_Heyho_MotionFlyIn;

void Heyho_StartMotionFlyIn(char *dst, char *src) {
    /* First call builds the ±256 offsets. mwcc emits each guard, the three
       stores and the ~Vector3() registration here. */
    static Vector3 sHeyho_StartMotionFlyIn_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_StartMotionFlyIn_Right256(0x100000, 0, 0);

    HEYHO(dst)->mBaseSize = HEYHO(src)->mBaseSize;
    *(Blk3 *)&HEYHO(dst)->mStartVel = *(Blk3 *)&HEYHO(src)->mStartVel;
    *(Blk4 *)&HEYHO(dst)->mRangeLeft = *(Blk4 *)&HEYHO(src)->mRangeLeft;
    *(Blk3 *)&HEYHO(dst)->mGoal = *(Blk3 *)&HEYHO(src)->mGoal;
    HEYHO(dst)->mMoveKind = HEYHO(src)->mMoveKind;

    if (*(volatile int *)&HEYHO(dst)->mGoal.x > 0) {
        Vec3 *p = (Vec3 *)&HEYHO(dst)->mPos;
        HEYHO(dst)->mPos.x = HEYHO(dst)->mGoal.x;
        HEYHO(dst)->mPos.y = HEYHO(dst)->mGoal.y;
        HEYHO(dst)->mPos.z = HEYHO(dst)->mGoal.z;
        AddVec3((Vector3 *)p, (Vector3 *)&sHeyho_StartMotionFlyIn_Right256, (Vector3 *)p);
    } else {
        Vec3 *q = (Vec3 *)&HEYHO(dst)->mPos;
        HEYHO(dst)->mPos.x = HEYHO(dst)->mGoal.x;
        HEYHO(dst)->mPos.y = HEYHO(dst)->mGoal.y;
        HEYHO(dst)->mPos.z = HEYHO(dst)->mGoal.z;
        AddVec3((Vector3 *)q, (Vector3 *)&sHeyho_StartMotionFlyIn_Left256, (Vector3 *)q);
    }

    HEYHO(dst)->mScale.x = 0x1400;
    HEYHO(dst)->mScale.y = 0x1400;
    HEYHO(dst)->mScale.z = 0x1400;
    HEYHO(dst)->mSize = HEYHO(dst)->mBaseSize;
    HEYHO(dst)->mLean = 0;
    HEYHO(dst)->mDelay = 8;
    HEYHO(dst)->mSeparated = 0;
    *(Pair *)HEYHO(dst)->mMotion = pmf_Heyho_MotionFlyIn;
}
}
}

/* ---- Heyho_MotionFlyIn ---- */
namespace n020c6a9c {
extern "C" {
// @symbol Heyho_MotionFlyIn
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

void Heyho_MotionFlyIn(char *c) {
    short *ang = &HEYHO(c)->mPhase;
    *ang = *ang + 0x200;

    HEYHO(c)->mVel.x = FX_MUL(HEYHO(c)->mGoal.x - HEYHO(c)->mPos.x, 0x100);
    HEYHO(c)->mVel.y = FX_MUL(HEYHO(c)->mGoal.y - HEYHO(c)->mPos.y, 0x100);
    HEYHO(c)->mVel.z = FX_MUL(HEYHO(c)->mGoal.z - HEYHO(c)->mPos.z, 0x100);
    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);

    if ((HEYHO(c)->mVel.x < 0 ? -HEYHO(c)->mVel.x : HEYHO(c)->mVel.x) < 0x10 &&
        (HEYHO(c)->mVel.y < 0 ? -HEYHO(c)->mVel.y : HEYHO(c)->mVel.y) < 0x10) {
        HEYHO(c)->mPos.x = HEYHO(c)->mGoal.x;
        HEYHO(c)->mPos.y = HEYHO(c)->mGoal.y;
        HEYHO(c)->mPos.z = HEYHO(c)->mGoal.z;

        switch (HEYHO(c)->mMoveKind) {
        case 0:
            Heyho_StartMotionHover(c);
            break;
        case 1:
            Heyho_StartMotionPatrolBob(c);
            break;
        case 2:
            Heyho_StartMotionBobVertical(c);
            break;
        case 3:
            Heyho_StartMotionBounceNear(c);
            break;
        case 4:
            Heyho_StartMotionVShape(c);
            break;
        case 5:
            Heyho_StartMotionBounceInRange(c);
            break;
        case 6:
            Heyho_StartMotionDiagonal(c);
            break;
        case 7:
            Heyho_StartMotionDiagonalMirror(c);
            break;
        case 8:
            Heyho_StartMotionCircleUpper(c);
            break;
        case 9:
            Heyho_StartMotionCircleLower(c);
            break;
        case 10:
            Heyho_StartMotionChase(c);
            break;
        default:
            Heyho_StartMotionHover(c);
            break;
        }
    }

    ApproachLinear2(HEYHO(c)->mDelay, 0, 1);
    if (HEYHO(c)->mDelay != 1)
        return;
    D3D_PlaySeAtX(0x1c5, HEYHO(c)->mPos.x);
}
}
}

/* ---- Heyho_StartMotionRegroup ---- */
namespace n020c68f4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[3]; } Blk3;
typedef struct { int w[4]; } Blk4;
typedef struct { int x, y, z, w; } Vec4;

extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
typedef struct { int w[2]; } Pair;
extern Pair pmf_Heyho_MotionRegroup;

void Heyho_StartMotionRegroup(char *dst, char *src) {
    /* Same ±256 offsets as FlyIn, registered on the first regroup. */
    static Vector3 sHeyho_StartMotionRegroup_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_StartMotionRegroup_Right256(0x100000, 0, 0);

    HEYHO(dst)->mBaseSize = HEYHO(src)->mBaseSize;
    *(Blk3 *)&HEYHO(dst)->mStartVel = *(Blk3 *)&HEYHO(src)->mStartVel;
    *(Blk4 *)&HEYHO(dst)->mRangeLeft = *(Blk4 *)&HEYHO(src)->mRangeLeft;
    *(Blk3 *)&HEYHO(dst)->mGoal = *(Blk3 *)&HEYHO(src)->mGoal;
    HEYHO(dst)->mMoveKind = HEYHO(src)->mMoveKind;

    if (*(volatile int *)&HEYHO(dst)->mGoal.x > 0) {
        Vec3 *p = (Vec3 *)&HEYHO(dst)->mPos;
        HEYHO(dst)->mPos.x = HEYHO(dst)->mGoal.x;
        HEYHO(dst)->mPos.y = HEYHO(dst)->mGoal.y;
        HEYHO(dst)->mPos.z = HEYHO(dst)->mGoal.z;
        AddVec3((Vector3 *)p, (Vector3 *)&sHeyho_StartMotionRegroup_Right256, (Vector3 *)p);
    } else {
        Vec3 *q = (Vec3 *)&HEYHO(dst)->mPos;
        HEYHO(dst)->mPos.x = HEYHO(dst)->mGoal.x;
        HEYHO(dst)->mPos.y = HEYHO(dst)->mGoal.y;
        HEYHO(dst)->mPos.z = HEYHO(dst)->mGoal.z;
        AddVec3((Vector3 *)q, (Vector3 *)&sHeyho_StartMotionRegroup_Left256, (Vector3 *)q);
    }

    HEYHO(dst)->mDelay = 8;
    HEYHO(dst)->mScale.x = 0x1400;
    HEYHO(dst)->mScale.y = 0x1400;
    HEYHO(dst)->mScale.z = 0x1400;
    HEYHO(dst)->mSize = HEYHO(dst)->mBaseSize;
    HEYHO(dst)->mLean = 0;
    HEYHO(dst)->mSeparated = 0;
    *(Pair *)HEYHO(dst)->mMotion = pmf_Heyho_MotionRegroup;
}
}
}

/* ---- Heyho_MotionRegroup ---- */
namespace n020c66bc {
extern "C" {
typedef struct { int x, y, z; } Vec3;

struct Obj;

extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(Vec3 *v);
extern int D3D_PlaySeAtX(int a, int b);
extern void Heyho_StartMotionHover(char *p);
extern void Heyho_StartMotionPatrolBob(int *c);
extern void Heyho_StartMotionBobVertical(int *c);
extern void Heyho_StartMotionBounceNear(void *c);
extern void Heyho_StartMotionVShape(char *self);
extern void Heyho_StartMotionBounceInRange(int *c);
extern void Heyho_StartMotionDiagonal(struct Obj *o);
extern void Heyho_StartMotionDiagonalMirror(struct Obj *o);
extern void Heyho_StartMotionCircleUpper(void *c);
extern void Heyho_StartMotionCircleLower(void *c);
extern void Heyho_StartMotionChase(char *c);
extern void Heyho_CheckHitB(void *c);
extern void Heyho_CheckHitA(void *c);

void Heyho_MotionRegroup(char *c) {
    int dx, dy, dz;
    int vx, vy;

    HEYHO(c)->mPhase += 0x200;

    dx = HEYHO(c)->mGoal.x - HEYHO(c)->mPos.x;
    HEYHO(c)->mVel.x = (int)((((long long)dx << 8) + 0x800) >> 12);
    dy = HEYHO(c)->mGoal.y - HEYHO(c)->mPos.y;
    HEYHO(c)->mVel.y = (int)((((long long)dy << 8) + 0x800) >> 12);
    dz = HEYHO(c)->mGoal.z - HEYHO(c)->mPos.z;
    HEYHO(c)->mVel.z = (int)((((long long)dz << 8) + 0x800) >> 12);

    AddVec3((Vector3 *)&HEYHO(c)->mPos, (Vector3 *)&HEYHO(c)->mVel, (Vector3 *)&HEYHO(c)->mPos);

    vx = HEYHO(c)->mVel.x;
    if (vx < 0) vx = -vx;
    if (vx < 0x18) {
        vy = HEYHO(c)->mVel.y;
        if (vy < 0) vy = -vy;
        if (vy < 0x18) {
            HEYHO(c)->mPos.x = HEYHO(c)->mGoal.x;
            HEYHO(c)->mPos.y = HEYHO(c)->mGoal.y;
            HEYHO(c)->mPos.z = HEYHO(c)->mGoal.z;

            switch (HEYHO(c)->mMoveKind) {
            case 0:
                Heyho_StartMotionHover(c);
                break;
            case 1:
                Heyho_StartMotionPatrolBob((int *)c);
                break;
            case 2:
                Heyho_StartMotionBobVertical((int *)c);
                break;
            case 3:
                Heyho_StartMotionBounceNear(c);
                break;
            case 4:
                Heyho_StartMotionVShape(c);
                break;
            case 5:
                Heyho_StartMotionBounceInRange((int *)c);
                break;
            case 6:
                Heyho_StartMotionDiagonal((struct Obj *)c);
                break;
            case 7:
                Heyho_StartMotionDiagonalMirror((struct Obj *)c);
                break;
            case 8:
                Heyho_StartMotionCircleUpper(c);
                break;
            case 9:
                Heyho_StartMotionCircleLower(c);
                break;
            case 10:
                Heyho_StartMotionChase(c);
                break;
            default:
                Heyho_StartMotionHover(c);
                break;
            }

            D3D_PlaySeAtX(0x1c5, HEYHO(c)->mPos.x);
        }
    }

    ApproachLinear2(HEYHO(c)->mDelay, 0, 1);

    if (HEYHO(c)->mDelay == 1) {
        D3D_PlaySeAtX(0x1c5, HEYHO(c)->mPos.x);
    }

    if (LenVec3((Vec3 *)&HEYHO(c)->mVel) < 0x1000) {
        Heyho_CheckHitB(c);
    } else {
        Heyho_CheckHitA(c);
    }
}
}
}

/* ---- Heyho_React ---- */
namespace n020c64e4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;

extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);

extern int pmf_Heyho_StateIdle[2];
extern int pmf_Heyho_MotionHitSpin_2[2];
extern W2 pmf_Heyho_MotionLeave_2;

void Heyho_React(char *c)
{
    Vec3 locals[3];
    int *v;
    volatile int *q;
    unsigned base;

    v = HEYHO(c)->mMotion;
    q = (volatile int *)pmf_Heyho_StateIdle;
    if (v[0] == pmf_Heyho_StateIdle[0] && (v[1] == q[1] || HEYHO(c)->mMotion[0] == 0))
        return;

    base = (unsigned)(int)c;
    v = HEYHO(base)->mMotion;
    q = (volatile int *)pmf_Heyho_MotionHitSpin_2;
    if (v[0] == pmf_Heyho_MotionHitSpin_2[0] && (v[1] == q[1] || HEYHO(c)->mMotion[0] == 0))
        return;

    /* Same ±256 offsets, registered after the idle and hit-spin early outs. */
    static Vector3 sHeyho_React_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_React_Right256(0x100000, 0, 0);

    if (HEYHO(c)->mPos.x > 0) {
        Vec3_Add(&locals[1], (Vec3 *)&HEYHO(c)->mPos, (Vec3 *)&sHeyho_React_Right256);
        HEYHO(c)->mWaypoint.x = locals[1].x;
        HEYHO(c)->mWaypoint.y = locals[1].y;
        HEYHO(c)->mWaypoint.z = locals[1].z;
    } else {
        Vec3_Add(&locals[2], (Vec3 *)&HEYHO(c)->mPos, (Vec3 *)&sHeyho_React_Left256);
        HEYHO(c)->mWaypoint.x = locals[2].x;
        HEYHO(c)->mWaypoint.y = locals[2].y;
        HEYHO(c)->mWaypoint.z = locals[2].z;
    }

    HEYHO(c)->mScale.x = 0x1400;
    HEYHO(c)->mScale.y = 0x1400;
    HEYHO(c)->mScale.z = 0x1400;
    *(W2 *)HEYHO(c)->mMotion = pmf_Heyho_MotionLeave_2;
}

}
}

/* ---- Heyho_MotionLeave ---- */
namespace n020c6400 {
extern "C" {
typedef struct { int x, y, z; } Vec3;

extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *out, const Vec3 *in, int scale);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void Heyho_SetIdle(char *self);

void Heyho_MotionLeave(char *self) {
    Vec3 diff, scaled;
    int x, y;
    {
        short *pa = &HEYHO(self)->mPhase;
        *pa += 0x200;
    }
    Vec3_Sub(&diff, (Vec3 *)&HEYHO(self)->mWaypoint, (Vec3 *)&HEYHO(self)->mPos);
    Vec3_MulScalar(&scaled, &diff, 0x100);
    HEYHO(self)->mVel.x = scaled.x;
    HEYHO(self)->mVel.y = scaled.y;
    HEYHO(self)->mVel.z = scaled.z;
    AddVec3((Vector3 *)&HEYHO(self)->mPos, (Vector3 *)&HEYHO(self)->mVel,
            (Vector3 *)&HEYHO(self)->mPos);
    if (HEYHO(self)->mVel.x > 0)
        ApproachLinear(HEYHO(self)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(self)->mLean, -0x3000, 0x200);
    x = HEYHO(self)->mVel.x;
    if (x < 0) x = -x;
    if (x >= 0x20) return;
    y = HEYHO(self)->mVel.y;
    if (y < 0) y = -y;
    if (y >= 0x20) return;
    Heyho_SetIdle(self);
}
}
}

/* ---- Heyho_StartMotionHover ---- */
namespace n020c63dc {
extern "C" {
struct S { int w[2]; };
extern struct S pmf_Heyho_MotionHover;
void Heyho_StartMotionHover(char *p) {
    HEYHO(p)->mLean = 0;
    *(struct S *)HEYHO(p)->mMotion = pmf_Heyho_MotionHover;
}
}
}

/* ---- Heyho_MotionHover ---- */
namespace n020c6378 {
extern "C" {
extern s16 data_02082214[];
extern void Heyho_CheckHitB(void* this_);

void Heyho_MotionHover(int this_) {
    /* The two re-reads (mPhase, mVel.y) go through a const view `ro`. In the
       raw char* spelling the view kept mwcc 2004 from CSEing the +0xea and
       +0xac field addresses (the cartridge re-issues ldrh r2,[r0,#0xea] /
       ldr r1,[r0,#0xac]). With typed member reads it no longer changes the
       code (reading through HEYHO(this_) with `ro` still declared is
       byte-identical), but dropping the `ro` local renumbers the TU's @NNN
       labels (@1351 -> @1349), so it stays. */
    const Heyho *ro = (const Heyho *)this_;
    HEYHO(this_)->mPhase += 0x200;
    HEYHO(this_)->mVel.x = 0;
    HEYHO(this_)->mVel.y = data_02082214[((u16)ro->mPhase >> 4) * 2] >> 3;
    HEYHO(this_)->mPos.y += ro->mVel.y;
    Heyho_CheckHitB((void*)this_);
}
}
}

/* ---- Heyho_StartMotionPatrolBob ---- */
namespace n020c6348 {
extern "C" {
struct S {
    int w[2];
};
extern struct S pmf_Heyho_MotionPatrolBob;
void Heyho_StartMotionPatrolBob(int *c) {
    HEYHO(c)->mVel.x = 0x800;
    HEYHO(c)->mVel.y = 0;
    HEYHO(c)->mVel.z = 0;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionPatrolBob;
}
}
}

/* ---- Heyho_MotionPatrolBob ---- */
namespace n020c627c {
extern "C" {
extern void AddVec3(void* a, void* b, void* c);
extern void Heyho_CheckHitB(void* c);
extern short data_02082214[];

void Heyho_MotionPatrolBob(char *c) {
    int v, w;
    HEYHO(c)->mPhase += 0x200;
    {
        int a = (unsigned short)HEYHO(c)->mPhase >> 4;
        HEYHO(c)->mVel.y = data_02082214[a * 2] >> 3;
    }
    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);
    v = HEYHO(c)->mVel.x;
    if (v < 0) {
        if (HEYHO(c)->mPos.x < -0x68000) {
            HEYHO(c)->mVel.x = -v;
            goto skip_else_if;
        }
    }
    if (v > 0) {
        if (HEYHO(c)->mPos.x > 0x68000)
            HEYHO(c)->mVel.x = -v;
    }
skip_else_if:
    w = HEYHO(c)->mVel.x;
    if (w > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionBobVertical ---- */
namespace n020c6248 {
extern "C" {
struct S {
    int w[2];
};
extern struct S pmf_Heyho_MotionBobVertical;
void Heyho_StartMotionBobVertical(int *c) {
    HEYHO(c)->mVel.x = 0;
    HEYHO(c)->mVel.y = 0x800;
    HEYHO(c)->mVel.z = 0;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionBobVertical;
    HEYHO(c)->mLean = 0;
}
}
}

/* ---- Heyho_MotionBobVertical ---- */
namespace n020c61c4 {
extern "C" {
// @symbol Heyho_MotionBobVertical
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *out);
void Heyho_MotionBobVertical(int this_) {
    int f24 = HEYHO(this_)->mGoal.y;
    int a0 = HEYHO(this_)->mPos.y;
    if (a0 < f24 + HEYHO(this_)->mRangeBottom && HEYHO(this_)->mVel.y < 0) {
        HEYHO(this_)->mVel.y = -HEYHO(this_)->mVel.y;
        HEYHO(this_)->mPhase = 0;
    } else if (a0 > f24 + HEYHO(this_)->mRangeTop && HEYHO(this_)->mVel.y > 0) {
        HEYHO(this_)->mVel.y = -HEYHO(this_)->mVel.y;
        HEYHO(this_)->mPhase = 0;
    }
    AddVec3(&HEYHO(this_)->mPos, &HEYHO(this_)->mVel, &HEYHO(this_)->mPos);
    Heyho_CheckHitB((void*)this_);
}
}
}

/* ---- Heyho_StartMotionBounceNear ---- */
namespace n020c6188 {
extern "C" {
struct S{int w[2];};
extern struct S pmf_Heyho_MotionBounceNear;
void Heyho_StartMotionBounceNear(void *c) {
    HEYHO(c)->mLean = 0;
    HEYHO(c)->mVel.x = HEYHO(c)->mStartVel.x;
    HEYHO(c)->mVel.y = HEYHO(c)->mStartVel.y;
    HEYHO(c)->mVel.z = HEYHO(c)->mStartVel.z;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionBounceNear;
}
}
}

/* ---- Heyho_MotionBounceNear ---- */
namespace n020c6088 {
extern "C" {
// @symbol Heyho_MotionBounceNear
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

void Heyho_MotionBounceNear(char *c) {
    int base_y = HEYHO(c)->mGoal.y;
    int pos_y = HEYHO(c)->mPos.y;
    if (pos_y < base_y) {
        int vel = HEYHO(c)->mVel.y;
        if (vel < 0) {
            HEYHO(c)->mVel.y = -vel;
            HEYHO(c)->mPhase = 0;
            goto after_y;
        }
    }
    if (pos_y > base_y + 0x60000) {
        int vel = HEYHO(c)->mVel.y;
        if (vel > 0) {
            HEYHO(c)->mVel.y = -vel;
            HEYHO(c)->mPhase = 0;
        }
    }
after_y:;
    {
        int base_x = HEYHO(c)->mGoal.x;
        int pos_x = HEYHO(c)->mPos.x;
        if (pos_x < base_x - 0x60000) {
            int vel = HEYHO(c)->mVel.x;
            if (vel < 0) {
                HEYHO(c)->mVel.x = -vel;
                HEYHO(c)->mPhase = 0;
                goto after_x;
            }
        }
        if (pos_x > base_x + 0x60000) {
            int vel = HEYHO(c)->mVel.x;
            if (vel > 0) {
                HEYHO(c)->mVel.x = -vel;
                HEYHO(c)->mPhase = 0;
            }
        }
    after_x:;
    }

    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);
    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionVShape ---- */
namespace n020c5ec8 {
extern "C" {
typedef struct { int x, y, z; } Vec3;

extern int LenVec3(Vec3* v);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void Vec3_MulScalarInPlace(int* v, int s);
typedef struct { int w[2]; } S2;
extern S2 pmf_Heyho_MotionVShape;

void Heyho_StartMotionVShape(char *self) {
    Vec3 v0, v1, v2, d0, d1, d2, diff;
    int len0, len1, len2;

    HEYHO(self)->mLean = 0;
    HEYHO(self)->mVel.x = HEYHO(self)->mStartVel.x;
    HEYHO(self)->mVel.y = HEYHO(self)->mStartVel.y;
    HEYHO(self)->mVel.z = HEYHO(self)->mStartVel.z;
    HEYHO(self)->mSpeed = LenVec3((Vec3 *)&HEYHO(self)->mVel);

    {
        int y = HEYHO(self)->mRangeTop;
        int x = HEYHO(self)->mRangeLeft;
        v0.x = x;
        v0.y = y;
        v0.z = 0;
    }
    {
        int y = HEYHO(self)->mRangeTop;
        int x = HEYHO(self)->mRangeRight;
        v1.x = x;
        v1.y = y;
        v1.z = 0;
    }
    {
        int a = HEYHO(self)->mRangeLeft;
        int b = HEYHO(self)->mRangeRight;
        int c = HEYHO(self)->mRangeBottom;
        v2.x = (a + b) >> 1;
        v2.y = c;
        v2.z = 0;
    }

    Vec3_Sub(&d0, &v0, (Vec3 *)&HEYHO(self)->mPos);
    len0 = LenVec3(&d0);
    Vec3_Sub(&d1, &v1, (Vec3 *)&HEYHO(self)->mPos);
    len1 = LenVec3(&d1);
    Vec3_Sub(&d2, &v2, (Vec3 *)&HEYHO(self)->mPos);
    len2 = LenVec3(&d2);

    if (len2 < len1 && len2 < len0) {
        HEYHO(self)->mWaypoint.x = v2.x;
        HEYHO(self)->mWaypoint.y = v2.y;
        HEYHO(self)->mWaypoint.z = v2.z;
    } else if (len1 < len0) {
        HEYHO(self)->mWaypoint.x = v1.x;
        HEYHO(self)->mWaypoint.y = v1.y;
        HEYHO(self)->mWaypoint.z = v1.z;
    } else {
        HEYHO(self)->mWaypoint.x = v0.x;
        HEYHO(self)->mWaypoint.y = v0.y;
        HEYHO(self)->mWaypoint.z = v0.z;
    }

    Vec3_Sub(&diff, (Vec3 *)&HEYHO(self)->mWaypoint, (Vec3 *)&HEYHO(self)->mPos);
    HEYHO(self)->mVel.x = diff.x;
    HEYHO(self)->mVel.y = diff.y;
    HEYHO(self)->mVel.z = diff.z;
    if (NormalizeVec3IfNonZero((Vec3 *)&HEYHO(self)->mVel) == 0) {
        HEYHO(self)->mVel.x = HEYHO(self)->mStartVel.x;
        HEYHO(self)->mVel.y = HEYHO(self)->mStartVel.y;
        HEYHO(self)->mVel.z = HEYHO(self)->mStartVel.z;
    } else {
        Vec3_MulScalarInPlace(&HEYHO(self)->mVel.x, HEYHO(self)->mSpeed);
    }

    *(S2 *)HEYHO(self)->mMotion = pmf_Heyho_MotionVShape;
}
}
}

/* ---- Heyho_MotionVShape ---- */
namespace n020c5d28 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern void Heyho_CheckHitB(void* c);

void Heyho_MotionVShape(char *c) {
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = HEYHO(c)->mVel.x;
    r5 = ApproachLinear(HEYHO(c)->mPos.x, HEYHO(c)->mWaypoint.x, t < 0 ? -t : t);
    t = HEYHO(c)->mVel.y;
    r0v = ApproachLinear(HEYHO(c)->mPos.y, HEYHO(c)->mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeBottom) {
        int c_a8 = HEYHO(c)->mVel.x;
        if (c_a8 > 0) {
            HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeRight;
            HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeTop;
        } else {
            HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeLeft;
            HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeTop;
        }
        Vec3_Sub(&sp0, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp0.x;
        HEYHO(c)->mVel.y = sp0.y;
        HEYHO(c)->mVel.z = sp0.z;
        func_0203ce80(&spC, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeTop) {
        HEYHO(c)->mWaypoint.x = (HEYHO(c)->mRangeLeft + HEYHO(c)->mRangeRight) >> 1;
        HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeBottom;
        Vec3_Sub(&sp18, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp18.x;
        HEYHO(c)->mVel.y = sp18.y;
        HEYHO(c)->mVel.z = sp18.z;
        func_0203ce80(&sp24, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    }

    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);

    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionBounceInRange ---- */
namespace n020c5cf4 {
extern "C" {
struct S {
    int w[2];
};
extern struct S pmf_Heyho_MotionBounceInRange;
void Heyho_StartMotionBounceInRange(int *c) {
    HEYHO(c)->mVel.x = HEYHO(c)->mStartVel.x;
    HEYHO(c)->mVel.y = HEYHO(c)->mStartVel.y;
    HEYHO(c)->mVel.z = HEYHO(c)->mStartVel.z;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionBounceInRange;
}
}
}

/* ---- Heyho_MotionBounceInRange ---- */
namespace n020c5bf8 {
extern "C" {
// @symbol Heyho_MotionBounceInRange
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

void Heyho_MotionBounceInRange(char *c) {
    int a8 = HEYHO(c)->mVel.x;
    if (a8 < 0 && HEYHO(c)->mPos.x < HEYHO(c)->mGoal.x + HEYHO(c)->mRangeLeft) {
        HEYHO(c)->mVel.x = -a8;
    } else if (a8 > 0 && HEYHO(c)->mPos.x > HEYHO(c)->mGoal.x + HEYHO(c)->mRangeRight) {
        HEYHO(c)->mVel.x = -a8;
    }
    int ac = HEYHO(c)->mVel.y;
    if (ac < 0 && HEYHO(c)->mPos.y < HEYHO(c)->mGoal.y + HEYHO(c)->mRangeBottom) {
        HEYHO(c)->mVel.y = -ac;
    } else if (ac > 0 && HEYHO(c)->mPos.y > HEYHO(c)->mGoal.y + HEYHO(c)->mRangeTop) {
        HEYHO(c)->mVel.y = -ac;
    }
    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);
    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionDiagonal ---- */
namespace n020c5aa4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;
extern int LenVec3(Vec3 *v);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Vec3_MulScalarInPlace(int *v, int s);

struct Obj {
    int pad0;
    int startVelX, startVelY, startVelZ;
    int rangeLeft, rangeRight, rangeBottom, rangeTop;
    char gap20[0x9c - 0x20];
    Vec3 pos;
    Vec3 vel;
    Vec3 waypoint;
    char gapc0[0xdc - 0xc0];
    int speed;
    char gape0[0xe6 - 0xe0];
    short lean;
};

extern W2 pmf_Heyho_MotionDiagonal;

void Heyho_StartMotionDiagonal(struct Obj *o) {
    Vec3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    o->vel.x = o->startVelX;
    o->vel.y = o->startVelY;
    o->vel.z = o->startVelZ;
    o->speed = LenVec3(&o->vel);
    o->lean = 0;
    ay = o->rangeTop;
    ax = o->rangeLeft;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = o->rangeBottom;
    bx = o->rangeRight;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &o->pos);
    la = LenVec3(&da);
    Vec3_Sub(&db, &b, &o->pos);
    if (LenVec3(&db) < la) {
        o->waypoint.x = a.x;
        o->waypoint.y = a.y;
        o->waypoint.z = a.z;
    } else {
        o->waypoint.x = b.x;
        o->waypoint.y = b.y;
        o->waypoint.z = b.z;
    }
    Vec3_Sub(&t, &o->waypoint, &o->pos);
    o->vel.x = t.x;
    o->vel.y = t.y;
    o->vel.z = t.z;
    if (NormalizeVec3IfNonZero(&o->vel) == 0) {
        o->vel.x = o->startVelX;
        o->vel.y = o->startVelY;
        o->vel.z = o->startVelZ;
    } else {
        Vec3_MulScalarInPlace((int *)&o->vel, o->speed);
    }
    *(W2 *)HEYHO(o)->mMotion = pmf_Heyho_MotionDiagonal;
}
}
}

/* ---- Heyho_MotionDiagonal ---- */
namespace n020c5928 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern void Heyho_CheckHitB(void* c);

void Heyho_MotionDiagonal(char *c) {
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = HEYHO(c)->mVel.x;
    r5 = ApproachLinear(HEYHO(c)->mPos.x, HEYHO(c)->mWaypoint.x, t < 0 ? -t : t);
    t = HEYHO(c)->mVel.y;
    r0v = ApproachLinear(HEYHO(c)->mPos.y, HEYHO(c)->mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeBottom) {
        HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeLeft;
        HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeTop;
        Vec3_Sub(&sp0, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp0.x;
        HEYHO(c)->mVel.y = sp0.y;
        HEYHO(c)->mVel.z = sp0.z;
        func_0203ce80(&spC, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeTop) {
        HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeRight;
        HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeBottom;
        Vec3_Sub(&sp18, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp18.x;
        HEYHO(c)->mVel.y = sp18.y;
        HEYHO(c)->mVel.z = sp18.z;
        func_0203ce80(&sp24, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    }

    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);

    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionDiagonalMirror ---- */
namespace n020c57d4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;
extern int LenVec3(Vec3 *v);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Vec3_MulScalarInPlace(int *v, int s);

struct Obj {
    int pad0;
    int startVelX, startVelY, startVelZ;
    int rangeLeft, rangeRight, rangeBottom, rangeTop;
    char gap20[0x9c - 0x20];
    Vec3 pos;
    Vec3 vel;
    Vec3 waypoint;
    char gapc0[0xdc - 0xc0];
    int speed;
    char gape0[0xe6 - 0xe0];
    short lean;
};

extern W2 pmf_Heyho_MotionDiagonalMirror;

void Heyho_StartMotionDiagonalMirror(struct Obj *o) {
    Vec3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    o->vel.x = o->startVelX;
    o->vel.y = o->startVelY;
    o->vel.z = o->startVelZ;
    o->speed = LenVec3(&o->vel);
    o->lean = 0;
    ay = o->rangeTop;
    ax = o->rangeRight;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = o->rangeBottom;
    bx = o->rangeLeft;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &o->pos);
    la = LenVec3(&da);
    Vec3_Sub(&db, &b, &o->pos);
    if (LenVec3(&db) < la) {
        o->waypoint.x = a.x;
        o->waypoint.y = a.y;
        o->waypoint.z = a.z;
    } else {
        o->waypoint.x = b.x;
        o->waypoint.y = b.y;
        o->waypoint.z = b.z;
    }
    Vec3_Sub(&t, &o->waypoint, &o->pos);
    o->vel.x = t.x;
    o->vel.y = t.y;
    o->vel.z = t.z;
    if (NormalizeVec3IfNonZero(&o->vel) == 0) {
        o->vel.x = o->startVelX;
        o->vel.y = o->startVelY;
        o->vel.z = o->startVelZ;
    } else {
        Vec3_MulScalarInPlace((int *)&o->vel, o->speed);
    }
    *(W2 *)HEYHO(o)->mMotion = pmf_Heyho_MotionDiagonalMirror;
}
}
}

/* ---- Heyho_MotionDiagonalMirror ---- */
namespace n020c5658 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern void Heyho_CheckHitB(void* c);

void Heyho_MotionDiagonalMirror(char *c) {
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = HEYHO(c)->mVel.x;
    r5 = ApproachLinear(HEYHO(c)->mPos.x, HEYHO(c)->mWaypoint.x, t < 0 ? -t : t);
    t = HEYHO(c)->mVel.y;
    r0v = ApproachLinear(HEYHO(c)->mPos.y, HEYHO(c)->mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeBottom) {
        HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeRight;
        HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeTop;
        Vec3_Sub(&sp0, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp0.x;
        HEYHO(c)->mVel.y = sp0.y;
        HEYHO(c)->mVel.z = sp0.z;
        func_0203ce80(&spC, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && HEYHO(c)->mWaypoint.y == HEYHO(c)->mRangeTop) {
        HEYHO(c)->mWaypoint.x = HEYHO(c)->mRangeLeft;
        HEYHO(c)->mWaypoint.y = HEYHO(c)->mRangeBottom;
        Vec3_Sub(&sp18, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
        HEYHO(c)->mVel.x = sp18.x;
        HEYHO(c)->mVel.y = sp18.y;
        HEYHO(c)->mVel.z = sp18.z;
        func_0203ce80(&sp24, (Vec3 *)&HEYHO(c)->mVel);
        Vec3_MulScalarInPlace(&HEYHO(c)->mVel.x, HEYHO(c)->mSpeed);
        HEYHO(c)->mPhase = 0;
    }

    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);

    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionCircleUpper ---- */
namespace n020c561c {
extern "C" {
struct S{int w[2];};
extern struct S pmf_Heyho_MotionCircleUpper;
void Heyho_StartMotionCircleUpper(void *c) {
    HEYHO(c)->mLean = 0;
    HEYHO(c)->mVel.x = HEYHO(c)->mStartVel.x;
    HEYHO(c)->mVel.y = HEYHO(c)->mStartVel.y;
    HEYHO(c)->mVel.z = HEYHO(c)->mStartVel.z;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionCircleUpper;
}
}
}

/* ---- Heyho_MotionCircleUpper ---- */
namespace n020c5530 {
extern "C" {
// @symbol Heyho_MotionCircleUpper
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

void Heyho_MotionCircleUpper(char *c) {
    int a8 = HEYHO(c)->mVel.x;
    if (a8 < 0 && HEYHO(c)->mPos.x < -0x68000) {
        HEYHO(c)->mVel.y = -a8;
        HEYHO(c)->mVel.x = 0;
    } else if (a8 > 0 && HEYHO(c)->mPos.x > 0x68000) {
        HEYHO(c)->mVel.y = -a8;
        HEYHO(c)->mVel.x = 0;
    }
    int ac = HEYHO(c)->mVel.y;
    if (ac < 0 && HEYHO(c)->mPos.y < 0x20000) {
        HEYHO(c)->mVel.x = ac;
        HEYHO(c)->mVel.y = 0;
    } else if (ac > 0 && HEYHO(c)->mPos.y > 0xc0000) {
        HEYHO(c)->mVel.x = ac;
        HEYHO(c)->mVel.y = 0;
    }
    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);
    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionCircleLower ---- */
namespace n020c54f4 {
extern "C" {
struct S{int w[2];};
extern struct S pmf_Heyho_MotionCircleLower;
void Heyho_StartMotionCircleLower(void *c) {
    HEYHO(c)->mLean = 0;
    HEYHO(c)->mVel.x = HEYHO(c)->mStartVel.x;
    HEYHO(c)->mVel.y = HEYHO(c)->mStartVel.y;
    HEYHO(c)->mVel.z = HEYHO(c)->mStartVel.z;
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionCircleLower;
}
}
}

/* ---- Heyho_MotionCircleLower ---- */
namespace n020c53f8 {
extern "C" {
// @symbol Heyho_MotionCircleLower
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);

void Heyho_MotionCircleLower(char *c) {
    int a8 = HEYHO(c)->mVel.x;
    if (a8 < 0 && HEYHO(c)->mPos.x < -0x68000) {
        HEYHO(c)->mVel.y = a8;
        HEYHO(c)->mVel.x = 0;
    } else if (a8 > 0 && HEYHO(c)->mPos.x > 0x68000) {
        HEYHO(c)->mVel.y = a8;
        HEYHO(c)->mVel.x = 0;
    }
    int ac = HEYHO(c)->mVel.y;
    if (ac < 0 && HEYHO(c)->mPos.y < -0xc8000) {
        HEYHO(c)->mVel.x = -ac;
        HEYHO(c)->mVel.y = 0;
    } else if (ac > 0 && HEYHO(c)->mPos.y > -0x20000) {
        HEYHO(c)->mVel.x = -ac;
        HEYHO(c)->mVel.y = 0;
    }
    AddVec3(&HEYHO(c)->mPos, &HEYHO(c)->mVel, &HEYHO(c)->mPos);
    if (HEYHO(c)->mVel.x > 0)
        ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
    else
        ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    Heyho_CheckHitB(c);
}
}
}

/* ---- Heyho_StartMotionChase ---- */
namespace n020c5370 {
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int pmf_Heyho_MotionChase[];

struct W2 { int w[2]; };

void Heyho_StartMotionChase(char *c) {
    unsigned int r;
    HEYHO(c)->mLean = 0;
    r = (unsigned int)RandomIntInternal(&data_0209e650);
    r &= 0x7fffffff;
    HEYHO(c)->mDelay = (short)(((int)((r >> 0x13) * 0x2d) >> 0xc) + 0xf);
    HEYHO(c)->mWaypoint.x = HEYHO(c)->mPos.x;
    HEYHO(c)->mWaypoint.y = HEYHO(c)->mPos.y;
    HEYHO(c)->mWaypoint.z = HEYHO(c)->mPos.z;
    HEYHO(c)->mVel.x = HEYHO(c)->mStartVel.x;
    HEYHO(c)->mVel.y = HEYHO(c)->mStartVel.y;
    HEYHO(c)->mVel.z = HEYHO(c)->mStartVel.z;
    *(struct W2 *)HEYHO(c)->mMotion = *(struct W2 *)pmf_Heyho_MotionChase;
}
}
}

/* ---- Heyho_MotionChase ---- */
namespace n020c4fa4 {

typedef struct Vec3 { int x, y, z; } Vec3;

struct VB {
    virtual int *m0();
    virtual int *m1();
    virtual int m2();
};

extern "C" {
extern int RandomIntInternal(int *seed);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int Vec3_Dist(Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *d, Vec3 *s, int m);
extern void Vec3_Sub(Vec3 *d, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *d, Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int DotVec3(Vec3 *a, Vec3 *b);
extern void Heyho_CheckHitB(char *c);
extern int data_0209e650;
extern char *sJumpAdapters[];
}

extern "C" void Heyho_MotionChase(char *c) {
    VB *best;
    Vec3 bp;
    Vec3 op;
    Vec3 delta;
    Vec3 nrm;
    Vec3 cand;
    Vec3 dir;
    Vec3 diff;
    Vec3 scaled;
    Vec3 proj;
    Vec3 sc2;
    Vec3 tgt;
    Vec3 vel;
    int bm, i, ra, rb;

    if (HEYHO(c)->mSeparated != 0) {
        ApproachLinear(HEYHO(c)->mWaypoint.x, HEYHO(c)->mPos.x, 0x1000);
        ApproachLinear(HEYHO(c)->mWaypoint.y, HEYHO(c)->mPos.y, 0x1000);
    }
    {
        int t = HEYHO(c)->mVel.x;
        if (t < 0) t = -t;
        ra = ApproachLinear(HEYHO(c)->mPos.x, HEYHO(c)->mWaypoint.x, t);
    }
    {
        int t = HEYHO(c)->mVel.y;
        if (t < 0) t = -t;
        rb = ApproachLinear(HEYHO(c)->mPos.y, HEYHO(c)->mWaypoint.y, t);
    }
    if (ra != 0)
        HEYHO(c)->mVel.x = 0;
    if (rb != 0)
        HEYHO(c)->mVel.y = 0;
    if (ra != 0 && rb != 0 && ApproachLinear(HEYHO(c)->mLean, 0, 0x200) != 0 &&
        ApproachLinear2(HEYHO(c)->mDelay, 0, 1) != 0) {
        best = 0;
        bm = 0x20000;
        for (i = 0; i < 3; i++) {
            if (((VB *)sJumpAdapters[i])->m2() != 0) {
                int *v;
                int dot;
                v = ((VB *)sJumpAdapters[i])->m0();
                op.x = v[0]; op.y = v[1]; op.z = v[2];
                Vec3_Sub(&delta, (Vec3 *)&HEYHO(c)->mPos, &op);
                v = ((VB *)sJumpAdapters[i])->m1();
                nrm.x = v[0]; nrm.y = v[1]; nrm.z = v[2];
                if (NormalizeVec3IfNonZero(&nrm) != 0) {
                    dot = DotVec3(&nrm, &delta);
                    if (dot > 0) {
                        int d;
                        Vec3_MulScalar(&scaled, &nrm, dot);
                        Vec3_Add(&proj, &op, &scaled);
                        cand.x = proj.x; cand.y = proj.y; cand.z = proj.z;
                        d = Vec3_Dist((Vec3 *)&HEYHO(c)->mPos, &cand);
                        if (d < bm) {
                            best = (VB *)sJumpAdapters[i];
                            bp = cand;
                            bm = d;
                        }
                    }
                }
            }
        }
        if (best != 0) {
            Vec3_Sub(&dir, (Vec3 *)&HEYHO(c)->mPos, &bp);
            if (NormalizeVec3IfNonZero(&dir) == 0) {
                dir.x = 0x1000; dir.y = 0; dir.z = 0;
            }
            Vec3_MulScalar(&sc2, &dir, 0x40000);
            Vec3_Add(&tgt, (Vec3 *)&HEYHO(c)->mPos, &sc2);
            HEYHO(c)->mWaypoint.x = tgt.x;
            HEYHO(c)->mWaypoint.y = tgt.y;
            HEYHO(c)->mWaypoint.z = tgt.z;
            {
                int v2 = HEYHO(c)->mWaypoint.x;
                if (v2 < -0x68000) v2 = -0x68000;
                else if (v2 > 0x68000) v2 = 0x68000;
                HEYHO(c)->mWaypoint.x = v2;
            }
            {
                int v3 = HEYHO(c)->mWaypoint.y;
                if (v3 < -0xa8000) v3 = -0xa8000;
                else if (v3 > 0xa0000) v3 = 0xa0000;
                HEYHO(c)->mWaypoint.y = v3;
            }
            {
                int len;
                Vec3_Sub(&diff, (Vec3 *)&HEYHO(c)->mWaypoint, (Vec3 *)&HEYHO(c)->mPos);
                len = LenVec3((Vec3 *)&HEYHO(c)->mStartVel);
                if (NormalizeVec3IfNonZero(&diff) != 0) {
                    Vec3_MulScalar(&vel, &diff, len);
                    HEYHO(c)->mVel.x = vel.x;
                    HEYHO(c)->mVel.y = vel.y;
                    HEYHO(c)->mVel.z = vel.z;
                } else {
                    int w = FX_MUL(len, 0xb50);
                    HEYHO(c)->mVel.x = w;
                    HEYHO(c)->mVel.y = w;
                }
            }
            {
                int rr = RandomIntInternal(&data_0209e650) & 0x7fffffff;
                HEYHO(c)->mDelay = (short)(((int)((unsigned int)rr >> 0x13) * 0x2d >> 12) + 0xf);
            }
        }
    } else {
        int df = HEYHO(c)->mWaypoint.x - HEYHO(c)->mPos.x;
        if (df > 0)
            ApproachLinear(HEYHO(c)->mLean, 0x3000, 0x200);
        else if (df < 0)
            ApproachLinear(HEYHO(c)->mLean, -0x3000, 0x200);
    }
    Heyho_CheckHitB(c);
}
}

/* ---- Heyho_StartMotionHitSpin ---- */
namespace n020c4f68 {
extern "C" {
struct S{int w[2];};
extern struct S pmf_Heyho_MotionHitSpin;
extern void D3D_PlaySeAtX(int a, int b);
void Heyho_StartMotionHitSpin(int *c) {
    HEYHO(c)->mPhase = 0;
    D3D_PlaySeAtX(0x1c8, HEYHO(c)->mPos.x);
    *(struct S *)HEYHO(c)->mMotion = pmf_Heyho_MotionHitSpin;
}
}
}

/* ---- Heyho_MotionHitSpin ---- */
namespace n020c4e8c {
extern "C" {
int D3D_PlaySeAtX(int a, int b);
void Heyho_Settle(char* c);
extern short data_02082214[];

void Heyho_MotionHitSpin(char *c) {
    if (HEYHO(c)->mPhase < 0x4000) {
        short *p = &HEYHO(c)->mPhase;
        short* tbl = data_02082214;
        int ang, idx, idx1, r0;
        *p = *p + 0x400;
        ang = (unsigned short)HEYHO(c)->mPhase >> 4;
        idx = ang * 2;
        idx1 = idx + 1;
        r0 = FX_MUL((tbl[idx] >> 1) + 0x1000, 0x1400);
        HEYHO(c)->mScale.x = r0;
        HEYHO(c)->mScale.y = FX_MUL((tbl[idx1] >> 1) + 0x800, 0x1400);
        HEYHO(c)->mScale.z = r0;
        return;
    }
    D3D_PlaySeAtX(0x1c7, HEYHO(c)->mPos.x);
    Heyho_Settle(c);
}
}
}

/* ---- Heyho_Settle ---- */
namespace n020c4d3c {
extern "C" {
typedef int Fix12;
extern void *d_mg3d_heyho_obj_adapterNewSimpleHold(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
extern void Camera_ProjectToScreen(void* a, void* b, int* c, int* d);
extern int Mg_GetTopScreenYOffset(void);
extern void Jump2Popup_Spawn(int a, int b, int c);
extern void Heyho_SetIdle(char* p);

extern int sHeyhoUnsettledCount;
extern int sJumpComboCount;
extern int sHeyhoScorePopups;
extern void *sMgSubCamera;
extern void *sMgMainCamera;

/* View of Particle::System (include/Particle__System.h) up to its
 * callbackScale; including that header renumbers this TU's pinned @NNN
 * labels. */
struct ParticleSystemView {
    u8 pad_000[0x50];
    s32 callbackScale; /* 0x50 */
};

void Heyho_Settle(char* c)
{
    void* a;
    void* b;
    int v0, v1;
    int t;

    a = Particle::System::NewSimple(0x122, HEYHO(c)->mPos.x << 3, HEYHO(c)->mPos.y << 3,
                                    HEYHO(c)->mPos.z << 3);
    b = Particle::System::NewSimple(0x123, HEYHO(c)->mPos.x << 3, HEYHO(c)->mPos.y << 3,
                                    HEYHO(c)->mPos.z << 3);
    t = (int)(HEYHO(c)->mSize << 0x1c) >> 0x10;
    /* Both stores go through a; the cartridge never sets b's scale. */
    if (a)
        ((ParticleSystemView *)a)->callbackScale = t;
    if (b)
        ((ParticleSystemView *)a)->callbackScale = t;

    ApproachLinear(sHeyhoUnsettledCount, 0, 1);
    ApproachLinear(sJumpComboCount, 0x270f, 1);

    if (sHeyhoScorePopups != 0) {
        if (HEYHO(c)->mPos.y > 0) {
            Camera_ProjectToScreen(sMgSubCamera, (void *)&HEYHO(c)->mPos, &v0, &v1);
            v1 = v1 - (Mg_GetTopScreenYOffset() + 0xc0);
        } else {
            Camera_ProjectToScreen(sMgMainCamera, (void *)&HEYHO(c)->mPos, &v0, &v1);
        }
        v1 = v1 - 0x20;
        Jump2Popup_Spawn(v0 << 0xc, v1 << 0xc, (short)sJumpComboCount);
    }

    Heyho_SetIdle(c);
}

}
}

/* ---- Heyho_SetIdle ---- */
namespace n020c4d20 {
extern "C" {
struct S { int w[2]; };
extern struct S pmf_Heyho_StateIdle_2;
void Heyho_SetIdle(char *p) {
    *(struct S *)HEYHO(p)->mMotion = pmf_Heyho_StateIdle_2;
}
}
}

/* ---- Heyho_StateIdle ---- */
namespace n020c4d1c {
extern "C" {
void Heyho_StateIdle(void)
{
}

}
}

/* ---- Heyho_Update ---- */
namespace n020c4cd8 {
extern "C" {
void Heyho_UpdateMatrix(void *c);
}
struct Foo {
  char pad[0x30];
  void (Foo::*pmf)();
};
extern "C" void Heyho_Update(struct Foo *c){
    (c->*(c->pmf))();
    HEYHO(c)->mAnmModel.Advance();
    Heyho_UpdateMatrix(c);
}
}

/* ---- Heyho_Draw ---- */
namespace n020c4c54 {
// @symbol Heyho_Draw

extern "C" void Vec3_MulScalar(Vector3 *res, Vector3 *v, int scalar);
extern "C" void Heyho_Draw(int this_) {
    Vector3 v;
    int *p = HEYHO(this_)->mMotion;
    Vector3 *d = &pmf_Heyho_StateIdle_3;
    if (p[0] == d->x) {
        if (p[1] == d->y)
            return;
        if (*(volatile int *)&HEYHO(this_)->mMotion[0] == 0)
            return;
    }
    Vec3_MulScalar(&v, &HEYHO(this_)->mScale, HEYHO(this_)->mSize);
    HEYHO(this_)->mAnmModel.Render(&v);
}

}

/* ---- Heyho_UpdateMatrix ---- */
namespace n020c4c00 {
extern "C" {
// @symbol Heyho_UpdateMatrix
extern struct Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3* m, short angY);
void Heyho_UpdateMatrix(char* c) {
    Matrix4x3_FromTranslation(&data_020a0e68, HEYHO(c)->mPos.x, HEYHO(c)->mPos.y, HEYHO(c)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, HEYHO(c)->mLean);
    HEYHO(c)->mAnmModel.mat4x3 = data_020a0e68;
}

}
}

/* ---- Heyho_CheckHitB ---- */
namespace n020c49d8 {
extern "C" {
typedef struct {
    int x, y, z;
} Vec3;
typedef int (**VT)(void *);

struct Self {
    char pad0[0x9c];
    Vec3 pos;                /* 0x9c */
    Vec3 vel;                /* 0xa8 */
    char padb4[0xd8 - 0xb4]; /* 0xb4 */
    int size;                /* 0xd8 -- Heyho::mSize */
};

extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int DotVec3(Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void Heyho_StartMotionHitSpin(struct Self *self);
extern char *sJumpAdapters[];

/* sJumpAdapters[i] as the adapter it is; OM re-reads the slot (volatile)
   before each mCommand store, as the cartridge does. */
#define O  ((dMg3DHeyhoObjAdapter_c *)sJumpAdapters[i])
#define OM ((dMg3DHeyhoObjAdapter_c *)((char* volatile*)sJumpAdapters)[i])

void Heyho_CheckHitB(struct Self* self)
{
    int i;
    int lim = FX_MUL(self->size, 0x12000);
    int half = lim >> 1;

    for (i = 0; i < 3; i++) {
        Vec3 a;
        Vec3 b;
        Vec3 d;
        Vec3 c;

        if ((*(VT *)sJumpAdapters[i])[2](sJumpAdapters[i]) == 0)
            continue;

        Vec3_Sub(&a, (Vec3 *)(*(VT *)sJumpAdapters[i])[0](sJumpAdapters[i]), &self->pos);
        a.y -= half;
        if (a.y < 0) {
            if (a.y > -0x18000) a.y = 0;
            else a.y += 0x18000;
        } else {
            if (a.x > 0x2000) a.x -= 0x2000;
            else if (a.x < -0x2000) a.x += 0x2000;
            else a.x = 0;
        }

        Vec3_Sub(&b, (Vec3 *)(*(VT *)sJumpAdapters[i])[1](sJumpAdapters[i]), &self->vel);
        Vec3_Add(&c, &a, &b);
        if (LenVec3(&c) > lim) continue;

        if (NormalizeVec3IfNonZero(&a) == 0) {
            OM->mCommand = 1;
            O->mAnchor.x = self->pos.x;
            O->mAnchor.y = self->pos.y;
            O->mAnchor.z = self->pos.z;
            Heyho_StartMotionHitSpin(self);
        }

        d.x = 0;
        d.y = 0x1000;
        d.z = 0;
        if (DotVec3(&d, &a) > 0) {
            OM->mCommand = 1;
            O->mAnchor.x = self->pos.x;
            O->mAnchor.y = self->pos.y;
            O->mAnchor.z = self->pos.z;
            Heyho_StartMotionHitSpin(self);
            return;
        } else {
            OM->mCommand = 2;
            O->mAnchor.x = self->pos.x;
            O->mAnchor.y = self->pos.y;
            O->mAnchor.z = self->pos.z;
        }
    }
}

}
}

/* ---- Heyho_CheckHitA ---- */
namespace n020c47d4 {
extern "C" {
typedef int Fix12;
typedef struct Vec3 { Fix12 x, y, z; } Vec3;

typedef struct Obj Obj;
typedef struct ObjVt {
    Vec3 *(*f0)(Obj *);
    Vec3 *(*f1)(Obj *);
    int   (*f2)(Obj *);
} ObjVt;
struct Obj {
    ObjVt *vt;      /* 0x0 */
    Vec3 anchor;    /* 0x4 dMg3DHeyhoObjAdapter_c::mAnchor */
    short command;  /* 0x10 dMg3DHeyhoObjAdapter_c::mCommand */
};

typedef struct Self {
    char _pad0[0x9c];
    Vec3 pos;                 /* 0x9c */
    Vec3 vel;                 /* 0xa8 */
    char _padb4[0xd8 - 0xb4]; /* 0xb4 */
    int size;                 /* 0xd8 -- Heyho::mSize */
} Self;

extern char *sJumpAdapters[];
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern Fix12 DotVec3(const Vec3 *a, const Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Heyho_StartMotionHitSpin(Self *self);

void Heyho_CheckHitA(Self *self)
{
    int i;
    int thresh = FX_MUL(self->size, 0x12000);

    for (i = 0; i < 3; i++) {
        Vec3 disp, rel, up, sum;
        Vec3 *p, *q;
        Obj *o;

        if (((Obj **)sJumpAdapters)[i]->vt->f2(((Obj **)sJumpAdapters)[i]) == 0)
            continue;

        p = ((Obj **)sJumpAdapters)[i]->vt->f0(((Obj **)sJumpAdapters)[i]);
        Vec3_Sub(&disp, p, &self->pos);

        disp.y -= thresh >> 1;
        if (disp.y < 0) {
            if (disp.y > -0x18000)
                disp.y = 0;
            else
                disp.y += 0x18000;
        } else {
            if (disp.x > 0x2000)
                disp.x -= 0x2000;
            else if (disp.x < -0x2000)
                disp.x += 0x2000;
            else
                disp.x = 0;
        }

        q = ((Obj **)sJumpAdapters)[i]->vt->f1(((Obj **)sJumpAdapters)[i]);
        Vec3_Sub(&rel, q, &self->vel);
        Vec3_Add(&sum, &disp, &rel);

        if (LenVec3(&sum) > thresh)
            continue;

        if (NormalizeVec3IfNonZero(&disp) == 0) {
            ((Obj **)sJumpAdapters)[i]->command = 1;
            o = ((Obj **)sJumpAdapters)[i];
            o->anchor.x = self->pos.x;
            o->anchor.y = self->pos.y;
            o->anchor.z = self->pos.z;
            Heyho_StartMotionHitSpin(self);
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (DotVec3(&up, &disp) <= 0)
            continue;

        if (rel.y >= 0)
            continue;

        ((Obj **)sJumpAdapters)[i]->command = 1;
        o = ((Obj **)sJumpAdapters)[i];
        o->anchor.x = self->pos.x;
        o->anchor.y = self->pos.y;
        o->anchor.z = self->pos.z;
        Heyho_StartMotionHitSpin(self);
        return;
    }
}

}
}

/* ---- Heyho_IsChasing ---- */
namespace n020c4710 {
extern "C" {
typedef struct UID {
    int low, high;
} UID;

typedef struct dActor_c {
    int pad[0x30 / 4];
    UID id;                          /* 0x30 */
    unsigned char pad2[0xe6 - 0x38]; /* 0x38 */
    short lean;                      /* 0xe6 -- Heyho::mLean */
} dActor_c;

extern UID pmf_Heyho_MotionLeave;
extern UID pmf_Heyho_MotionChase_2;

int Heyho_IsChasing(dActor_c *a)
{
    int result = 0;
    UID *p = (UID *)(int)(&a->id);
    UID *c = (UID *)(int)(&pmf_Heyho_MotionLeave);
    int neq = 1;
    if (p->low == c->low) {
        if (p->high == c->high || a->id.low == 0)
            neq = 0;
    }
    if (neq != 0) {
        int flag = 1;
        int neq2 = flag;
        UID *q = (UID *)(int)((long long)(unsigned int)&a->id);
        UID *c2 = (UID *)(int)(&pmf_Heyho_MotionChase_2);
        if (q->low == c2->low) {
            if (q->high == c2->high || a->id.low == 0)
                neq2 = 0;
        }
        if (neq2 == 0) {
            if (a->lean != 0)
                flag = 0;
        }
        if (flag != 0)
            result = 1;
    }
    return result;
}
}
}

/* ---- Heyho_InitPool ---- */
namespace n020c4684 {
extern "C" {
extern int Heyho_LoadResources(char *p);
extern int sHeyhoCount;
extern int sHeyhoUnsettledCount;
extern char *sHeyhos;
extern int sHeyhoScorePopups;
int Heyho_InitPool(char *ptr, int n);
int Heyho_InitPool(char *ptr, int n){
    int i;
    int off;
    sHeyhoScorePopups = 0;
    sHeyhoCount = n;
    sHeyhoUnsettledCount = n;
    sHeyhos = ptr;
    if (n > 0) {
        i = 0;
        off = 0;
        do {
            if (Heyho_LoadResources(sHeyhos + off) == 0)
                return 0;
            i++;
            off += 0xf0;
        } while (i < sHeyhoCount);
    }
    return 1;
}

}
}

/* ---- Heyho_SpawnAll ---- */
namespace n020c44b4 {
extern "C" {
extern void *data_ov006_021402fc;
extern int sHeyhoUnsettledCount;
extern int sJumpComboCount;
extern int sHeyhoLayout;
extern char *sHeyhos;
extern int sHeyhoCount;
extern int data_ov006_021403b8[];
extern int data_0209e650;
extern char sHeyhoLayoutFlyInPaths[];
extern char sHeyhoFlyInPaths[];

extern void Heyho_StartMotionFlyIn(char *dst, char *src);
extern void Heyho_UpdateMatrix(char *c);
extern int RandomIntInternal(int *seed);

void Heyho_SpawnAll(void *arg0, int arg1)
{
    int count;
    int sl;
    int sb;
    int r8;
    int r4;
    int *seed;
    int r6;
    int sp0;
    int sp4;
    int sp8;
    int sp10;
    int spc;
    int r1;
    char *base;
    int t;

    count = sHeyhoCount;
    sHeyhoLayout = arg1;
    data_ov006_021402fc = arg0;
    sHeyhoUnsettledCount = count;
    sJumpComboCount = 0;
    if (count <= 0)
        return;

    seed = &data_0209e650;
    sl = 0;
    r8 = sl;
    sp4 = sl;
    r4 = sl;
    spc = sl;
    sp10 = sl;
    sp8 = sl;

    do {
        if (sl < 3) {
            base = sHeyhos + r8;
            HEYHO(base)->mPhase = (short)sp8;
            Heyho_StartMotionFlyIn(sHeyhos + r8,
                                   sHeyhoLayoutFlyInPaths + sHeyhoLayout * 0x90 + sp4);
            Heyho_UpdateMatrix(sHeyhos + r8);
        } else {
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sb = t >> 0xc;
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sp0 = t >> 0xc;
            r1 = spc;
            r6 = (sl - 3) | (r1 - r1);
            if (r6 > 0) {
                do {
                    if (sb == data_ov006_021403b8[r1]) {
                        t = RandomIntInternal(seed) & 0x7fffffff;
                        t = (int)((unsigned)t >> 0x13);
                        t = t << 2;
                        sb = t >> 0xc;
                        r1 = r4;
                    }
                    r1 = r1 + 1;
                } while (r1 < r6);
            }
            data_ov006_021403b8[sl - 3] = sb;
            base = sHeyhos + r8;
            HEYHO(base)->mPhase = (short)sp10;
            Heyho_StartMotionFlyIn(sHeyhos + r8, sHeyhoFlyInPaths + sb * 0xc0 + sp0 * 0x30);
            Heyho_UpdateMatrix(sHeyhos + r8);
        }
        sp4 = sp4 + 0x30;
        r8 = r8 + 0xf0;
        sl = sl + 1;
    } while (sl < sHeyhoCount);
}

}
}

/* ---- Heyho_SeparateAll ---- */
namespace n020c42bc {
extern "C" {
typedef struct {
    int x, y, z;
} Vec3;

struct Obj {
    char pad0[0x9c];
    Vec3 pos;                /* 0x9c */
    Vec3 vel;                /* 0xa8 */
    char padb4[0xe0 - 0xb4]; /* 0xb4 */
    int separated;           /* 0xe0 -- Heyho::mSeparated */
    char pade4[0xf0 - 0xe4]; /* 0xe4 */
};

extern char *sHeyhos;
extern int sHeyhoCount;

extern void Heyho_Update(struct Obj* o);
extern int Heyho_IsChasing(struct Obj* o);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);

void Heyho_SeparateAll(void)
{
    Vec3 v1;
    Vec3 v2;
    struct Obj* oi;
    struct Obj* oj;
    Vec3* pi;
    Vec3* pj;
    int i, j, k, t;
    Vec3* ai;

    for (i = 0; i < sHeyhoCount; i++) {
        Heyho_Update(&((struct Obj *)sHeyhos)[i]);
    }
    for (k = 0; k < sHeyhoCount; k++) {
        ((struct Obj *)sHeyhos)[k].separated = 0;
    }
    for (t = 0; t < sHeyhoCount; t++) {
        if (Heyho_IsChasing(&((struct Obj *)sHeyhos)[t]) != 0) {
            oi = &((struct Obj *)sHeyhos)[t];
            pi = &oi->pos;
            ai = &oi->vel;
            for (j = t + 1; j < sHeyhoCount; j++) {
                if (Heyho_IsChasing(&((struct Obj *)sHeyhos)[j]) != 0) {
                    oj = (struct Obj *)((long long)(int)&((struct Obj *)sHeyhos)[j]);
                    pj = &oj->pos;
                    Vec3_Sub(&v1, pi, pj);
                    Vec3_Sub(&v2, ai, &oj->vel);
                    if (LenVec3(&v1) >= 0x12000) continue;
                    if (LenVec3(&v2) >= 0x100) continue;
                    if (NormalizeVec3IfNonZero(&v1) == 0) continue;
                    AddVec3((Vector3 *)pi, (Vector3 *)&v1, (Vector3 *)pi);
                    SubVec3((Vector3 *)pj, (Vector3 *)&v1, (Vector3 *)pj);
                    ((struct Obj *)sHeyhos)[t].separated = 1;
                    ((struct Obj *)sHeyhos)[j].separated = 1;
                }
            }
        }
    }
}
}
}

/* ---- Heyho_DrawAll ---- */
namespace n020c425c {
extern "C" {
extern void Heyho_Draw(int);
extern int sHeyhoCount;
extern char *sHeyhos;
void Heyho_DrawAll(void){
  int i = 0;
  if (sHeyhoCount > 0) {
      int off = 0;
      do {
          Heyho_Draw((int)(sHeyhos + off));
          i++;
          off += 0xf0;
      } while (i < sHeyhoCount);
  }
}

}
}

/* ---- Heyho_SettleAll ---- */
namespace n020c4148 {
extern "C" {
typedef struct Ent {
    char _pad0[0x30];
    int x;
    int y;
    char _pad38[0xb8];
} Ent;

extern int sHeyhoCount;
extern char *sHeyhos;
extern int data_ov006_0213afc8[2];
extern char sHeyhoSettlePaths;
extern int sHeyhoUnsettledCount;
extern int data_0209e650;

extern int RandomIntInternal(int *seed);
extern void Heyho_StartMotionRegroup(char *a, char *b);

void Heyho_SettleAll(void)
{
    int i;
    for (i = 0; i < sHeyhoCount; i++) {
        int *v = (int *)(&((Ent *)sHeyhos)[i].x);
        volatile int *q = (volatile int *)data_ov006_0213afc8;
        if (v[0] != data_ov006_0213afc8[0] || (v[1] != q[1] && ((Ent *)sHeyhos)[i].x != 0))
            continue;
        {
            int r1 = RandomIntInternal(&data_0209e650);
            int r2 = RandomIntInternal(&data_0209e650);
            unsigned v0 = (unsigned)(r1 & 0x7fffffff) >> 0x13;
            unsigned u0 = (unsigned)(r2 & 0x7fffffff) >> 0x13;
            Heyho_StartMotionRegroup((char *)sHeyhos + i * 0xf0,
                                     &sHeyhoSettlePaths + (((int)(v0 * 9)) >> 0xc) * 0xf0 +
                                         (((int)(u0 * 5)) >> 0xc) * 0x30);
            ApproachLinear(sHeyhoUnsettledCount, sHeyhoCount, 1);
            return;
        }
    }
}

}
}

/* ---- Heyho_ReactAll ---- */
namespace n020c40e8 {
extern "C" {
extern void Heyho_React(int);
extern int sHeyhoCount;
extern char *sHeyhos;
void Heyho_ReactAll(void){
  int i = 0;
  if (sHeyhoCount > 0) {
      int off = 0;
      do {
          Heyho_React((int)(sHeyhos + off));
          i++;
          off += 0xf0;
      } while (i < sHeyhoCount);
  }
}

}
}

/* ---- Heyho_AllIdle ---- */
namespace n020c4060 {
extern "C" {
// Scans sHeyhoCount entries (stride 0xf0) from sHeyhos:
// returns 0 if any entry's pair at +0x30 mismatches pmf_Heyho_StateIdle_4
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x30];  // 0x00
    int x;             // 0x30
    int y;             // 0x34
    char _pad38[0xb8]; // 0x38 (stride 0xf0)
} Ent;

extern int sHeyhoCount;
extern char *sHeyhos;
extern int pmf_Heyho_StateIdle_4[2];

int Heyho_AllIdle(void) {
    int i;
    for (i = 0; i < sHeyhoCount; i++) {
        int *v = (int *)(&((Ent *)sHeyhos)[i].x);
        volatile int* q = (volatile int*)pmf_Heyho_StateIdle_4;
        if (v[0] != pmf_Heyho_StateIdle_4[0] || (v[1] != q[1] && ((Ent *)sHeyhos)[i].x != 0))
            return 0;
    }
    return 1;
}

}
}

// @symbol _ZN22dMg3DHeyhoObjAdapter_cC2Ev
dMg3DHeyhoObjAdapter_c::dMg3DHeyhoObjAdapter_c() : mCommand(0)
{
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the reverse-emitted .text run. Construction order is
 * the model handle, then the animation handle. The plain counters have
 * no initializer: one would grow the sinit. */
extern "C" {
int sHeyhoUnsettledCount;
int sJumpComboCount;
int sHeyhoScorePopups;
int sHeyhoLayout;
char *sHeyhos;
int sHeyhoCount;
int data_ov006_021403b8[3];
char *sJumpAdapters[3];
}

HeyhoAdapterModelFilePtr gMgPHeyhoMiniModelFile(0x217);
HeyhoAdapterAnimationFileHandle gMgPHeyhoWaitAnimFile(0x218);
