//cpp
/* dMg3DHeyhoObjAdapter_c translation unit, ov006 0x020c4048..0x020c6f8c, text only: the class's
   base constructor (its vtable and RTTI stay at their ROM homes in the module's unowned .data),
   the Heyho (Shy Guy) record's member functions, and the pool functions the Jump scenes call
   (see config/tu_manifest.d/ov006/dMg3DHeyhoObjAdapter_c.json).
   Function order is the reverse of the ROM: mwccarm 2004/b56 emits .text in reverse source order.

   leftovers:
   - Heyho::mMotion is a real pointer to member, and every motion is a Heyho member. The
     23 pointer-to-member literals the motions are installed from and compared against
     (pmf_Heyho_*, data_ov006_0213afc8) are the compiler's &Heyho::X temporaries, but they
     sit in the module's .data, which this text-only TU does not own; the TU reads them by
     their configured names until it does.
   - Heyho::mAnmModel is a real dExtAnmModel_c. Heyho's own constructor and destructor (ROM
     0x020c6f70 and 0x020c6f3c) construct and destroy it.
   - The model and animation handles are file-scope objects; mwcc emits their sinit and
     destructor registration. The six +-256 side offsets are function-local static Vector3s,
     so their guards and registrations are inline in the three starters.
   - Three codegen pins stay, each measured and commented at its site: the volatile re-read
     of mPath.goal.x in the two starters, SeparateAll's (long long)(int) widening, and
     React's three-slot Vector3 array (its stack layout).
   - sJumpAdapters is char *[3] because d_mg_jump3d_mario.cpp fills it that way; JumpAdapter()
     reads a slot as the dMg3DHeyhoObjAdapter_c it is. */
#include "common.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "MgCamera.h"
#include "decl_common.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "SharedFilePtr.h"
#include "fBase_c.h"
#include "dMgJump3DMario_c.h"
#include "Heyho.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "LevelRng.h"

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

struct BMD_File;
struct BCA_File;

/* The running scene. */
extern fBase_c *gCurrentScene;

typedef void (Heyho::*HeyhoMotion)();

/* A random int in [0, n) from the actors' random state. */
#define HEYHO_RAND(n) ((int)(((u32)(RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 19) * (n)) >> 12)

extern "C" {

extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);

extern void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);

extern void Vec3_MulScalarInPlace(Vector3 *v, int s);

extern int NormalizeVec3IfNonZero(Vector3 *v);
}
void Jump2Popup_Spawn(int x, int y, int number);

extern HeyhoMotion pmf_Heyho_MotionPatrolBob;

extern HeyhoMotion pmf_Heyho_MotionBounceNear;

extern HeyhoMotion pmf_Heyho_MotionChase;

extern HeyhoMotion pmf_Heyho_MotionCircleUpper;

extern HeyhoMotion pmf_Heyho_MotionFlyIn;

extern HeyhoMotion pmf_Heyho_MotionDiagonal;

extern HeyhoMotion pmf_Heyho_MotionCircleLower;

extern HeyhoMotion pmf_Heyho_MotionDiagonalMirror;

extern HeyhoMotion pmf_Heyho_MotionBounceInRange;

extern HeyhoMotion pmf_Heyho_MotionRegroup;

extern HeyhoMotion pmf_Heyho_MotionVShape;

extern HeyhoMotion pmf_Heyho_StateIdle;

extern HeyhoMotion pmf_Heyho_MotionBobVertical;

extern HeyhoMotion pmf_Heyho_MotionHitSpin;

extern HeyhoMotion pmf_Heyho_MotionHover;

extern HeyhoMotion pmf_Heyho_StateIdle_2;

extern HeyhoMotion pmf_Heyho_StateIdle_3;

extern HeyhoMotion pmf_Heyho_MotionLeave;

extern HeyhoMotion pmf_Heyho_StateIdle_4;

extern HeyhoMotion pmf_Heyho_MotionChase_2;

extern HeyhoMotion pmf_Heyho_MotionLeave_2;

extern HeyhoMotion pmf_Heyho_MotionHitSpin_2;

extern HeyhoMotion data_ov006_0213afc8;

extern int sHeyhoUnsettledCount;

extern int sJumpComboCount;

extern int sHeyhoScorePopups;

extern int sHeyhoLayout;

extern Heyho *sHeyhos;

extern int sHeyhoCount;

extern int data_ov006_021403b8[];

extern char *sJumpAdapters[];

extern void *data_ov006_021402fc;

extern const HeyhoPath sHeyhoLayoutFlyInPaths[][3];

extern const HeyhoPath sHeyhoFlyInPaths[][4];

extern const HeyhoPath sHeyhoSettlePaths[][5];

extern MgCamera *sMgSubCamera;

/* The Jump scene's three players, seen through the adapter base the Heyhos
   test against. */
static inline dMg3DHeyhoObjAdapter_c *JumpAdapter(int i) {
    return (dMg3DHeyhoObjAdapter_c *)sJumpAdapters[i];
}

/* Pins the player's anchor to the Heyho it touched. */
static inline void SetAnchor(dMg3DHeyhoObjAdapter_c *adapter, const Vector3 &pos) {
    adapter->mAnchor.x = pos.x;
    adapter->mAnchor.y = pos.y;
    adapter->mAnchor.z = pos.z;
}

/* ---- Heyho::Heyho ---- */
Heyho::Heyho() {}

/* ---- Heyho::~Heyho ---- */
/* Each Heyho releases both shared resource handles; mAnmModel's destructor
   runs after the body. */
Heyho::~Heyho() {
    gMgPHeyhoMiniModelFile.Release();
    gMgPHeyhoWaitAnimFile.Release();
}

/* ---- Heyho::LoadResources ---- */
int Heyho::LoadResources() {
    BMD_File *m = dExtSimpleModel_c::LoadFile(gMgPHeyhoMiniModelFile);
    BCA_File *a = dExtFrameCtrl_c::LoadFile(gMgPHeyhoWaitAnimFile);
    if (mAnmModel.SetFile(m, 1, -1) == 0) {
        return 0;
    }
    mAnmModel.SetAnim(a, 0, 0x800, 0);
    mAnmModel.ClearMaterialFlags(1);
    mAnmModel.SetMaterialFlags(2);
    int b = gCurrentScene->actorID == 0x175;
    if (b != 0) {
        func_02016a14((char *)&mAnmModel, 0x7fff);
        func_02016a04((char *)&mAnmModel, 0x7d40);
    }
    UpdateMatrix();
    SetIdle();
    return 1;
}

/* ---- Heyho::StartMotionFlyIn ---- */
void Heyho::StartMotionFlyIn(const HeyhoPath *path) {
    /* First call builds the ±256 offsets. mwcc emits each guard, the three
       stores and the ~Vector3() registration here. */
    static Vector3 sHeyho_StartMotionFlyIn_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_StartMotionFlyIn_Right256(0x100000, 0, 0);

    mPath = *path;

    /* The ROM re-loads goal.x from the record after the copy instead of
       reusing the copied register; only a volatile read reproduces it. */
    if (*(volatile int *)&mPath.goal.x > 0) {
        mPos.x = mPath.goal.x;
        mPos.y = mPath.goal.y;
        mPos.z = mPath.goal.z;
        VEC_Add(&mPos, &sHeyho_StartMotionFlyIn_Right256, &mPos);
    } else {
        mPos.x = mPath.goal.x;
        mPos.y = mPath.goal.y;
        mPos.z = mPath.goal.z;
        VEC_Add(&mPos, &sHeyho_StartMotionFlyIn_Left256, &mPos);
    }

    mScale.x = 0x1400;
    mScale.y = 0x1400;
    mScale.z = 0x1400;
    mSize = mPath.baseSize;
    mLean = 0;
    mDelay = 8;
    mSeparated = 0;
    mMotion = pmf_Heyho_MotionFlyIn;
}

/* ---- Heyho::MotionFlyIn ---- */
void Heyho::MotionFlyIn() {
    mPhase += 0x200;

    mVel.x = FX_MUL(mPath.goal.x - mPos.x, 0x100);
    mVel.y = FX_MUL(mPath.goal.y - mPos.y, 0x100);
    mVel.z = FX_MUL(mPath.goal.z - mPos.z, 0x100);
    VEC_Add(&mPos, &mVel, &mPos);

    if ((mVel.x < 0 ? -mVel.x : mVel.x) < 0x10 && (mVel.y < 0 ? -mVel.y : mVel.y) < 0x10) {
        mPos.x = mPath.goal.x;
        mPos.y = mPath.goal.y;
        mPos.z = mPath.goal.z;

        switch (mPath.moveKind) {
        case 0:
            StartMotionHover();
            break;
        case 1:
            StartMotionPatrolBob();
            break;
        case 2:
            StartMotionBobVertical();
            break;
        case 3:
            StartMotionBounceNear();
            break;
        case 4:
            StartMotionVShape();
            break;
        case 5:
            StartMotionBounceInRange();
            break;
        case 6:
            StartMotionDiagonal();
            break;
        case 7:
            StartMotionDiagonalMirror();
            break;
        case 8:
            StartMotionCircleUpper();
            break;
        case 9:
            StartMotionCircleLower();
            break;
        case 10:
            StartMotionChase();
            break;
        default:
            StartMotionHover();
            break;
        }
    }

    ApproachLinear2(mDelay, 0, 1);
    if (mDelay != 1)
        return;
    D3D_PlaySeAtX(0x1c5, mPos.x);
}

/* ---- Heyho::StartMotionRegroup ---- */
void Heyho::StartMotionRegroup(const HeyhoPath *path) {
    /* Same ±256 offsets as FlyIn, registered on the first regroup. */
    static Vector3 sHeyho_StartMotionRegroup_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_StartMotionRegroup_Right256(0x100000, 0, 0);

    mPath = *path;

    /* goal.x re-read: see StartMotionFlyIn. */
    if (*(volatile int *)&mPath.goal.x > 0) {
        mPos.x = mPath.goal.x;
        mPos.y = mPath.goal.y;
        mPos.z = mPath.goal.z;
        VEC_Add(&mPos, &sHeyho_StartMotionRegroup_Right256, &mPos);
    } else {
        mPos.x = mPath.goal.x;
        mPos.y = mPath.goal.y;
        mPos.z = mPath.goal.z;
        VEC_Add(&mPos, &sHeyho_StartMotionRegroup_Left256, &mPos);
    }

    mDelay = 8;
    mScale.x = 0x1400;
    mScale.y = 0x1400;
    mScale.z = 0x1400;
    mSize = mPath.baseSize;
    mLean = 0;
    mSeparated = 0;
    mMotion = pmf_Heyho_MotionRegroup;
}

/* ---- Heyho::MotionRegroup ---- */
void Heyho::MotionRegroup() {
    int vx, vy;

    mPhase += 0x200;

    mVel.x = FX_MUL(mPath.goal.x - mPos.x, 0x100);
    mVel.y = FX_MUL(mPath.goal.y - mPos.y, 0x100);
    mVel.z = FX_MUL(mPath.goal.z - mPos.z, 0x100);

    VEC_Add(&mPos, &mVel, &mPos);

    vx = mVel.x;
    if (vx < 0) vx = -vx;
    if (vx < 0x18) {
        vy = mVel.y;
        if (vy < 0) vy = -vy;
        if (vy < 0x18) {
            mPos.x = mPath.goal.x;
            mPos.y = mPath.goal.y;
            mPos.z = mPath.goal.z;

            switch (mPath.moveKind) {
            case 0:
                StartMotionHover();
                break;
            case 1:
                StartMotionPatrolBob();
                break;
            case 2:
                StartMotionBobVertical();
                break;
            case 3:
                StartMotionBounceNear();
                break;
            case 4:
                StartMotionVShape();
                break;
            case 5:
                StartMotionBounceInRange();
                break;
            case 6:
                StartMotionDiagonal();
                break;
            case 7:
                StartMotionDiagonalMirror();
                break;
            case 8:
                StartMotionCircleUpper();
                break;
            case 9:
                StartMotionCircleLower();
                break;
            case 10:
                StartMotionChase();
                break;
            default:
                StartMotionHover();
                break;
            }

            D3D_PlaySeAtX(0x1c5, mPos.x);
        }
    }

    ApproachLinear2(mDelay, 0, 1);

    if (mDelay == 1) {
        D3D_PlaySeAtX(0x1c5, mPos.x);
    }

    if (VEC_Mag(&mVel) < 0x1000) {
        CheckHitB();
    } else {
        CheckHitA();
    }
}

/* ---- Heyho::React ---- */
void Heyho::React() {
    /* Slot 0 is never named; the three-slot array is what gives the ROM's
       stack layout (two separate locals cost 14 words). */
    Vector3 locals[3];

    if (mMotion == pmf_Heyho_StateIdle)
        return;
    if (mMotion == pmf_Heyho_MotionHitSpin_2)
        return;

    /* Same ±256 offsets, registered after the idle and hit-spin early outs. */
    static Vector3 sHeyho_React_Left256(-0x100000, 0, 0);
    static Vector3 sHeyho_React_Right256(0x100000, 0, 0);

    if (mPos.x > 0) {
        Vec3_Add(&locals[1], &mPos, &sHeyho_React_Right256);
        mWaypoint.x = locals[1].x;
        mWaypoint.y = locals[1].y;
        mWaypoint.z = locals[1].z;
    } else {
        Vec3_Add(&locals[2], &mPos, &sHeyho_React_Left256);
        mWaypoint.x = locals[2].x;
        mWaypoint.y = locals[2].y;
        mWaypoint.z = locals[2].z;
    }

    mScale.x = 0x1400;
    mScale.y = 0x1400;
    mScale.z = 0x1400;
    mMotion = pmf_Heyho_MotionLeave_2;
}

/* ---- Heyho::MotionLeave ---- */
void Heyho::MotionLeave() {
    Vector3 diff, scaled;
    int x, y;
    mPhase += 0x200;
    Vec3_Sub(&diff, &mWaypoint, &mPos);
    Vec3_MulScalar(&scaled, &diff, 0x100);
    mVel.x = scaled.x;
    mVel.y = scaled.y;
    mVel.z = scaled.z;
    VEC_Add(&mPos, &mVel, &mPos);
    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    x = mVel.x;
    if (x < 0)
        x = -x;
    if (x >= 0x20)
        return;
    y = mVel.y;
    if (y < 0)
        y = -y;
    if (y >= 0x20)
        return;
    SetIdle();
}

/* ---- Heyho::StartMotionHover ---- */
void Heyho::StartMotionHover() {
    mLean = 0;
    mMotion = pmf_Heyho_MotionHover;
}

/* ---- Heyho::MotionHover ---- */
void Heyho::MotionHover() {
    mPhase += 0x200;
    mVel.x = 0;
    mVel.y = FX_SinIdx((u16)mPhase >> 4) >> 3;
    mPos.y += mVel.y;
    CheckHitB();
}

/* ---- Heyho::StartMotionPatrolBob ---- */
void Heyho::StartMotionPatrolBob() {
    mVel.x = 0x800;
    mVel.y = 0;
    mVel.z = 0;
    mMotion = pmf_Heyho_MotionPatrolBob;
}

/* ---- Heyho::MotionPatrolBob ---- */
void Heyho::MotionPatrolBob() {
    int v, w;
    mPhase += 0x200;
    mVel.y = FX_SinIdx((u16)mPhase >> 4) >> 3;
    VEC_Add(&mPos, &mVel, &mPos);
    v = mVel.x;
    if (v < 0 && mPos.x < -0x68000)
        mVel.x = -v;
    else if (v > 0 && mPos.x > 0x68000)
        mVel.x = -v;
    w = mVel.x;
    if (w > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    CheckHitB();
}

/* ---- Heyho::StartMotionBobVertical ---- */
void Heyho::StartMotionBobVertical() {
    mVel.x = 0;
    mVel.y = 0x800;
    mVel.z = 0;
    mMotion = pmf_Heyho_MotionBobVertical;
    mLean = 0;
}

/* ---- Heyho::MotionBobVertical ---- */
void Heyho::MotionBobVertical() {
    int goalY = mPath.goal.y;
    int posY = mPos.y;
    if (posY < goalY + mPath.range.bottom && mVel.y < 0) {
        mVel.y = -mVel.y;
        mPhase = 0;
    } else if (posY > goalY + mPath.range.top && mVel.y > 0) {
        mVel.y = -mVel.y;
        mPhase = 0;
    }
    VEC_Add(&mPos, &mVel, &mPos);
    CheckHitB();
}

/* ---- Heyho::StartMotionBounceNear ---- */
void Heyho::StartMotionBounceNear() {
    mLean = 0;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mMotion = pmf_Heyho_MotionBounceNear;
}

/* ---- Heyho::MotionBounceNear ---- */
void Heyho::MotionBounceNear() {
    int goalY = mPath.goal.y;
    int posY = mPos.y;
    if (posY < goalY && mVel.y < 0) {
        mVel.y = -mVel.y;
        mPhase = 0;
    } else if (posY > goalY + 0x60000 && mVel.y > 0) {
        mVel.y = -mVel.y;
        mPhase = 0;
    }

    int goalX = mPath.goal.x;
    int posX = mPos.x;
    if (posX < goalX - 0x60000 && mVel.x < 0) {
        mVel.x = -mVel.x;
        mPhase = 0;
    } else if (posX > goalX + 0x60000 && mVel.x > 0) {
        mVel.x = -mVel.x;
        mPhase = 0;
    }

    VEC_Add(&mPos, &mVel, &mPos);
    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    CheckHitB();
}

/* ---- Heyho::StartMotionVShape ---- */
void Heyho::StartMotionVShape() {
    Vector3 v0, v1, v2, d0, d1, d2, diff;
    int len0, len1, len2;

    mLean = 0;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mSpeed = VEC_Mag(&mVel);

    {
        int y = mPath.range.top;
        int x = mPath.range.left;
        v0.x = x;
        v0.y = y;
        v0.z = 0;
    }
    {
        int y = mPath.range.top;
        int x = mPath.range.right;
        v1.x = x;
        v1.y = y;
        v1.z = 0;
    }
    {
        int a = mPath.range.left;
        int b = mPath.range.right;
        int c = mPath.range.bottom;
        v2.x = (a + b) >> 1;
        v2.y = c;
        v2.z = 0;
    }

    Vec3_Sub(&d0, &v0, &mPos);
    len0 = VEC_Mag(&d0);
    Vec3_Sub(&d1, &v1, &mPos);
    len1 = VEC_Mag(&d1);
    Vec3_Sub(&d2, &v2, &mPos);
    len2 = VEC_Mag(&d2);

    if (len2 < len1 && len2 < len0) {
        mWaypoint.x = v2.x;
        mWaypoint.y = v2.y;
        mWaypoint.z = v2.z;
    } else if (len1 < len0) {
        mWaypoint.x = v1.x;
        mWaypoint.y = v1.y;
        mWaypoint.z = v1.z;
    } else {
        mWaypoint.x = v0.x;
        mWaypoint.y = v0.y;
        mWaypoint.z = v0.z;
    }

    Vec3_Sub(&diff, &mWaypoint, &mPos);
    mVel.x = diff.x;
    mVel.y = diff.y;
    mVel.z = diff.z;
    if (NormalizeVec3IfNonZero(&mVel) == 0) {
        mVel.x = mPath.startVel.x;
        mVel.y = mPath.startVel.y;
        mVel.z = mPath.startVel.z;
    } else {
        Vec3_MulScalarInPlace(&mVel, mSpeed);
    }

    mMotion = pmf_Heyho_MotionVShape;
}

/* ---- Heyho::MotionVShape ---- */
void Heyho::MotionVShape() {
    Vector3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = mVel.x;
    r5 = ApproachLinear(mPos.x, mWaypoint.x, t < 0 ? -t : t);
    t = mVel.y;
    r0v = ApproachLinear(mPos.y, mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.bottom) {
        int c_a8 = mVel.x;
        if (c_a8 > 0) {
            mWaypoint.x = mPath.range.right;
            mWaypoint.y = mPath.range.top;
        } else {
            mWaypoint.x = mPath.range.left;
            mWaypoint.y = mPath.range.top;
        }
        Vec3_Sub(&sp0, &mWaypoint, &mPos);
        mVel.x = sp0.x;
        mVel.y = sp0.y;
        mVel.z = sp0.z;
        func_0203ce80(&spC, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.top) {
        mWaypoint.x = (mPath.range.left + mPath.range.right) >> 1;
        mWaypoint.y = mPath.range.bottom;
        Vec3_Sub(&sp18, &mWaypoint, &mPos);
        mVel.x = sp18.x;
        mVel.y = sp18.y;
        mVel.z = sp18.z;
        func_0203ce80(&sp24, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    }

    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);

    CheckHitB();
}

/* ---- Heyho::StartMotionBounceInRange ---- */
void Heyho::StartMotionBounceInRange() {
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mMotion = pmf_Heyho_MotionBounceInRange;
}

/* ---- Heyho::MotionBounceInRange ---- */
void Heyho::MotionBounceInRange() {
    int vx = mVel.x;
    if (vx < 0 && mPos.x < mPath.goal.x + mPath.range.left) {
        mVel.x = -vx;
    } else if (vx > 0 && mPos.x > mPath.goal.x + mPath.range.right) {
        mVel.x = -vx;
    }
    int vy = mVel.y;
    if (vy < 0 && mPos.y < mPath.goal.y + mPath.range.bottom) {
        mVel.y = -vy;
    } else if (vy > 0 && mPos.y > mPath.goal.y + mPath.range.top) {
        mVel.y = -vy;
    }
    VEC_Add(&mPos, &mVel, &mPos);
    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    CheckHitB();
}

/* ---- Heyho::StartMotionDiagonal ---- */
void Heyho::StartMotionDiagonal() {
    Vector3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mSpeed = VEC_Mag(&mVel);
    mLean = 0;
    ay = mPath.range.top;
    ax = mPath.range.left;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = mPath.range.bottom;
    bx = mPath.range.right;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &mPos);
    la = VEC_Mag(&da);
    Vec3_Sub(&db, &b, &mPos);
    if (VEC_Mag(&db) < la) {
        mWaypoint.x = a.x;
        mWaypoint.y = a.y;
        mWaypoint.z = a.z;
    } else {
        mWaypoint.x = b.x;
        mWaypoint.y = b.y;
        mWaypoint.z = b.z;
    }
    Vec3_Sub(&t, &mWaypoint, &mPos);
    mVel.x = t.x;
    mVel.y = t.y;
    mVel.z = t.z;
    if (NormalizeVec3IfNonZero(&mVel) == 0) {
        mVel.x = mPath.startVel.x;
        mVel.y = mPath.startVel.y;
        mVel.z = mPath.startVel.z;
    } else {
        Vec3_MulScalarInPlace(&mVel, mSpeed);
    }
    mMotion = pmf_Heyho_MotionDiagonal;
}

/* ---- Heyho::MotionDiagonal ---- */
void Heyho::MotionDiagonal() {
    Vector3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = mVel.x;
    r5 = ApproachLinear(mPos.x, mWaypoint.x, t < 0 ? -t : t);
    t = mVel.y;
    r0v = ApproachLinear(mPos.y, mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.bottom) {
        mWaypoint.x = mPath.range.left;
        mWaypoint.y = mPath.range.top;
        Vec3_Sub(&sp0, &mWaypoint, &mPos);
        mVel.x = sp0.x;
        mVel.y = sp0.y;
        mVel.z = sp0.z;
        func_0203ce80(&spC, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.top) {
        mWaypoint.x = mPath.range.right;
        mWaypoint.y = mPath.range.bottom;
        Vec3_Sub(&sp18, &mWaypoint, &mPos);
        mVel.x = sp18.x;
        mVel.y = sp18.y;
        mVel.z = sp18.z;
        func_0203ce80(&sp24, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    }

    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);

    CheckHitB();
}

/* ---- Heyho::StartMotionDiagonalMirror ---- */
void Heyho::StartMotionDiagonalMirror() {
    Vector3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mSpeed = VEC_Mag(&mVel);
    mLean = 0;
    ay = mPath.range.top;
    ax = mPath.range.right;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = mPath.range.bottom;
    bx = mPath.range.left;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &mPos);
    la = VEC_Mag(&da);
    Vec3_Sub(&db, &b, &mPos);
    if (VEC_Mag(&db) < la) {
        mWaypoint.x = a.x;
        mWaypoint.y = a.y;
        mWaypoint.z = a.z;
    } else {
        mWaypoint.x = b.x;
        mWaypoint.y = b.y;
        mWaypoint.z = b.z;
    }
    Vec3_Sub(&t, &mWaypoint, &mPos);
    mVel.x = t.x;
    mVel.y = t.y;
    mVel.z = t.z;
    if (NormalizeVec3IfNonZero(&mVel) == 0) {
        mVel.x = mPath.startVel.x;
        mVel.y = mPath.startVel.y;
        mVel.z = mPath.startVel.z;
    } else {
        Vec3_MulScalarInPlace(&mVel, mSpeed);
    }
    mMotion = pmf_Heyho_MotionDiagonalMirror;
}

/* ---- Heyho::MotionDiagonalMirror ---- */
void Heyho::MotionDiagonalMirror() {
    Vector3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = mVel.x;
    r5 = ApproachLinear(mPos.x, mWaypoint.x, t < 0 ? -t : t);
    t = mVel.y;
    r0v = ApproachLinear(mPos.y, mWaypoint.y, t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.bottom) {
        mWaypoint.x = mPath.range.right;
        mWaypoint.y = mPath.range.top;
        Vec3_Sub(&sp0, &mWaypoint, &mPos);
        mVel.x = sp0.x;
        mVel.y = sp0.y;
        mVel.z = sp0.z;
        func_0203ce80(&spC, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    } else if (r5 != 0 && r0v != 0 && mWaypoint.y == mPath.range.top) {
        mWaypoint.x = mPath.range.left;
        mWaypoint.y = mPath.range.bottom;
        Vec3_Sub(&sp18, &mWaypoint, &mPos);
        mVel.x = sp18.x;
        mVel.y = sp18.y;
        mVel.z = sp18.z;
        func_0203ce80(&sp24, &mVel);
        Vec3_MulScalarInPlace(&mVel, mSpeed);
        mPhase = 0;
    }

    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);

    CheckHitB();
}

/* ---- Heyho::StartMotionCircleUpper ---- */
void Heyho::StartMotionCircleUpper() {
    mLean = 0;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mMotion = pmf_Heyho_MotionCircleUpper;
}

/* ---- Heyho::MotionCircleUpper ---- */
void Heyho::MotionCircleUpper() {
    int vx = mVel.x;
    if (vx < 0 && mPos.x < -0x68000) {
        mVel.y = -vx;
        mVel.x = 0;
    } else if (vx > 0 && mPos.x > 0x68000) {
        mVel.y = -vx;
        mVel.x = 0;
    }
    int vy = mVel.y;
    if (vy < 0 && mPos.y < 0x20000) {
        mVel.x = vy;
        mVel.y = 0;
    } else if (vy > 0 && mPos.y > 0xc0000) {
        mVel.x = vy;
        mVel.y = 0;
    }
    VEC_Add(&mPos, &mVel, &mPos);
    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    CheckHitB();
}

/* ---- Heyho::StartMotionCircleLower ---- */
void Heyho::StartMotionCircleLower() {
    mLean = 0;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mMotion = pmf_Heyho_MotionCircleLower;
}

/* ---- Heyho::MotionCircleLower ---- */
void Heyho::MotionCircleLower() {
    int vx = mVel.x;
    if (vx < 0 && mPos.x < -0x68000) {
        mVel.y = vx;
        mVel.x = 0;
    } else if (vx > 0 && mPos.x > 0x68000) {
        mVel.y = vx;
        mVel.x = 0;
    }
    int vy = mVel.y;
    if (vy < 0 && mPos.y < -0xc8000) {
        mVel.x = -vy;
        mVel.y = 0;
    } else if (vy > 0 && mPos.y > -0x20000) {
        mVel.x = -vy;
        mVel.y = 0;
    }
    VEC_Add(&mPos, &mVel, &mPos);
    if (mVel.x > 0)
        ApproachLinear(mLean, 0x3000, 0x200);
    else
        ApproachLinear(mLean, -0x3000, 0x200);
    CheckHitB();
}

/* ---- Heyho::StartMotionChase ---- */
void Heyho::StartMotionChase() {
    mLean = 0;
    mDelay = HEYHO_RAND(45) + 15;
    mWaypoint.x = mPos.x;
    mWaypoint.y = mPos.y;
    mWaypoint.z = mPos.z;
    mVel.x = mPath.startVel.x;
    mVel.y = mPath.startVel.y;
    mVel.z = mPath.startVel.z;
    mMotion = pmf_Heyho_MotionChase;
}

/* ---- Heyho::MotionChase ---- */
void Heyho::MotionChase() {
    dMg3DHeyhoObjAdapter_c *best;
    Vector3 bp;
    Vector3 op;
    Vector3 delta;
    Vector3 nrm;
    Vector3 cand;
    Vector3 dir;
    Vector3 diff;
    Vector3 scaled;
    Vector3 proj;
    Vector3 sc2;
    Vector3 tgt;
    Vector3 vel;
    int bm, i, ra, rb;

    if (mSeparated != 0) {
        ApproachLinear(mWaypoint.x, mPos.x, 0x1000);
        ApproachLinear(mWaypoint.y, mPos.y, 0x1000);
    }
    {
        int t = mVel.x;
        if (t < 0) t = -t;
        ra = ApproachLinear(mPos.x, mWaypoint.x, t);
    }
    {
        int t = mVel.y;
        if (t < 0) t = -t;
        rb = ApproachLinear(mPos.y, mWaypoint.y, t);
    }
    if (ra != 0)
        mVel.x = 0;
    if (rb != 0)
        mVel.y = 0;
    if (ra != 0 && rb != 0 && ApproachLinear(mLean, 0, 0x200) != 0 &&
        ApproachLinear2(mDelay, 0, 1) != 0) {
        best = 0;
        bm = 0x20000;
        for (i = 0; i < 3; i++) {
            if (JumpAdapter(i)->IsLaunched() != 0) {
                int dot;
                op = *(const Vector3 *)JumpAdapter(i)->GetPos();
                Vec3_Sub(&delta, &mPos, &op);
                nrm = *(const Vector3 *)JumpAdapter(i)->GetVelocity();
                if (NormalizeVec3IfNonZero(&nrm) != 0) {
                    dot = VEC_DotProduct(&nrm, &delta);
                    if (dot > 0) {
                        int d;
                        Vec3_MulScalar(&scaled, &nrm, dot);
                        Vec3_Add(&proj, &op, &scaled);
                        cand.x = proj.x;
                        cand.y = proj.y;
                        cand.z = proj.z;
                        d = Vec3_Dist(&mPos, &cand);
                        if (d < bm) {
                            best = JumpAdapter(i);
                            bp = cand;
                            bm = d;
                        }
                    }
                }
            }
        }
        if (best != 0) {
            Vec3_Sub(&dir, &mPos, &bp);
            if (NormalizeVec3IfNonZero(&dir) == 0) {
                dir.x = 0x1000; dir.y = 0; dir.z = 0;
            }
            Vec3_MulScalar(&sc2, &dir, 0x40000);
            Vec3_Add(&tgt, &mPos, &sc2);
            mWaypoint.x = tgt.x;
            mWaypoint.y = tgt.y;
            mWaypoint.z = tgt.z;
            {
                int v2 = mWaypoint.x;
                if (v2 < -0x68000) v2 = -0x68000;
                else if (v2 > 0x68000) v2 = 0x68000;
                mWaypoint.x = v2;
            }
            {
                int v3 = mWaypoint.y;
                if (v3 < -0xa8000) v3 = -0xa8000;
                else if (v3 > 0xa0000) v3 = 0xa0000;
                mWaypoint.y = v3;
            }
            {
                int len;
                Vec3_Sub(&diff, &mWaypoint, &mPos);
                len = VEC_Mag((const Vector3 *)&mPath.startVel);
                if (NormalizeVec3IfNonZero(&diff) != 0) {
                    Vec3_MulScalar(&vel, &diff, len);
                    mVel.x = vel.x;
                    mVel.y = vel.y;
                    mVel.z = vel.z;
                } else {
                    int w = FX_MUL(len, 0xb50);
                    mVel.x = w;
                    mVel.y = w;
                }
            }
            mDelay = HEYHO_RAND(45) + 15;
        }
    } else {
        int df = mWaypoint.x - mPos.x;
        if (df > 0)
            ApproachLinear(mLean, 0x3000, 0x200);
        else if (df < 0)
            ApproachLinear(mLean, -0x3000, 0x200);
    }
    CheckHitB();
}

/* ---- Heyho::StartMotionHitSpin ---- */
void Heyho::StartMotionHitSpin() {
    mPhase = 0;
    D3D_PlaySeAtX(0x1c8, mPos.x);
    mMotion = pmf_Heyho_MotionHitSpin;
}

/* ---- Heyho::MotionHitSpin ---- */
void Heyho::MotionHitSpin() {
    if (mPhase < 0x4000) {
        int ang, r0;
        mPhase += 0x400;
        ang = (u16)mPhase >> 4;
        r0 = FX_MUL((FX_SinIdx(ang) >> 1) + 0x1000, 0x1400);
        mScale.x = r0;
        mScale.y = FX_MUL((FX_CosIdx(ang) >> 1) + 0x800, 0x1400);
        mScale.z = r0;
        return;
    }
    D3D_PlaySeAtX(0x1c7, mPos.x);
    Settle();
}

/* ---- Heyho::Settle ---- */
void Heyho::Settle() {
    Particle::System *a;
    Particle::System *b;
    int v0, v1;
    int t;

    a = (Particle::System *)Particle::System::NewSimple(kPtclPoofDustA, mPos.x << 3, mPos.y << 3,
                                                        mPos.z << 3);
    b = (Particle::System *)Particle::System::NewSimple(kPtclPoofDustB, mPos.x << 3, mPos.y << 3,
                                                        mPos.z << 3);
    t = (int)(mSize << 0x1c) >> 0x10;
    /* Both stores go through a; the cartridge never sets b's scale. */
    if (a)
        a->callbackScale = t;
    if (b)
        a->callbackScale = t;

    ApproachLinear(sHeyhoUnsettledCount, 0, 1);
    ApproachLinear(sJumpComboCount, 0x270f, 1);

    if (sHeyhoScorePopups != 0) {
        if (mPos.y > 0) {
            Camera_ProjectToScreen(sMgSubCamera, &mPos, &v0, &v1);
            v1 = v1 - (Mg_GetTopScreenYOffset() + 0xc0);
        } else {
            Camera_ProjectToScreen(sMgMainCamera, &mPos, &v0, &v1);
        }
        v1 = v1 - 0x20;
        Jump2Popup_Spawn(v0 << 0xc, v1 << 0xc, (short)sJumpComboCount);
    }

    SetIdle();
}

/* ---- Heyho::SetIdle ---- */
void Heyho::SetIdle() {
    mMotion = pmf_Heyho_StateIdle_2;
}

/* ---- Heyho::StateIdle ---- */
void Heyho::StateIdle() {}

/* ---- Heyho::Update ---- */
void Heyho::Update() {
    (this->*mMotion)();
    mAnmModel.Advance();
    UpdateMatrix();
}

/* ---- Heyho::Draw ---- */
void Heyho::Draw() {
    Vector3 v;
    if (mMotion == pmf_Heyho_StateIdle_3)
        return;
    Vec3_MulScalar(&v, &mScale, mSize);
    mAnmModel.Render(&v);
}

/* ---- Heyho::UpdateMatrix ---- */
void Heyho::UpdateMatrix() {
    MtxStack::transS(mPos.x, mPos.y, mPos.z);
    MtxStack::YrotM(mLean);
    mAnmModel.mat4x3 = MtxStack::get();
}

/* ---- Heyho::CheckHitB ---- */
void Heyho::CheckHitB() {
    int i;
    int lim = FX_MUL(mSize, 0x12000);
    int half = lim >> 1;

    for (i = 0; i < 3; i++) {
        Vector3 a;
        Vector3 b;
        Vector3 d;
        Vector3 c;

        if (JumpAdapter(i)->IsLaunched() == 0)
            continue;

        Vec3_Sub(&a, (const Vector3 *)JumpAdapter(i)->GetPos(), &mPos);
        a.y -= half;
        if (a.y < 0) {
            if (a.y > -0x18000)
                a.y = 0;
            else
                a.y += 0x18000;
        } else {
            if (a.x > 0x2000)
                a.x -= 0x2000;
            else if (a.x < -0x2000)
                a.x += 0x2000;
            else
                a.x = 0;
        }

        Vec3_Sub(&b, (const Vector3 *)JumpAdapter(i)->GetVelocity(), &mVel);
        Vec3_Add(&c, &a, &b);
        if (VEC_Mag(&c) > lim)
            continue;

        if (NormalizeVec3IfNonZero(&a) == 0) {
            JumpAdapter(i)->mCommand = 1;
            SetAnchor(JumpAdapter(i), mPos);
            StartMotionHitSpin();
        }

        d.x = 0;
        d.y = 0x1000;
        d.z = 0;
        if (VEC_DotProduct(&d, &a) > 0) {
            JumpAdapter(i)->mCommand = 1;
            SetAnchor(JumpAdapter(i), mPos);
            StartMotionHitSpin();
            return;
        } else {
            JumpAdapter(i)->mCommand = 2;
            SetAnchor(JumpAdapter(i), mPos);
        }
    }
}

/* ---- Heyho::CheckHitA ---- */
void Heyho::CheckHitA() {
    int i;
    int thresh = FX_MUL(mSize, 0x12000);

    for (i = 0; i < 3; i++) {
        Vector3 disp, rel, up, sum;

        if (JumpAdapter(i)->IsLaunched() == 0)
            continue;

        Vec3_Sub(&disp, (const Vector3 *)JumpAdapter(i)->GetPos(), &mPos);

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

        Vec3_Sub(&rel, (const Vector3 *)JumpAdapter(i)->GetVelocity(), &mVel);
        Vec3_Add(&sum, &disp, &rel);

        if (VEC_Mag(&sum) > thresh)
            continue;

        if (NormalizeVec3IfNonZero(&disp) == 0) {
            JumpAdapter(i)->mCommand = 1;
            SetAnchor(JumpAdapter(i), mPos);
            StartMotionHitSpin();
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (VEC_DotProduct(&up, &disp) <= 0)
            continue;

        if (rel.y >= 0)
            continue;

        JumpAdapter(i)->mCommand = 1;
        SetAnchor(JumpAdapter(i), mPos);
        StartMotionHitSpin();
        return;
    }
}

/* ---- Heyho::IsChasing ---- */
/* 1 unless the Heyho is leaving, or chasing while still leaning into a turn. */
int Heyho::IsChasing() {
    return mMotion != pmf_Heyho_MotionLeave && (mMotion != pmf_Heyho_MotionChase_2 || mLean == 0);
}

/* ---- Heyho_InitPool ---- */
int Heyho_InitPool(void *heyhos, int count) {
    int i;
    sHeyhoScorePopups = 0;
    sHeyhoCount = count;
    sHeyhoUnsettledCount = count;
    sHeyhos = (Heyho *)heyhos;
    for (i = 0; i < sHeyhoCount; i++) {
        if (sHeyhos[i].LoadResources() == 0)
            return 0;
    }
    return 1;
}

/* ---- Heyho_SpawnAll ---- */
void Heyho_SpawnAll(void *arg0, int layout) {
    int i;
    sHeyhoLayout = layout;
    data_ov006_021402fc = arg0;
    sHeyhoUnsettledCount = sHeyhoCount;
    sJumpComboCount = 0;
    for (i = 0; i < sHeyhoCount; i++) {
        if (i < 3) {
            sHeyhos[i].mPhase = 0;
            sHeyhos[i].StartMotionFlyIn(&sHeyhoLayoutFlyInPaths[sHeyhoLayout][i]);
            sHeyhos[i].UpdateMatrix();
        } else {
            int row = HEYHO_RAND(4);
            int col = HEYHO_RAND(4);
            int j;
            for (j = 0; j < i - 3; j++) {
                if (row == data_ov006_021403b8[j]) {
                    row = HEYHO_RAND(4);
                    j = 0;
                }
            }
            data_ov006_021403b8[i - 3] = row;
            sHeyhos[i].mPhase = 0;
            sHeyhos[i].StartMotionFlyIn(&sHeyhoFlyInPaths[row][col]);
            sHeyhos[i].UpdateMatrix();
        }
    }
}

/* ---- Heyho_SeparateAll ---- */
void Heyho_SeparateAll(void) {
    Vector3 v1;
    Vector3 v2;
    Heyho *oi;
    Heyho *oj;
    Vector3 *pi;
    Vector3 *pj;
    int i, j, k, t;
    Vector3 *ai;

    for (i = 0; i < sHeyhoCount; i++) {
        sHeyhos[i].Update();
    }
    for (k = 0; k < sHeyhoCount; k++) {
        sHeyhos[k].mSeparated = 0;
    }
    for (t = 0; t < sHeyhoCount; t++) {
        if (sHeyhos[t].IsChasing() != 0) {
            oi = &sHeyhos[t];
            pi = &oi->mPos;
            ai = &oi->mVel;
            for (j = t + 1; j < sHeyhoCount; j++) {
                if (sHeyhos[j].IsChasing() != 0) {
                    /* The long long widening is load-bearing (re-measured 2026-11-11):
                       dropping it costs Heyho_SeparateAll 15 words. */
                    oj = (Heyho *)((long long)(int)&sHeyhos[j]);
                    pj = &oj->mPos;
                    Vec3_Sub(&v1, pi, pj);
                    Vec3_Sub(&v2, ai, &oj->mVel);
                    if (VEC_Mag(&v1) >= 0x12000)
                        continue;
                    if (VEC_Mag(&v2) >= 0x100)
                        continue;
                    if (NormalizeVec3IfNonZero(&v1) == 0) continue;
                    VEC_Add(pi, &v1, pi);
                    VEC_Subtract(pj, &v1, pj);
                    sHeyhos[t].mSeparated = 1;
                    sHeyhos[j].mSeparated = 1;
                }
            }
        }
    }
}

/* ---- Heyho_DrawAll ---- */
void Heyho_DrawAll(void) {
    int i;
    for (i = 0; i < sHeyhoCount; i++)
        sHeyhos[i].Draw();
}

/* ---- Heyho_SettleAll ---- */
void Heyho_SettleAll(void) {
    int i;
    for (i = 0; i < sHeyhoCount; i++) {
        if (sHeyhos[i].mMotion == data_ov006_0213afc8) {
            sHeyhos[i].StartMotionRegroup(&sHeyhoSettlePaths[HEYHO_RAND(9)][HEYHO_RAND(5)]);
            ApproachLinear(sHeyhoUnsettledCount, sHeyhoCount, 1);
            return;
        }
    }
}

/* ---- Heyho_ReactAll ---- */
void Heyho_ReactAll(void) {
    int i;
    for (i = 0; i < sHeyhoCount; i++)
        sHeyhos[i].React();
}

/* ---- Heyho_AllIdle ---- */
int Heyho_AllIdle(void) {
    int i;
    for (i = 0; i < sHeyhoCount; i++) {
        if (sHeyhos[i].mMotion != pmf_Heyho_StateIdle_4)
            return 0;
    }
    return 1;
}

dMg3DHeyhoObjAdapter_c::dMg3DHeyhoObjAdapter_c() : mCommand(0)
{
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the reverse-emitted .text run. Construction order is
 * the model handle, then the animation handle. The plain counters have
 * no initializer: one would grow the sinit. */

int sHeyhoUnsettledCount;

int sJumpComboCount;

int sHeyhoScorePopups;

int sHeyhoLayout;

Heyho *sHeyhos;

int sHeyhoCount;

int data_ov006_021403b8[3];

char *sJumpAdapters[3];

HeyhoAdapterModelFilePtr gMgPHeyhoMiniModelFile(0x217);
HeyhoAdapterAnimationFileHandle gMgPHeyhoWaitAnimFile(0x218);
