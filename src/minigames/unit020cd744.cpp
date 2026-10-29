//cpp
/* ov006 stylus-drawn trampolines, 0x020cd744..0x020d1018.
 *
 * The run sits between dMgTrmpln2Mario_c and dScMgAmida_c. No RTTI
 * in it. A line slot is a 4x4 patch: positions at +0x5c, normals at
 * +0x1dc, packed normals at +0x2dc. Four slots of 0x32c bytes sit at
 * sTrmpLines. defer_codegen off keeps ROM order. The file owns its
 * static initializer and its .bss: the line slots, the three ring
 * model handles, the counters and the function-local unit vectors.
 *
 * leftovers:
 * - Address names stay. Each function keeps its namespace, because
 *   the local structs and externs disagree.
 * - TrmpItem_MotionSway keeps opt_propagation off.
 *   TrmpLine_ComputeNormals keeps opt_strength_reduction off.
 *   TrmpLine_PushJumper keeps opt_common_subs off and opt_propagation off.
 * - Vector3 locals emit an empty destructor, licensed as a deadstrip
 *   duplicate of the arm9 copy. Spare4 in TrmpLine_Build emits
 *   another, deadstripped as compiler-only output.
 * - TrmpLine and TrmpItem describe the known named members. Remaining
 *   raw fields preserve partial reconstruction of these records. dExtSimpleModel_c
 *   and texture-animation operations use their real headers.
 */

#include "types.h"
#include "nitro/fx/fx.h"
#include "decl_common.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "TrmpLine.h"
#include "TrmpItem.h"
#include "math/Approach.h"
/* Typed access to a TrmpLine slot from the char* helpers below. */
#define TRMPLINE(p) ((TrmpLine *)(p))
/* Typed access to a TrmpItem record from the char* helpers below. */
#define TRMPITEM(p) ((TrmpItem *)(p))

#pragma defer_codegen off

#include "dExtSimpleModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "SharedFilePtr.h"

/* This unit's globals. The per-function namespaces below still re-declare
   several of them in the spelling each one-function source used. */
extern "C" {
void *sTrmpLinePlttAddr;
void *sTrmpLineTexAddr;
int sTrmpItemsHit;
int sTrmpItemsActive;
char *sTrmpItems;
int sTrmp2Score;
int sTrmpItemCount;
int sTrmp2MarioPtrs[5];
void *sTrmpJumpers[5];
}

/* The three ring model handles (files 0x21a..0x21c), defined at the end of
   the file. The wrapper name is local; its constructor and destructor are
   the ROM resource-family functions, aliased in the manifest. */
struct TrmpItemRingModelFile : SharedFilePtr {
    TrmpItemRingModelFile(u32 fileID);
    ~TrmpItemRingModelFile();
};
extern TrmpItemRingModelFile sTrmpItemRingAModelFile;
extern TrmpItemRingModelFile sTrmpItemRingBModelFile;
extern TrmpItemRingModelFile sTrmpItemRingCModelFile;

/* The four line slots, defined beside TrmpLine's constructor below. */
extern TrmpLine sTrmpLines[4];

// Partial callback interface only. ROM vtables at 0x0213b2c4/0x0213b2e0
// implement these five slots through dMgTrmpObjAdapter_c and its children.
// No destructor slots or complete actor layout are implied by this view.
namespace cstd {
int fdiv(s32, s32);
}
namespace {
struct TrampolineObjectCallbacks {
    virtual Vector3 *GetPos();
    virtual Vector3 *GetPrevPos();
    virtual Vector3 *GetVelocity();
    virtual int IsActive();
    virtual void Clear();
};

// The two item counters on the jumper, named as in dMgTrmpln2Mario_c.h.
struct TrampolineJumperCounts : TrampolineObjectCallbacks {
    char pad_04[0x20 - 0x04];
    u16 mItemHitCount;  // +0x20: items hit; doubles the score
    s16 mItemPushCount; // +0x22: item-end pushes this frame
};
}

namespace s020cd744 {
extern "C" {
extern int sTrmpItemsActive;
}

extern "C" void TrmpItem_GrowScale(char *c) {
    ApproachLinear(TRMPITEM(c)->mScale.x, 0x1200, 0x80);
    ApproachLinear(TRMPITEM(c)->mScale.z, 0x1200, 0x80);
    ApproachLinear(TRMPITEM(c)->mScale.y, 0x20000, 0x400);
    if (ApproachLinear2(*(int *)&TRMPITEM(c)->mAlpha, 0, 1) == 0)
        return;
    TRMPITEM(c)->mActive = 0;
    ApproachLinear(sTrmpItemsActive, 0, 1);
}
}

namespace s020cd7b8 {
extern "C" extern void Camera_ProjectToScreen(void *a, void *b, int *c, int *d);
extern "C" extern int Mg_GetTopScreenYOffset(void);
extern "C" extern void Jump2Popup_Spawn(int a, int b, int c);
extern "C" {
extern void *sMgSubCamera;
}
extern "C" {
extern void *sMgMainCamera;
}
struct P2 {
    int w[2];
};
extern "C" {
extern struct P2 pmf_TrmpItem_GrowScale;
}

extern "C" void TrmpItem_ProjectToScreen(char *c, int scoreValue) {
    int screenX, screenY;
    if (TRMPITEM(c)->mPos.y > 0) {
        Camera_ProjectToScreen(sMgSubCamera, &TRMPITEM(c)->mPos, &screenX, &screenY);
        screenY = screenY - (Mg_GetTopScreenYOffset() + 0xc0);
    } else {
        Camera_ProjectToScreen(sMgMainCamera, &TRMPITEM(c)->mPos, &screenX, &screenY);
    }
    Jump2Popup_Spawn(screenX << 0xc, screenY << 0xc, (short)scoreValue);
    *(struct P2 *)c = pmf_TrmpItem_GrowScale;
}
}

namespace s020cd864 {
// @symbol TrmpItem_MotionSpinHover
extern "C" void TrmpItem_MotionSpinHover(char *arg) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28((int *)&TRMPITEM(arg)->mForward, 0x100);
    func_0203ce80(&normalCopy, &TRMPITEM(arg)->mForward);
    func_0203cf00(&tangent, &TRMPITEM(arg)->mForward, &sUnitZ);
    TRMPITEM(arg)->mSide.x = tangent.x;
    TRMPITEM(arg)->mSide.y = tangent.y;
    TRMPITEM(arg)->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &TRMPITEM(arg)->mSide);
    Quaternion_FromVector3(TRMPITEM(arg)->mRotation, &sUnitY, &TRMPITEM(arg)->mForward);
    Quaternion_Normalize(TRMPITEM(arg)->mRotation);
    TrmpItem_MotionHover(arg);
}
}

namespace s020cd98c {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionSpinHover;
}
extern "C" void TrmpItem_SetMotionSpinHover(int *c) {
    TRMPITEM(c)->mPhase = 0;
    *(struct S *)c = pmf_TrmpItem_MotionSpinHover;
}
}

namespace s020cd9b0 {
typedef struct {
    int x, y, z;
} Vec3;
extern "C" extern void func_0203cc28(int *p, int angle);
extern "C" extern void func_0203ce80(Vec3 *dst, Vec3 *src);
extern "C" extern void func_0203cf00(Vec3 *out, Vec3 *a, Vec3 *b);
extern "C" extern void Quaternion_FromVector3(int *q, Vec3 *axis, Vec3 *v);
extern "C" extern void Quaternion_Normalize(int *q);

extern "C" void TrmpItem_MotionSpinDrift(char *self) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    Vec3 normalCopy, tangent, tangentCopy, unused;

    func_0203cc28((int *)&TRMPITEM(self)->mForward, 0x100);
    func_0203ce80(&normalCopy, (Vec3 *)&TRMPITEM(self)->mForward);
    func_0203cf00(&tangent, (Vec3 *)&TRMPITEM(self)->mForward, (Vec3 *)&sUnitZ);
    TRMPITEM(self)->mSide.x = tangent.x;
    TRMPITEM(self)->mSide.y = tangent.y;
    TRMPITEM(self)->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, (Vec3 *)&TRMPITEM(self)->mSide);
    Quaternion_FromVector3(TRMPITEM(self)->mRotation, (Vec3 *)&sUnitY,
                           (Vec3 *)&TRMPITEM(self)->mForward);
    Quaternion_Normalize(TRMPITEM(self)->mRotation);
}
}

namespace s020cdad0 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionSpinDrift;
}
extern "C" void TrmpItem_SetMotionSpinDrift(char *p) {
    *(struct S *)TRMPITEM(p)->mState = pmf_TrmpItem_MotionSpinDrift;
}
}

namespace s020cdaec {
// @symbol TrmpItem_MotionSpinSway
extern "C" void TrmpItem_MotionSpinSway(char *arg) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28((int *)&TRMPITEM(arg)->mForward, 0x100);
    func_0203ce80(&normalCopy, &TRMPITEM(arg)->mForward);
    func_0203cf00(&tangent, &TRMPITEM(arg)->mForward, &sUnitZ);
    TRMPITEM(arg)->mSide.x = tangent.x;
    TRMPITEM(arg)->mSide.y = tangent.y;
    TRMPITEM(arg)->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &TRMPITEM(arg)->mSide);
    Quaternion_FromVector3(TRMPITEM(arg)->mRotation, &sUnitY, &TRMPITEM(arg)->mForward);
    Quaternion_Normalize(TRMPITEM(arg)->mRotation);
    TrmpItem_MotionSway(arg);
}
}

namespace s020cdc14 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionSpinSway;
}
extern "C" void TrmpItem_SetMotionSpinSway(char *c) {
    TRMPITEM(c)->mPhase = 0x2000;
    *(struct S *)c = pmf_TrmpItem_MotionSpinSway;
}
}

namespace s020cdc38 {
extern "C" void TrmpItem_MotionHover(void *arg0) {
    TRMPITEM(arg0)->mPhase += 0x100;
    if (TRMPITEM(arg0)->mPhase & 0x8000) {
        TRMPITEM(arg0)->mVel.y = 0xc00;
    } else {
        TRMPITEM(arg0)->mVel.y = -0xc00;
    }
}
}

namespace s020cdc68 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionHover;
}
extern "C" void TrmpItem_SetMotionHover(char *c) {
    TRMPITEM(c)->mPhase = 0;
    *(struct S *)c = pmf_TrmpItem_MotionHover;
}
}

namespace s020cdc8c {

#pragma push
#pragma opt_propagation off
extern "C" void TrmpItem_MotionSway(char *self) {
    int distanceFromCenterY;
    int xSpeed;
    int minimumSpeed = 0x400;
    u16 *phase = &TRMPITEM(self)->mPhase;

    *phase = *phase + 0x40;

    distanceFromCenterY = TRMPITEM(self)->mPos.y - 0x74000;
    if (distanceFromCenterY < 0)
        distanceFromCenterY = -distanceFromCenterY;
    xSpeed = (cstd::fdiv(distanceFromCenterY, 0x4c000) >> 3) + minimumSpeed;

    if (TRMPITEM(self)->mPhase & 0x4000)
        TRMPITEM(self)->mVel.x = xSpeed;
    else
        TRMPITEM(self)->mVel.x = -xSpeed;
}
#pragma pop
}

namespace s020cdce4 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionSway;
}
extern "C" void TrmpItem_SetMotionSway(char *c) {
    TRMPITEM(c)->mPhase = 0x2000;
    *(struct S *)c = pmf_TrmpItem_MotionSway;
}
}

namespace s020cdd08 {
typedef struct {
    int x, y, z;
} Vec3;
extern "C" extern void func_0203cc28(int *p, int angle);
extern "C" extern void func_0203ce80(Vec3 *dst, Vec3 *src);
extern "C" extern void func_0203cf00(Vec3 *out, Vec3 *a, Vec3 *b);
extern "C" extern void Quaternion_FromVector3(int *q, Vec3 *axis, Vec3 *v);
extern "C" extern void Quaternion_Normalize(int *q);

extern "C" void TrmpItem_MotionSpin(char *self) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    Vec3 normalCopy, tangent, tangentCopy, d;

    func_0203cc28((int *)&TRMPITEM(self)->mForward, 0x100);
    func_0203ce80(&normalCopy, (Vec3 *)&TRMPITEM(self)->mForward);
    func_0203cf00(&tangent, (Vec3 *)&TRMPITEM(self)->mForward, (Vec3 *)&sUnitZ);
    TRMPITEM(self)->mSide.x = tangent.x;
    TRMPITEM(self)->mSide.y = tangent.y;
    TRMPITEM(self)->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, (Vec3 *)&TRMPITEM(self)->mSide);
    Quaternion_FromVector3(TRMPITEM(self)->mRotation, (Vec3 *)&sUnitY,
                           (Vec3 *)&TRMPITEM(self)->mForward);
    Quaternion_Normalize(TRMPITEM(self)->mRotation);
}
}

namespace s020cde28 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionSpin;
}
extern "C" void TrmpItem_SetMotionSpin(char *c) {
    TRMPITEM(c)->mVel.y = 0;
    *(struct S *)c = pmf_TrmpItem_MotionSpin;
}
}

namespace s020cde4c {
extern "C" void TrmpItem_MotionBob(char *item) {
    int y = TRMPITEM(item)->mPos.y;
    if (y > 0x80000) {
        TRMPITEM(item)->mVel.y = -0x1000;
        return;
    }
    if (y < -0x60000)
        TRMPITEM(item)->mVel.y = 0x1000;
}
}

namespace s020cde7c {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionBob;
}
extern "C" void TrmpItem_SetMotionBob(char *c) {
    TRMPITEM(c)->mVel.y = 0x1000;
    *(struct S *)c = pmf_TrmpItem_MotionBob;
}
}

namespace s020cdea0 {
extern "C" {
extern int sTrmpItemPatrolLimits[];
}
extern "C" {
extern int sTrmpItemPatrolSpeed[];
}
extern "C" void TrmpItem_MotionPatrol(char *item) {
    int itemType = TRMPITEM(item)->mSizeIdx;
    if (TRMPITEM(item)->mPos.x > sTrmpItemPatrolLimits[itemType]) {
        TRMPITEM(item)->mVel.x = -sTrmpItemPatrolSpeed[itemType];
        return;
    }
    if (TRMPITEM(item)->mPos.x < -sTrmpItemPatrolLimits[itemType]) {
        TRMPITEM(item)->mVel.x = sTrmpItemPatrolSpeed[itemType];
    }
}
}

namespace s020cdeec {
extern "C" {
extern int sTrmpItemPatrolSpeed[];
}
struct S2 {
    int w[2];
};
extern "C" {
extern struct S2 pmf_TrmpItem_MotionPatrol;
}
extern "C" void TrmpItem_SetMotionPatrol(char *c) {
    int itemType = TRMPITEM(c)->mSizeIdx;
    TRMPITEM(c)->mVel.x = sTrmpItemPatrolSpeed[itemType];
    *(struct S2 *)c = pmf_TrmpItem_MotionPatrol;
}
}

namespace s020cdf1c {
extern "C" void TrmpItem_MotionStill(void) {}
}

namespace s020cdf20 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpItem_MotionStill;
}
extern "C" void TrmpItem_SetMotionStill(char *p) {
    *(struct S *)TRMPITEM(p)->mState = pmf_TrmpItem_MotionStill;
}
}

namespace s020cdf3c {
/* TrmpItem_StateAppear at 0x020cdf3c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern "C" extern void TrmpItem_SetMotionPatrol(char *c);
extern "C" extern void TrmpItem_SetMotionBob(char *c);
extern "C" extern void TrmpItem_SetMotionSpin(char *c);
extern "C" extern void TrmpItem_SetMotionStill(char *c);
extern "C" extern void TrmpItem_SetMotionSway(char *c);
extern "C" extern void TrmpItem_SetMotionHover(char *c);
extern "C" extern void TrmpItem_SetMotionSpinDrift(char *c);
extern "C" extern void TrmpItem_SetMotionSpinSway(char *c);
extern "C" extern void TrmpItem_SetMotionSpinHover(char *c);

extern "C" void TrmpItem_StateAppear(char *c) {
    int yScaleReady = ApproachLinear(TRMPITEM(c)->mScale.y, 0x1000, 0xc0);
    int xScaleReady = ApproachLinear(TRMPITEM(c)->mScale.x, 0x1000, 0x180);
    int zScaleReady = ApproachLinear(TRMPITEM(c)->mScale.z, 0x1000, 0x180);
    int responseScaleReady = ApproachLinear(TRMPITEM(c)->mAppearScale, 0x800, 0x60);
    if (yScaleReady == 0)
        return;
    if (xScaleReady == 0)
        return;
    if ((zScaleReady & responseScaleReady) == 0)
        return;
    switch (TRMPITEM(c)->mMotionKind) {
    case 1:
        TrmpItem_SetMotionPatrol(c);
        break;
    case 2:
        TrmpItem_SetMotionBob(c);
        break;
    case 3:
        TrmpItem_SetMotionSpin(c);
        break;
    case 0:
        TrmpItem_SetMotionStill(c);
        break;
    case 4:
        TrmpItem_SetMotionSway(c);
        break;
    case 6:
        TrmpItem_SetMotionHover(c);
        break;
    case 8:
        TrmpItem_SetMotionSpinDrift(c);
        break;
    case 5:
        TrmpItem_SetMotionSpinSway(c);
        break;
    case 7:
        TrmpItem_SetMotionSpinHover(c);
        break;
    }
}
}

namespace s020ce0ac {
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern int sTrmpItemsActive;
}
struct S2 {
    int w[2];
};
extern "C" {
extern struct S2 pmf_TrmpItem_StateAppear_2;
}
extern "C" void TrmpItem_StartAppear(char *c) {
    ApproachLinear(sTrmpItemsActive, sTrmpItemCount, 1);
    TRMPITEM(c)->mAlpha = 0x1f;
    TRMPITEM(c)->mVel.x = 0;
    TRMPITEM(c)->mVel.y = 0;
    TRMPITEM(c)->mVel.z = 0;
    *(struct S2 *)c = pmf_TrmpItem_StateAppear_2;
}
}

namespace s020ce108 {
// @symbol TrmpItem_ReflectOffLine

extern "C" {
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(Vector3 *v);
extern int DotVec3(const Vector3 *a, const Vector3 *b);
extern int NormalizeVec3IfNonZero(Vector3 *v);
extern void Vec3_MulScalarInPlace(Vector3 *v, int s);

extern "C" {
extern Vector3 data_020a0ebc;
}
}

typedef TrampolineObjectCallbacks Obj;

extern "C" void TrmpItem_ReflectOffLine(char *item, Obj *jumper) {
    Vector3 negativeEndOffset;
    Vector3 positiveEndOffset;
    Vector3 itemToJumper;
    Vector3 pushSum;
    Vector3 halfSpan;
    Vector3 normalVelocityChange;
    Vector3 relativePosition;
    Vector3 negativeEndCorrection;
    Vector3 positiveEndCorrection;
    Vector3 endVelocityChange;
    Vector3 *position;
    int along;
    Vector3 *velocity;
    int negativeEndDistance;
    int positiveEndDistance;
    int across;

    {
        int *growState = pmf_TrmpItem_GrowScale_3;
        int stateWord = *(int *)item;
        if (stateWord == growState[0]) {
            if (TRMPITEM(item)->mState[1] == growState[1])
                return;
            if (stateWord == 0)
                return;
        }
    }

    position = jumper->GetPos();
    velocity = jumper->GetVelocity();

    pushSum = data_020a0ebc;
    Vec3_Sub(&relativePosition, position, &TRMPITEM(item)->mPos);
    positiveEndOffset = relativePosition;
    negativeEndOffset = relativePosition;
    itemToJumper = relativePosition;

    Vec3_MulScalar(&halfSpan, &TRMPITEM(item)->mSide, sTrmpItemHitWidths[TRMPITEM(item)->mSizeIdx]);
    AddVec3(&negativeEndOffset, &halfSpan, &negativeEndOffset);
    SubVec3(&positiveEndOffset, &halfSpan, &positiveEndOffset);

    negativeEndOffset.y += 0xE000;
    positiveEndOffset.y += 0xE000;
    negativeEndOffset.y >>= 1;
    positiveEndOffset.y >>= 1;

    negativeEndDistance = LenVec3(&negativeEndOffset);
    positiveEndDistance = LenVec3(&positiveEndOffset);
    along = DotVec3(&itemToJumper, &TRMPITEM(item)->mSide);
    across = DotVec3(&itemToJumper, &TRMPITEM(item)->mForward);

    if (negativeEndDistance < 0xE000 && NormalizeVec3IfNonZero(&negativeEndOffset) != 0) {
        Vec3_MulScalarInPlace(&negativeEndOffset, 0xE000 - negativeEndDistance);
        negativeEndOffset.y <<= 1;
        Vec3_MulScalar(&negativeEndCorrection, &negativeEndOffset, TRMPITEM(item)->mAppearScale);
        AddVec3(position, &negativeEndCorrection, position);
        AddVec3(&pushSum, &negativeEndOffset, &pushSum);
        ((TrampolineJumperCounts *)jumper)->mItemPushCount += 1;
    } else if (positiveEndDistance < 0xE000 && NormalizeVec3IfNonZero(&positiveEndOffset) != 0) {
        Vec3_MulScalarInPlace(&positiveEndOffset, 0xE000 - positiveEndDistance);
        positiveEndOffset.y <<= 1;
        Vec3_MulScalar(&positiveEndCorrection, &positiveEndOffset, TRMPITEM(item)->mAppearScale);
        AddVec3(position, &positiveEndCorrection, position);
        AddVec3(&pushSum, &positiveEndOffset, &pushSum);
        ((TrampolineJumperCounts *)jumper)->mItemPushCount += 1;
    }

    Vec3_MulScalar(&endVelocityChange, &pushSum, 0x100);
    AddVec3(velocity, &endVelocityChange, velocity);

    {
        int x = velocity->x;
        if (x < -0x1000)
            x = -0x1000;
        else if (x > 0x1000)
            x = 0x1000;
        velocity->x = x;
    }

    {
        int maxYVelocity = sTrmp2RiseSpeedMax[0];
        int minYVelocity = sTrmp2FallSpeed[0];
        int y = velocity->y;
        if (y >= minYVelocity) {
            if (y <= maxYVelocity)
                maxYVelocity = y;
            minYVelocity = maxYVelocity;
        }
        velocity->y = minYVelocity;
    }

    if ((across < 0 ? -across : across) >= 0xE000)
        return;

    if (along < 0)
        along = -along;
    if (along >= sTrmpItemHitWidths[TRMPITEM(item)->mSizeIdx])
        return;

    Vec3_MulScalar(&normalVelocityChange, &TRMPITEM(item)->mForward, 0x70);
    if (across > 0)
        Vec3_MulScalarInPlace(&normalVelocityChange, -0x1000);
    {
        int yChange = normalVelocityChange.y;
        if (yChange > 0x80)
            yChange = 0x80;
        normalVelocityChange.y = yChange;
    }
    AddVec3(velocity, &normalVelocityChange, velocity);
}
}

namespace s020ce46c {
extern "C" {
extern int sTrmpItemHitsOff;
}
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern char *sTrmpItems;
}
extern "C" {
extern int sTrmpItemsHit;
}
extern "C" {
extern int sTrmp2Score;
}

extern "C" extern void TrmpItem_ReflectOffLine(char *a, void *o);
extern "C" extern int TrmpItem_HitTest2D(char *a, void *o, int arg1);
extern "C" extern void D3D_PlaySeAtX(int a, int b);
extern "C" extern void Trmp2Spinner_InitAll(int c);
extern "C" extern void TrmpItem_ProjectToScreen(char *c, unsigned short arg1);

#define ELEM ((char *)sTrmpItems + itemIndex * 0x1d0)

extern "C" void TrmpItem_UpdateWithJumper(void *object, int arg1) {
    char *sb = (char *)object;
    int itemIndex;
    int doubling;
    int scoreIncrement;
    int hitCount;
    int scoreDoublings;

    if (sTrmpItemHitsOff != 0)
        return;
    ((TrampolineJumperCounts *)sb)->mItemPushCount = 0;

    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        if (TRMPITEM(ELEM)->mActive == 0)
            continue;
        TrmpItem_ReflectOffLine(ELEM, sb);
        if (TrmpItem_HitTest2D(ELEM, sb, arg1) == 0)
            continue;
        ApproachLinear(sTrmpItemsHit, 0x270f, 1);
        hitCount = ((TrampolineJumperCounts *)sb)->mItemHitCount;
        // Double the award for each prior hit, up to six doublings.
        scoreIncrement = 100;
        scoreDoublings = (hitCount > 6) ? 6 : hitCount;
        for (doubling = 0; doubling < scoreDoublings; doubling++)
            scoreIncrement <<= 1;
        switch (hitCount) {
        case 0:
            D3D_PlaySeAtX(0x131, TRMPITEM(ELEM)->mPos.x);
            break;
        case 1:
            D3D_PlaySeAtX(0x132, TRMPITEM(ELEM)->mPos.x);
            break;
        case 2:
            D3D_PlaySeAtX(0x133, TRMPITEM(ELEM)->mPos.x);
            break;
        case 3:
            D3D_PlaySeAtX(0x134, TRMPITEM(ELEM)->mPos.x);
            break;
        case 4:
        default:
            D3D_PlaySeAtX(0x135, TRMPITEM(ELEM)->mPos.x);
            break;
        }
        ApproachLinear(sTrmp2Score, 0xf423f, scoreIncrement);
        Trmp2Spinner_InitAll(((TrampolineJumperCounts *)sb)->mItemHitCount);
        TrmpItem_ProjectToScreen(ELEM, (short)scoreIncrement);
        ((TrampolineJumperCounts *)sb)->mItemHitCount++;
    }
}
}

namespace s020ce674 {
struct V2 {
    int x, y;
};

#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

extern "C" extern void TrmpItem_ProjectOnLine(char *self, void *other, struct V2 *a, struct V2 *b);
extern "C" extern void Vec2_Sub(struct V2 *o, struct V2 *a, struct V2 *b);
extern "C" extern int func_0203d524(struct V2 *a, struct V2 *b);
extern "C" {
extern int sTrmpItemHitWidths[];
}
extern "C" {
extern int pmf_TrmpItem_StateAppear[2];
}
extern "C" {
extern int pmf_TrmpItem_GrowScale_2[2];
}

// The caller supplies a third argument; this implementation does not read it.
extern "C" int TrmpItem_HitTest2D(char *item, void *jumper, int) {
    struct V2 currentProjection, previousProjection, lineStart, lineEnd;
    struct V2 testDirection, firstOffset, secondOffset;
    struct V2 movementDirection, toEnd, toStart;
    int stateWord = *(int *)item;
    int currentSide, previousSide, endSide, startSide;
    int result;

    {
        int *appearState = (int *)AT(pmf_TrmpItem_StateAppear, 0);
        int *growState;
        if ((stateWord == appearState[0] &&
             (TRMPITEM(item)->mState[1] == appearState[1] || stateWord == 0)) ||
            (growState = (int *)AT(pmf_TrmpItem_GrowScale_2, 0),
             stateWord == growState[0] &&
                 (TRMPITEM(item)->mState[1] == growState[1] || stateWord == 0)))
            return 0;
    }

    result = 0;
    TrmpItem_ProjectOnLine(item, jumper, &currentProjection, &previousProjection);

    {
        int *halfLengths = (int *)AT(sTrmpItemHitWidths, 0);
        lineStart.x = -halfLengths[TRMPITEM(item)->mSizeIdx];
        lineStart.y = 0;
        lineEnd.x = halfLengths[TRMPITEM(item)->mSizeIdx];
        lineEnd.y = 0;
    }

    // The projections must straddle the line without lying on it together.
    Vec2_Sub(&testDirection, &lineEnd, &lineStart);
    Vec2_Sub(&firstOffset, &currentProjection, &lineStart);
    Vec2_Sub(&secondOffset, &previousProjection, &lineStart);
    currentSide = func_0203d524(&testDirection, &firstOffset);
    previousSide = func_0203d524(&testDirection, &secondOffset);
    if (currentSide < -1)
        currentSide = -1;
    else if (currentSide > 1)
        currentSide = 1;
    if (previousSide < -1)
        previousSide = -1;
    else if (previousSide > 1)
        previousSide = 1;
    if (currentSide * previousSide > 0)
        goto done;
    if (currentSide == 0 && previousSide == 0)
        goto done;

    // Require an intersection strictly between the two line endpoints.
    Vec2_Sub(&movementDirection, &currentProjection, &previousProjection);
    testDirection = movementDirection;
    Vec2_Sub(&toEnd, &lineEnd, &currentProjection);
    firstOffset = toEnd;
    Vec2_Sub(&toStart, &lineStart, &currentProjection);
    secondOffset = toStart;
    endSide = func_0203d524(&testDirection, &firstOffset);
    startSide = func_0203d524(&testDirection, &secondOffset);
    if (endSide < -1)
        endSide = -1;
    else if (endSide > 1)
        endSide = 1;
    if (startSide < -1)
        startSide = -1;
    else if (startSide > 1)
        startSide = 1;
    if (endSide * startSide < 0 && (endSide != 0 || startSide != 0))
        result = 1;
done:
    return result;
}
}

namespace s020ce8a0 {
// @symbol TrmpItem_ProjectOnLine

extern "C" void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern "C" int DotVec3(struct Vector3 *a, struct Vector3 *b);

typedef TrampolineObjectCallbacks Obj;

extern "C" void TrmpItem_ProjectOnLine(char *item, Obj *jumper, int *currentProjection,
                                       int *previousProjection) {
    struct Vector3 currentPosition, previousPosition, currentRelative, previousRelative;
    struct Vector3 *position;
    int across;

    position = jumper->GetPos();
    currentPosition.x = position->x;
    currentPosition.y = position->y;
    currentPosition.z = position->z;
    position = jumper->GetPrevPos();
    previousPosition.x = position->x;
    previousPosition.y = position->y;
    previousPosition.z = position->z;
    Vec3_Sub(&currentRelative, &currentPosition, &TRMPITEM(item)->mPos);
    Vec3_Sub(&previousRelative, &previousPosition, &TRMPITEM(item)->mPrevPos);
    // Only the current relative position receives this vertical offset.
    currentRelative.y += 0xc000;
    across = DotVec3(&TRMPITEM(item)->mForward, &currentRelative);
    currentProjection[0] = DotVec3(&TRMPITEM(item)->mSide, &currentRelative);
    currentProjection[1] = across;
    across = DotVec3(&TRMPITEM(item)->mPrevForward, &previousRelative);
    previousProjection[0] = DotVec3(&TRMPITEM(item)->mPrevSide, &previousRelative);
    previousProjection[1] = across;
}
}

namespace s020ce988 {
// @symbol TrmpItem_UpdateMatrix

extern "C" {
void Matrix4x3_FromQuaternion(const void *q, struct Matrix4x3 *mF);
void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
void MulMat4x3Mat4x3(struct Matrix4x3 *a, struct Matrix4x3 *b, struct Matrix4x3 *out);
}
extern "C" {
extern struct Matrix4x3 data_020a0e68;
}

extern "C" void TrmpItem_UpdateMatrix(char *self) {
    Matrix4x3 rotation;
    Matrix4x3_FromQuaternion(TRMPITEM(self)->mRotation, &rotation);
    Matrix4x3_FromTranslation(&data_020a0e68, TRMPITEM(self)->mPos.x, TRMPITEM(self)->mPos.y,
                              TRMPITEM(self)->mPos.z);
    MulMat4x3Mat4x3(&rotation, &data_020a0e68, &data_020a0e68);
    TRMPITEM(self)->mModel->mat4x3 = data_020a0e68;
    TRMPITEM(self)->mTexAnim.Update(TRMPITEM(self)->mModel->data);
    TRMPITEM(self)->mModel->ApplyOpacity(TRMPITEM(self)->mAlpha, 0);
    TRMPITEM(self)->mModel->Render(&TRMPITEM(self)->mScale);
}
}

namespace s020cea2c {
extern "C" {
void AddVec3(void *a, void *b, void *c);
}

struct C;
typedef void (C::*PMF)();

extern "C" void TrmpItem_Update(char *item) {
    PMF *state = (PMF *)item;
    (((C *)item)->**state)();
    AddVec3(&TRMPITEM(item)->mPos, &TRMPITEM(item)->mVel, &TRMPITEM(item)->mPos);
    TRMPITEM(item)->mTexAnim.Advance();
    TRMPITEM(item)->mPrevPos.x = TRMPITEM(item)->mPos.x;
    TRMPITEM(item)->mPrevPos.y = TRMPITEM(item)->mPos.y;
    TRMPITEM(item)->mPrevPos.z = TRMPITEM(item)->mPos.z;
    TRMPITEM(item)->mPrevForward.x = TRMPITEM(item)->mForward.x;
    TRMPITEM(item)->mPrevForward.y = TRMPITEM(item)->mForward.y;
    TRMPITEM(item)->mPrevForward.z = TRMPITEM(item)->mForward.z;
    TRMPITEM(item)->mPrevSide.x = TRMPITEM(item)->mSide.x;
    TRMPITEM(item)->mPrevSide.y = TRMPITEM(item)->mSide.y;
    TRMPITEM(item)->mPrevSide.z = TRMPITEM(item)->mSide.z;
}
}

namespace s020ceabc {
typedef struct {
    int x, y, z;
} Vec3;

extern "C" {
extern int data_02092768[4];
}
extern "C" {
extern int data_020a0ebc[3];
}
extern "C" extern void func_0203cc28(int *p, int angle);
extern "C" extern void Quaternion_FromVector3(int *q, Vec3 *a, Vec3 *b);
extern "C" extern void TrmpItem_StartAppear(char *c);

extern "C" void TrmpItem_Init(char *self, int *position, int *normal, int itemType,
                              short motionType) {
    /* The axis the starting rotation is built about, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);

    TRMPITEM(self)->mPos.x = position[0];
    TRMPITEM(self)->mPos.y = position[1];
    TRMPITEM(self)->mPos.z = position[2];
    TRMPITEM(self)->mForward.x = normal[0];
    TRMPITEM(self)->mForward.y = normal[1];
    TRMPITEM(self)->mForward.z = normal[2];
    {
        int *normalComponents = (int *)(int)&TRMPITEM(self)->mForward;
        TRMPITEM(self)->mSide.x = normalComponents[0];
        TRMPITEM(self)->mSide.y = normalComponents[1];
        TRMPITEM(self)->mSide.z = normalComponents[2];
    }
    func_0203cc28((int *)&TRMPITEM(self)->mSide, -0x4000);
    TRMPITEM(self)->mPrevPos.x = TRMPITEM(self)->mPos.x;
    TRMPITEM(self)->mPrevPos.y = TRMPITEM(self)->mPos.y;
    TRMPITEM(self)->mPrevPos.z = TRMPITEM(self)->mPos.z;
    {
        int *cachedPosition = (int *)(int)&TRMPITEM(self)->mPrevPos;
        TRMPITEM(self)->mSpawnPos.x = cachedPosition[0];
        TRMPITEM(self)->mSpawnPos.y = cachedPosition[1];
        TRMPITEM(self)->mSpawnPos.z = cachedPosition[2];
    }
    TRMPITEM(self)->mPrevForward.x = TRMPITEM(self)->mForward.x;
    TRMPITEM(self)->mPrevForward.y = TRMPITEM(self)->mForward.y;
    TRMPITEM(self)->mPrevForward.z = TRMPITEM(self)->mForward.z;
    TRMPITEM(self)->mPrevSide.x = TRMPITEM(self)->mSide.x;
    TRMPITEM(self)->mPrevSide.y = TRMPITEM(self)->mSide.y;
    TRMPITEM(self)->mPrevSide.z = TRMPITEM(self)->mSide.z;
    TRMPITEM(self)->mRotation[0] = data_02092768[0];
    TRMPITEM(self)->mRotation[1] = data_02092768[1];
    TRMPITEM(self)->mRotation[2] = data_02092768[2];
    TRMPITEM(self)->mRotation[3] = data_02092768[3];
    Quaternion_FromVector3(TRMPITEM(self)->mRotation, (Vec3 *)&sUnitY,
                           (Vec3 *)&TRMPITEM(self)->mForward);
    TRMPITEM(self)->mSizeIdx = itemType;
    TRMPITEM(self)->mMotionKind = motionType;
    TRMPITEM(self)->mActive = 1;
    switch (TRMPITEM(self)->mSizeIdx) {
    case 0:
        TRMPITEM(self)->mModel = &TRMPITEM(self)->mRingA;
        break;
    case 1:
        TRMPITEM(self)->mModel = &TRMPITEM(self)->mRingB;
        break;
    case 2:
    default:
        TRMPITEM(self)->mModel = &TRMPITEM(self)->mRingC;
        break;
    }
    TRMPITEM(self)->mScale.x = data_020a0ebc[0];
    TRMPITEM(self)->mScale.y = data_020a0ebc[1];
    TRMPITEM(self)->mScale.z = data_020a0ebc[2];
    TRMPITEM(self)->mAppearScale = 0;
    *(short *)(self + 0x92) = 0x5a;
    TrmpItem_StartAppear(self);
}
}

namespace s020cecb4 {
extern "C" void TrmpItem_Hide(int *p) {
    TRMPITEM(p)->mActive = 0;
}
}

namespace s020cecc0 {
extern "C" {
void TrmpItem_Hide(int *p);
}
extern "C" {
extern BTA_File sTrmpItemTextureAnim;
}

extern "C" void TrmpItem_Load(char *c) {
    TRMPITEM(c)->mRingA.SetFile((BMD_File *)sTrmpItemRingAModelFile.filePtr, 1, -1);
    TRMPITEM(c)->mRingB.SetFile((BMD_File *)sTrmpItemRingBModelFile.filePtr, 1, -1);
    TRMPITEM(c)->mRingC.SetFile((BMD_File *)sTrmpItemRingCModelFile.filePtr, 1, -1);
    TRMPITEM(c)->mRingA.SetPolygonID(1);
    TRMPITEM(c)->mRingB.SetPolygonID(2);
    TRMPITEM(c)->mRingC.SetPolygonID(3);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)sTrmpItemRingAModelFile.filePtr, sTrmpItemTextureAnim);
    TRMPITEM(c)->mTexAnim.SetFile(sTrmpItemTextureAnim, 0, 0x1000, 0);
    TrmpItem_Hide((int *)c);
}
}

namespace s020ced84 {
/* TrmpItem_UpdateMatricesAll at 0x020ced84
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern char *sTrmpItems;
}
extern "C" extern void TrmpItem_UpdateMatrix(char *c);

extern "C" void TrmpItem_UpdateMatricesAll(void) {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        char *p = sTrmpItems + itemIndex * 0x1d0;
        if (TRMPITEM(p)->mActive != 0)
            TrmpItem_UpdateMatrix(p);
    }
}
}

namespace s020cedf0 {
/* TrmpItem_UpdateAll at 0x020cedf0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern char *sTrmpItems;
}
extern "C" extern void TrmpItem_Update(char *c);

extern "C" void TrmpItem_UpdateAll(void) {
    int itemIndex = 0;
    int itemOffset;
    if (sTrmpItemCount > 0) {
        itemOffset = 0;
        do {
            char *p = sTrmpItems + itemOffset;
            if (TRMPITEM(p)->mActive != 0) {
                TrmpItem_Update(p);
            }
            itemIndex++;
            itemOffset += 0x1d0;
        } while (itemIndex < sTrmpItemCount);
    }
}
}

namespace s020cee5c {
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern unsigned char *sTrmpItems;
}
extern "C" {
extern int sTrmpItemsActive;
}
extern "C" {
extern int sTrmpItemsHit;
}
extern "C" {
extern int sTrmp2Score;
}
extern "C" extern void TrmpItem_Hide(int *p);
extern "C" void TrmpItem_HideAll(void) {
    int itemIndex = 0;
    int itemOffset;
    if (sTrmpItemCount > 0) {
        itemOffset = 0;
        do {
            TrmpItem_Hide((int *)(sTrmpItems + itemOffset));
            itemIndex++;
            itemOffset += 0x1d0;
        } while (itemIndex < sTrmpItemCount);
    }
    sTrmpItemsActive = 0;
    sTrmpItemsHit = 0;
    sTrmp2Score = 0;
}
}

namespace s020ceedc {
extern "C" void TrmpItem_ReleaseFiles(void) {
    sTrmpItemRingAModelFile.Release();
    sTrmpItemRingBModelFile.Release();
    sTrmpItemRingCModelFile.Release();
}
}

namespace s020cef14 {
extern "C" extern void TrmpItem_Load(void *p);
extern "C" {
extern char *sTrmpItems[];
}
extern "C" {
extern int sTrmpItemCount[];
}

extern "C" void TrmpItem_LoadAll(char *items, int count) {
    int itemIndex;
    int itemOffset;
    dExtSimpleModel_c::LoadFile(sTrmpItemRingAModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingBModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingCModelFile);
    sTrmpItems[0] = items;
    sTrmpItemCount[0] = count;
    itemIndex = 0;
    if (count <= 0)
        return;
    itemOffset = 0;
    do {
        TrmpItem_Load(sTrmpItems[0] + itemOffset);
        itemIndex++;
        itemOffset += 0x1d0;
    } while (itemIndex < sTrmpItemCount[0]);
}
}

namespace s020cefa4 {
/* TrmpItem_Spawn at 0x020cefa4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern "C" {
extern int sTrmpItemCount;
}
extern "C" {
extern char *sTrmpItems;
}
// Keep motionType word-sized here: a short declaration adds sign extension
// in this caller. The initializer retains its ROM-matched short load.
extern "C" void TrmpItem_Init(char *item, int *position, int *normal, int itemType, int motionType);

extern "C" char *TrmpItem_Spawn(int position, int *normal, int itemType, int motionType) {
    int itemIndex = 0;
    char *base;
    int itemOffset;
    if (sTrmpItemCount > 0) {
        base = sTrmpItems;
        do {
            if (TRMPITEM(base)->mActive == 0) {
                itemOffset = itemIndex * 0x1d0;
                TrmpItem_Init(sTrmpItems + itemOffset, (int *)position, normal, itemType,
                              motionType);
                return sTrmpItems + itemOffset;
            }
            itemIndex++;
            base += 0x1d0;
        } while (itemIndex < sTrmpItemCount);
    }
    return 0;
}
}

namespace s020cf040 {
struct Vec3 {
    int x;
    int y;
    int z;
};
extern "C" extern int DotVec3(struct Vec3 *a, struct Vec3 *b);
extern "C" extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern "C" extern int LenVec3(struct Vec3 *v);
extern "C" {
extern int data_ov006_0212e0f0[];
}
extern "C" void TrmpLine_PointDistance(char *sl, void *referencePoint, struct Vec3 *offsetVector) {
    int row;
    int negatedProjection = -DotVec3(offsetVector, (struct Vec3 *)&TRMPLINE(sl)->mNormal);
    int *amplitude = TRMPLINE(sl)->mFlexAmplitude;
    char *restVertex = (char *)TRMPLINE(sl)->mRestVerts;
    int weightedAmplitude;
    int *gridWeight = data_ov006_0212e0f0;
    for (row = 0; row < 4; row++) {
        int column;
        for (column = 0; column < 4; column++) {
            struct Vec3 delta;
            SubVec3((struct Vec3 *)restVertex, (struct Vec3 *)referencePoint, &delta);
            int halfLength = (long long)(TRMPLINE(sl)->mHalfWidth);
            int distance = LenVec3(&delta);
            int distanceWeight = cstd::fdiv(halfLength, halfLength + distance);
            int weightedProjection = FX_MUL(negatedProjection, *gridWeight);
            weightedAmplitude = FX_MUL(weightedProjection, distanceWeight);
            *amplitude = weightedAmplitude;
            restVertex += 0xc;
            gridWeight++;
            amplitude++;
        }
    }
}
}

namespace s020cf124 {
typedef struct {
    int x, y, z;
} Vec3;

extern "C" extern void SubVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern "C" extern void CrossVec3(const Vec3 *a, const Vec3 *b, Vec3 *out);
extern "C" extern void NormalizeVec3(int *v, int *out);

#pragma push
#pragma opt_strength_reduction off
extern "C" void TrmpLine_ComputeNormals(char *self) {
    Vec3 columnEdge;
    Vec3 rowEdge;
    char *p;
    Vec3 *normal;
    int *packedNormal;
    int row;
    int column;
    int componentMask = 0x3ff;

    p = (char *)TRMPLINE(self)->mVerts;
    normal = (Vec3 *)TRMPLINE(self)->mNormals;
    packedNormal = TRMPLINE(self)->mPackedNormals;

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            int *components;
            int zero = 0;

            components = (int *)&columnEdge;
            components[0] = zero;
            components[1] = zero;
            components[2] = zero;
            components = (int *)&rowEdge;
            components[0] = zero;
            components[1] = zero;
            components[2] = zero;

            if (column == 0)
                SubVec3((Vec3 *)p, (Vec3 *)p + 1, &columnEdge);
            else if (column == 3)
                SubVec3((Vec3 *)p - 1, (Vec3 *)p, &columnEdge);
            else
                SubVec3((Vec3 *)p - 1, (Vec3 *)p + 1, &columnEdge);

            if (row == 0)
                SubVec3((Vec3 *)p, (Vec3 *)p + 4, &rowEdge);
            else if (row == 3)
                SubVec3((Vec3 *)p - 4, (Vec3 *)p, &rowEdge);
            else
                SubVec3((Vec3 *)p - 4, (Vec3 *)p + 4, &rowEdge);

            CrossVec3(&columnEdge, &rowEdge, normal);
            NormalizeVec3((int *)normal, (int *)normal);

            normal->x = FX_MUL(normal->x, 0xff8);
            normal->y = FX_MUL(normal->y, 0xff8);
            normal->z = FX_MUL(normal->z, 0xff8);

            *packedNormal = ((normal->x >> 3) & componentMask) |
                            (((normal->y >> 3) & componentMask) << 10) |
                            (((normal->z >> 3) & componentMask) << 20);

            normal++;
            packedNormal++;
            p += 0xc;
        }
    }
}
#pragma pop
}

namespace s020cf2fc {
typedef volatile unsigned int vu32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

struct Matrix4x3;
extern "C" extern struct Matrix4x3 data_020a0e68;
extern "C" extern struct Matrix4x3 data_0209b3ec;
extern "C" extern unsigned short sTrmpLineDiffuse[];
extern "C" extern unsigned short sTrmpLineAmbient[];
extern "C" extern int sTrmpLineTexCoords[];
extern "C" extern void *sTrmpLineTexAddr;
extern "C" extern void *sTrmpLinePlttAddr;
extern "C" extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern "C" extern void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
extern "C" extern void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3 *m, int x, int y, int z);
extern "C" extern void func_020553a4(int *m);

#define REG_MTX_MODE       (*(vu32 *)0x4000440)
#define REG_MTX_IDENTITY   (*(vu32 *)0x4000454)
#define REG_MTX_SCALE      (*(vu32 *)0x400046c)
#define REG_NORMAL         (*(vu32 *)0x4000484)
#define REG_TEXCOORD       (*(vu32 *)0x4000488)
#define REG_VTX_16         (*(vu32 *)0x400048c)
#define REG_POLYGON_ATTR   (*(vu32 *)0x40004a4)
#define REG_TEXIMAGE_PARAM (*(vu32 *)0x40004a8)
#define REG_TEXPLTT_BASE   (*(vu32 *)0x40004ac)
#define REG_DIF_AMB        (*(vu32 *)0x40004c0)
#define REG_SPE_EMI        (*(vu32 *)0x40004c4)
#define REG_BEGIN_VTXS     (*(vu32 *)0x4000500)
#define REG_END_VTXS       (*(vu32 *)0x4000504)

#define PATCH_VTX(obj) ((Vec3 *)TRMPLINE(obj)->mVerts)
#define PATCH_NRM(obj) (TRMPLINE(obj)->mPackedNormals)

static inline void G3_Vtx(s16 x, s16 y, s16 z) {
    REG_VTX_16 = (u16)x | ((u16)y << 16);
    REG_VTX_16 = (u16)z;
}

#define SEND_VTX(p) G3_Vtx((s16)((p)->x >> 8), (s16)((p)->y >> 8), (s16)((p)->z >> 8))

// @symbol TrmpLine_Draw
/* Loads the object's placement and scale matrices, then draws its 4x4 vertex
 * patch (positions mVerts, packed normals mPackedNormals) as three triangle
 * strips per face, the back face first with negated normals. Each normal
 * pointer steps in its own statement after the REG_NORMAL write; stepping it
 * inside the write reorders the vertex loads. */
extern "C" void TrmpLine_Draw(char *obj) {
    int i;
    int m2[12];
    int m1[12];
    Matrix4x3_FromTranslation(&data_020a0e68, TRMPLINE(obj)->mCenter.x, TRMPLINE(obj)->mCenter.y,
                              TRMPLINE(obj)->mCenter.z);
    MulMat4x3Mat4x3((const int *)&data_020a0e68, (const int *)&data_0209b3ec, m1);
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, TRMPLINE(obj)->mScale.x, TRMPLINE(obj)->mScale.y,
                                  TRMPLINE(obj)->mScale.z);
    MulMat4x3Mat4x3((const int *)&data_020a0e68, (const int *)&data_0209b3ec, m2);
    REG_MTX_MODE = 2;
    func_020553a4(m1);
    REG_MTX_MODE = 1;
    func_020553a4(m2);
    REG_MTX_SCALE = 0x100000;
    REG_MTX_SCALE = 0x100000;
    REG_MTX_SCALE = 0x100000;
    REG_MTX_MODE = 3;
    REG_MTX_IDENTITY = 0;
    REG_TEXIMAGE_PARAM = 0x8da70000 | ((u32)sTrmpLineTexAddr >> 3);
    REG_TEXPLTT_BASE = (u32)sTrmpLinePlttAddr >> 4;
    {
        short *slotIndex = &TRMPLINE(obj)->mSlotIndex;
        int sh = *slotIndex;
        unsigned short *dif = sTrmpLineDiffuse;
        volatile unsigned char *alpha = &TRMPLINE(obj)->mAlpha;
        int pl = *alpha;
        REG_POLYGON_ATTR = (((sh + 1) << 24) | 0x82) | (pl << 16);
        {
            unsigned short *bounceCount = &TRMPLINE(obj)->mBounceCount;
            unsigned idx = *bounceCount;
            REG_DIF_AMB = dif[idx] | (sTrmpLineAmbient[idx] << 16);
        }
    }
    REG_SPE_EMI = 0x8000;

    /* back face: row i then row i+1, normals negated */
    for (i = 0; i < 3; i++) {
        Vec3 *v0 = &PATCH_VTX(obj)[i * 4];
        Vec3 *v1 = &PATCH_VTX(obj)[(i + 1) * 4];
        int *n0 = &PATCH_NRM(obj)[i * 4];
        int *n1 = &PATCH_NRM(obj)[(i + 1) * 4];
        int k;
        REG_BEGIN_VTXS = 2;
        for (k = 0; k < 4; k++) {
            REG_TEXCOORD = sTrmpLineTexCoords[i * 4 + k];
            REG_NORMAL = -*n0 & 0x3fffffff;
            n0++;
            SEND_VTX(v0);
            v0++;
            REG_TEXCOORD = sTrmpLineTexCoords[(i + 1) * 4 + k];
            REG_NORMAL = -*n1 & 0x3fffffff;
            n1++;
            SEND_VTX(v1);
            v1++;
        }
        REG_END_VTXS = 0;
    }

    /* front face: row i+1 then row i */
    for (i = 0; i < 3; i++) {
        Vec3 *v0 = &PATCH_VTX(obj)[i * 4];
        Vec3 *v1 = &PATCH_VTX(obj)[(i + 1) * 4];
        int *n0 = &PATCH_NRM(obj)[i * 4];
        int *n1 = &PATCH_NRM(obj)[(i + 1) * 4];
        int k;
        REG_BEGIN_VTXS = 2;
        for (k = 0; k < 4; k++) {
            REG_TEXCOORD = sTrmpLineTexCoords[(i + 1) * 4 + k];
            REG_NORMAL = *n1;
            n1++;
            SEND_VTX(v1);
            v1++;
            REG_TEXCOORD = sTrmpLineTexCoords[i * 4 + k];
            REG_NORMAL = *n0;
            n0++;
            SEND_VTX(v0);
            v0++;
        }
        REG_END_VTXS = 0;
    }
}
}

namespace s020cf758 {
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
// @symbol TrmpLine_Update
extern "C" void TrmpLine_Update(C *c) {
    (c->*(c->pmf))();
}
}

namespace s020cf790 {
extern "C" extern void TrmpLine_ComputeNormals(char *c);

// @symbol TrmpLine_StateFade
extern "C" void TrmpLine_StateFade(char *c) {
    if (ApproachLinear2(*(int *)&TRMPLINE(c)->mAlpha, 0, 1) != 0) {
        TRMPLINE(c)->mStatus = 0;
        return;
    }
    TRMPLINE(c)->mScale.x += 0x80;
    TRMPLINE(c)->mScale.y += 0x80;
    TRMPLINE(c)->mScale.z += 0x80;
    TrmpLine_ComputeNormals(c);
}
}

namespace s020cf804 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpLine_StateFade;
}
// @symbol TrmpLine_SetStateFade
extern "C" void TrmpLine_SetStateFade(char *p) {
    *(struct S *)TRMPLINE(p)->mState = pmf_TrmpLine_StateFade;
}
}

namespace s020cf820 {
extern "C" extern void TrmpLine_ComputeNormals(char *c);
extern "C" {
extern int data_020a0db0;
}
extern "C" {
extern s16 data_02082214[];
}

// @symbol TrmpLine_StateSettle
extern "C" void TrmpLine_StateSettle(char *c) {
    int normalX, normalY;
    int negativeSine;
    int deflectionX, deflectionY;
    u16 phase;
    int row, column;
    int targetAmplitude, decayStep;
    int *restPosition, *position, *amplitude;

    if ((data_020a0db0 & 1) != 0) {
        if (ApproachLinear2(*(int *)&TRMPLINE(c)->mAlpha, 0, 1) != 0) {
            TRMPLINE(c)->mStatus = 0;
        }
    }

    {
        u16 *phasePtr = &TRMPLINE(c)->mWobblePhase;
        *phasePtr = *phasePtr + TRMPLINE(c)->mWobbleSpeed;
        phase = TRMPLINE(c)->mWobblePhase;
    }

    normalX = TRMPLINE(c)->mNormal.x;
    normalY = TRMPLINE(c)->mNormal.y;

    negativeSine = -(int)data_02082214[(phase >> 4) * 2];

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    restPosition = (int *)TRMPLINE(c)->mRestVerts;
    position = (int *)TRMPLINE(c)->mVerts;
    amplitude = TRMPLINE(c)->mFlexAmplitude;

    targetAmplitude = 0;
    decayStep = 0x100;

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            position[0] = restPosition[0];
            position[1] = restPosition[1];
            position[2] = restPosition[2];
            position[0] = position[0] + FX_MUL(*amplitude, deflectionX);
            position[1] = position[1] + FX_MUL(*amplitude, deflectionY);
            ApproachLinear(*amplitude, targetAmplitude, decayStep);
            restPosition += 3;
            position += 3;
            amplitude += 1;
        }
    }

    TrmpLine_ComputeNormals(c);
}
}

namespace s020cfa28 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpLine_StateSettle;
}
// @symbol TrmpLine_SetStateSettle
extern "C" void TrmpLine_SetStateSettle(char *p) {
    *(struct S *)TRMPLINE(p)->mState = pmf_TrmpLine_StateSettle;
}
}

namespace s020cfa44 {
typedef struct {
    int x, y, z;
} Vec3;

extern "C" {
extern s16 data_02082214[];
}
extern "C" extern int TrmpLine_PushJumper(char *o);
extern "C" extern void TrmpLine_ComputeNormals(char *o);
extern "C" extern void TrmpLine_SetStateFade(char *o);

// @symbol TrmpLine_StateFlex
extern "C" void TrmpLine_StateFlex(char *o) {
    int row, column;
    int normalX, normalY;
    int angleIndex;
    s16 sine;
    int negativeSine;
    int deflectionX, deflectionY;
    int targetAmplitude;
    int firstColumn;
    int decayStep;
    Vec3 *restPosition;
    Vec3 *position;
    int *amplitude;
    u16 *phasePtr;

    TrmpLine_PushJumper(o);

    phasePtr = &TRMPLINE(o)->mWobblePhase;
    *phasePtr = *phasePtr + TRMPLINE(o)->mWobbleSpeed;

    normalX = TRMPLINE(o)->mNormal.x;
    normalY = TRMPLINE(o)->mNormal.y;

    angleIndex = TRMPLINE(o)->mWobblePhase >> 4;
    sine = data_02082214[angleIndex * 2];
    negativeSine = -(int)sine;

    restPosition = (Vec3 *)TRMPLINE(o)->mRestVerts;
    position = (Vec3 *)TRMPLINE(o)->mVerts;

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    amplitude = TRMPLINE(o)->mFlexAmplitude;

    targetAmplitude = 0;
    firstColumn = 0;
    decayStep = 0x100;

    for (row = 0; row < 4; row++) {
        for (column = firstColumn; column < 4; column++) {
            position->x = restPosition->x;
            position->y = restPosition->y;
            position->z = restPosition->z;
            position->x += FX_MUL(*amplitude, deflectionX);
            position->y += FX_MUL(*amplitude, deflectionY);

            ApproachLinear(*amplitude, targetAmplitude, decayStep);
            restPosition++;
            position++;
            amplitude++;
        }
    }

    TrmpLine_ComputeNormals(o);

    if (TRMPLINE(o)->mSlotIndex == 3)
        return;

    if (ApproachLinear2(TRMPLINE(o)->mLifeTimer, 0, 1) == 0)
        return;

    TrmpLine_SetStateFade(o);
}
}

namespace s020cfc58 {
struct S {
    int w[2];
};
extern "C" {
extern struct S pmf_TrmpLine_StateFlex;
}
// @symbol TrmpLine_SetStateFlex
extern "C" void TrmpLine_SetStateFlex(char *p) {
    *(struct S *)TRMPLINE(p)->mState = pmf_TrmpLine_StateFlex;
}
}

namespace s020cfc74 {
/*
 * Per-frame gate-crossing check over the five tracked objects
 * (sTrmpJumpers). For each live object, project its current and
 * previous positions onto the gate frame (line+0x14 / line+0x20 basis,
 * line+0x58 half-width). A segment that crosses the gate records the hit
 * (line+0x38 position, line+0x44
 * direction, state 1 or 2 with a sound whose pitch is lerped from the
 * crossing point), decrements the remaining count at line+0x324 and returns 1
 * when it reaches zero. Segments that miss just outside the gate edges are
 * marked state 3.
 *
 * Shape notes: the +-1 clamps are a ternary macro so the constants rank
 * above the coordinate/index locals in the callee-saved band and still
 * hoist in the compiler's own order; jumpers is assigned after the null/active checks so its pool
 * load sits in the hoisted-constant sequence; the +0x320/+0x324 halfword
 * accesses go through line directly, which yields the ROM's shared
 * line+0x300 base and the copied read.
 */
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))

struct V2 {
    int x, y;
};

// Consumed prefix only; no complete jumper-object size is implied.
struct TrampolineContactView : TrampolineObjectCallbacks {
    Vector3 lineNormal; // +0x04
    int alongLine;      // +0x10: dot with the line direction
    int lineHalfLength; // +0x14
    u16 contactState;   // +0x18: cleared by callback slot 4
};

extern "C" extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern "C" extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern "C" extern int DotVec3(Vector3 *a, Vector3 *b);
extern "C" extern void Vec2_Sub(int *o, int *a, int *b);
extern "C" extern int func_0203d524(int *a, int *b);
extern "C" extern void D3D_PlaySePannedLoop(int a0, int a1, int a2);
extern "C" extern void TrmpLine_PointDistance(char *sl, void *arg1, Vector3 *r2);
extern "C" extern void TrmpLine_SetStateSettle(char *p);
extern "C" extern void Vec3_MulScalar(Vector3 *out, Vector3 *in, int scale);
extern "C" extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);

extern "C" {
extern struct TrampolineContactView *sTrmpJumpers[];
}
extern "C" {
extern int sTrmpLinePushWeakPitchMid;
}
extern "C" {
extern int sTrmpLinePushWeakPitchEnd;
}
extern "C" {
extern int sTrmpLinePushStrongPitchMid;
}
extern "C" {
extern int sTrmpLinePushStrongPitchEnd;
}

#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
// @symbol TrmpLine_PushJumper
extern "C" int TrmpLine_PushJumper(char *c) {
    int jumperIndex;
    int currentAlong;
    struct TrampolineContactView **jumpers;

    jumperIndex = 0;
    do {
        int currentAcross, previousAlong, previousAcross;
        Vector3 currentPos, previousPos, movementDelta;
        struct V2 previousProjection, currentProjection;
        struct V2 lineStart, lineEnd;
        struct V2 edgeDirection, firstOffset, secondOffset;
        int crossedLine, previousSide, currentSide;
        Vector3 fromStart, fromEnd, halfLineForStart, halfLineForEnd;
        Vector3 *currentPositionPtr;
        Vector3 *previousPositionPtr;

        if (sTrmpJumpers[jumperIndex] == 0)
            continue;
        if (sTrmpJumpers[jumperIndex]->IsActive() == 0)
            continue;
        jumpers = sTrmpJumpers;

        currentPositionPtr = jumpers[jumperIndex]->GetPos();
        currentPos.x = currentPositionPtr->x;
        currentPos.y = currentPositionPtr->y;
        currentPos.z = currentPositionPtr->z;

        previousPositionPtr = jumpers[jumperIndex]->GetPrevPos();
        previousPos.x = previousPositionPtr->x;
        previousPos.y = previousPositionPtr->y;
        previousPos.z = previousPositionPtr->z;

        Vec3_Sub(&movementDelta, &currentPos, &previousPos);
        SubVec3(&currentPos, &TRMPLINE(c)->mCenter, &currentPos);
        SubVec3(&previousPos, &TRMPLINE(c)->mCenter, &previousPos);

        // Test the previous-to-current segment against the c in its own frame.
        currentAlong = DotVec3(&TRMPLINE(c)->mAxis, &currentPos);
        currentAcross = DotVec3(&TRMPLINE(c)->mNormal, &currentPos);
        previousAlong = DotVec3(&TRMPLINE(c)->mAxis, &previousPos);
        previousAcross = DotVec3(&TRMPLINE(c)->mNormal, &previousPos);
        DotVec3(&TRMPLINE(c)->mNormal, &movementDelta);
        DotVec3(&TRMPLINE(c)->mAxis, &movementDelta);

        currentProjection.y = currentAcross;
        previousProjection.x = previousAlong;
        currentProjection.x = currentAlong;
        previousProjection.y = previousAcross;

        lineStart.x = -(TRMPLINE(c)->mHalfWidth);
        lineStart.y = 0;
        lineEnd.x = TRMPLINE(c)->mHalfWidth;
        lineEnd.y = 0;

        Vec2_Sub((int *)&edgeDirection, (int *)&lineStart, (int *)&lineEnd);
        Vec2_Sub((int *)&firstOffset, (int *)&lineStart, (int *)&previousProjection);
        Vec2_Sub((int *)&secondOffset, (int *)&lineStart, (int *)&currentProjection);

        crossedLine = 0;
        previousSide = func_0203d524((int *)&edgeDirection, (int *)&firstOffset);
        currentSide = func_0203d524((int *)&edgeDirection, (int *)&secondOffset);
        previousSide = CLAMP(previousSide, -1, 1);
        currentSide = CLAMP(currentSide, -1, 1);

        if (previousSide * currentSide <= 0 && previousSide > currentSide) {
            struct V2 edge, e1, e2;
            int startSide, endSide;
            Vec2_Sub((int *)&edge, (int *)&previousProjection, (int *)&currentProjection);
            edgeDirection = edge;
            Vec2_Sub((int *)&e1, (int *)&previousProjection, (int *)&lineStart);
            firstOffset = e1;
            Vec2_Sub((int *)&e2, (int *)&previousProjection, (int *)&lineEnd);
            secondOffset = e2;

            startSide = func_0203d524((int *)&edgeDirection, (int *)&firstOffset);
            endSide = func_0203d524((int *)&edgeDirection, (int *)&secondOffset);
            startSide = CLAMP(startSide, -1, 1);
            endSide = CLAMP(endSide, -1, 1);
            if (startSide * endSide <= 0)
                crossedLine = 1;
        }

        if (crossedLine != 0) {
            int offsetTimesHalfLength, edgeRatio;
            TRMPLINE(c)->mContactPoint.x = currentPos.x;
            TRMPLINE(c)->mContactPoint.y = currentPos.y;
            TRMPLINE(c)->mContactPoint.z = currentPos.z;
            TRMPLINE(c)->mContactMotion.x = movementDelta.x;
            TRMPLINE(c)->mContactMotion.y = movementDelta.y;
            TRMPLINE(c)->mContactMotion.z = movementDelta.z;

            edgeRatio = cstd::fdiv((currentAlong < 0) ? -currentAlong : currentAlong,
                                   TRMPLINE(c)->mHalfWidth);
            {
                int halfLength = TRMPLINE(c)->mHalfWidth;
                int absoluteAlong = (currentAlong < 0) ? -currentAlong : currentAlong;
                offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
            }

            if (offsetTimesHalfLength < 0x400000) {
                jumpers[jumperIndex]->contactState = 2;
                D3D_PlaySePannedLoop(0x1b1, TRMPLINE(c)->mCenter.x,
                                     (sTrmpLinePushWeakPitchEnd * edgeRatio +
                                      sTrmpLinePushWeakPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            } else {
                jumpers[jumperIndex]->contactState = 1;
                D3D_PlaySePannedLoop(0x1ae, TRMPLINE(c)->mCenter.x,
                                     (sTrmpLinePushStrongPitchEnd * edgeRatio +
                                      sTrmpLinePushStrongPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            }

            {
                int *normalComponents = (int *)&jumpers[jumperIndex]->lineNormal;
                normalComponents[0] = TRMPLINE(c)->mNormal.x;
                normalComponents[1] = TRMPLINE(c)->mNormal.y;
                normalComponents[2] = TRMPLINE(c)->mNormal.z;
                jumpers[jumperIndex]->alongLine = currentAlong;
                jumpers[jumperIndex]->lineHalfLength = TRMPLINE(c)->mHalfWidth;
                TRMPLINE(c)->mWobblePhase = 0;
                TrmpLine_PointDistance(c, &TRMPLINE(c)->mContactPoint,
                                       &TRMPLINE(c)->mContactMotion);

                TRMPLINE(c)->mBouncesLeft -= 1;
                if (TRMPLINE(c)->mBouncesLeft == 0) {
                    TRMPLINE(c)->mStatus = 3;
                    TrmpLine_SetStateSettle(c);
                    return 1;
                }
                TRMPLINE(c)->mBounceCount += 1;
            }
        } else {
            Vec3_MulScalar(&halfLineForStart, &TRMPLINE(c)->mAxis, TRMPLINE(c)->mHalfWidth);
            Vec3_Add(&fromStart, &currentPos, &halfLineForStart);
            Vec3_MulScalar(&halfLineForEnd, &TRMPLINE(c)->mAxis, TRMPLINE(c)->mHalfWidth);
            Vec3_Sub(&fromEnd, &currentPos, &halfLineForEnd);

            if (fromStart.y < 0 && fromStart.y > -0x30000) {
                if (fromStart.x > -0x8000 && fromStart.x < 0) {
                    if (movementDelta.x > 0) {
                        jumpers[jumperIndex]->contactState = 3;
                    }
                }
            } else {
                if (fromEnd.y < 0 && fromEnd.y > -0x30000 && fromEnd.x > 0 && fromEnd.x < 0x8000 &&
                    movementDelta.x < 0) {
                    jumpers[jumperIndex]->contactState = 3;
                }
            }
        }
    } while (++jumperIndex < 5);
    return 0;
}
#pragma pop
}

namespace s020d01e0 {
/* The minigame's touch-drawn line becomes a strip of grid points, and the
 * routine then picks the object that line cuts closest.
 *
 *   THE FRAME. The ROM opens with `sub sp, sp, #0xd4`: fourteen scratch words
 *   of which thirteen are ever written, then thirteen twelve-byte objects of
 *   which twelve are ever written. sp+0x34 and sp+0xc8 are reserved and never
 *   touched. In C that frame is unreachable, because an unused local emits
 *   nothing and is given no home (six shapes were measured, among them a dead
 *   struct copy, a guarded dead store and an aliasing pointer). A type with a
 *   DECLARED destructor behaves differently: the frontend still reserves its
 *   stack slot even when the optimiser empties it. Vector3 declares one already,
 *   for the arrays the ROM destroys through __cxa_vec_cleanup. `spareVec` and
 *   `spare4` are those two reservations written down. Their original spelling is
 *   not recoverable from the bytes; an elided copy of a by-value return is the
 *   likely source.
 *
 *   THE SWAP TEST. Its two reads go through a per-site const cast (see
 *   notes/mwccarm-codegen.md 6bk). The ROM re-reads both halfwords from memory
 *   after writing them, and the cast is its own CSE class, so the stores above
 *   do not forward into it.
 *
 * `colWeight += 0x555` sits at the tail of the inner loop with the other two
 * induction steps. Placed between the two `cell->x` statements instead, which is
 * where the earlier drafts had it, the whole 0x340..0x3e4 window schedules one
 * slot early and 35 words differ. Nothing else moves that residue: 246 pragma
 * names at on and off, every declaration order and type name, the loop form, the
 * pointer form and the multiply operand order are all inert on it.
 */
extern "C" {
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern void Vec3_MulScalar(Vector3 *out, Vector3 *in, int s);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
extern void TrmpLine_PointDistance(void *a, void *b, void *c);
extern void TrmpLine_ComputeNormals(void *a);
extern void D3D_PlaySePannedLoop(int a0, int a1, int a2);
extern void TrmpLine_SetStateSettle(char *p);
extern void TrmpLine_SetStateFlex(char *p);

extern void *sTrmpJumpers[];
extern s32 sTrmpLineWeakPitchEnd;
extern s32 sTrmpLineWeakPitchMid;
extern s32 sTrmpLineStrongPitchEnd;
extern s32 sTrmpLineStrongPitchMid;
}

// The jumper fields a new line writes, the same view TrmpLine_PushJumper uses.
typedef s020cfc74::TrampolineContactView Contact;

struct Spare4 {
    s32 v;
    ~Spare4() {}
};

// @symbol TrmpLine_Build
extern "C" void TrmpLine_Build(char *c, short *startPoint, short *endPoint) {
    Spare4 spare4;
    Vector3 startPosition, endPosition;
    Vector3 selectedPosition, selectedVelocity;
    Vector3 relativePosition, velocity;
    Vector3 endpointSum, midpoint, lineSpan, directionCopy, zAxis, lineNormal;
    Vector3 spareVec;
    short swapX, swapY;
    Fix12i lineLength, rowStep;
    s32 row;
    s32 flag;
    TrmpLine::Vtx *cell, *mirror;
    void *bestObj;
    void *jumper;
    Vector3 *positionPtr, *velocityPtr;
    s32 selectedAcross, selectedAlong;
    s32 jumperIndex;
    s32 along, across, normalVelocity;
    s32 halfLength;
    s32 absoluteAlong, offsetTimesHalfLength, edgeRatio;

    TRMPLINE(c)->mStart[0] = startPoint[0];
    TRMPLINE(c)->mStart[1] = startPoint[1];
    TRMPLINE(c)->mEnd[0] = endPoint[0];
    TRMPLINE(c)->mEnd[1] = endPoint[1];

    startPoint[0] = (s16)(startPoint[0] - 0x80);
    startPoint[1] = (s16)(0 - startPoint[1]);
    endPoint[0] = (s16)(endPoint[0] - 0x80);
    endPoint[1] = (s16)(0 - endPoint[1]);

    if (*(const s16 *)startPoint > *(const s16 *)endPoint) {
        swapX = startPoint[0];
        swapY = startPoint[1];
        startPoint[0] = endPoint[0];
        startPoint[1] = endPoint[1];
        endPoint[0] = swapX;
        endPoint[1] = swapY;
    }

    {
        s32 ax = ((s32)startPoint[0]) << 12;
        s32 ay = ((s32)startPoint[1]) << 12;
        startPosition.x = ax;
        startPosition.y = ay;
        startPosition.z = 0;
    }
    {
        s32 bx = ((s32)endPoint[0]) << 12;
        s32 by = ((s32)endPoint[1]) << 12;
        endPosition.x = bx;
        endPosition.y = by;
        endPosition.z = 0;
    }

    Vec3_Add(&endpointSum, &startPosition, &endPosition);
    Vec3_MulScalar(&midpoint, &endpointSum, 0x800);
    TRMPLINE(c)->mCenter.x = midpoint.x;
    TRMPLINE(c)->mCenter.y = midpoint.y;
    TRMPLINE(c)->mCenter.z = midpoint.z;

    Vec3_Sub(&lineSpan, &endPosition, &startPosition);
    TRMPLINE(c)->mAxis.x = lineSpan.x;
    TRMPLINE(c)->mAxis.y = lineSpan.y;
    TRMPLINE(c)->mAxis.z = lineSpan.z;

    func_0203ce80(&directionCopy, &TRMPLINE(c)->mAxis);

    zAxis.x = 0;
    zAxis.y = 0;
    zAxis.z = 0x1000;
    func_0203cf00(&lineNormal, &TRMPLINE(c)->mAxis, &zAxis);
    TRMPLINE(c)->mNormal.x = lineNormal.x;
    TRMPLINE(c)->mNormal.y = lineNormal.y;
    TRMPLINE(c)->mNormal.z = lineNormal.z;

    if (TRMPLINE(c)->mNormal.y < 0) {
        TRMPLINE(c)->mNormal.x = 0 - TRMPLINE(c)->mNormal.x;
        TRMPLINE(c)->mNormal.y = 0 - TRMPLINE(c)->mNormal.y;
    }

    TRMPLINE(c)->mContactPoint.x = 0;
    TRMPLINE(c)->mContactPoint.y = 0;
    TRMPLINE(c)->mContactPoint.z = 0;
    TRMPLINE(c)->mContactMotion.x = 0 - TRMPLINE(c)->mNormal.x;
    TRMPLINE(c)->mContactMotion.y = 0 - TRMPLINE(c)->mNormal.y;
    TRMPLINE(c)->mContactMotion.z = 0 - TRMPLINE(c)->mNormal.z;
    Vec3_MulScalarInPlace((s32 *)&TRMPLINE(c)->mContactMotion, 0x800);

    SubVec3(&startPosition, &TRMPLINE(c)->mCenter, &startPosition);
    SubVec3(&endPosition, &TRMPLINE(c)->mCenter, &endPosition);
    lineLength = Vec3_Dist(&startPosition, &endPosition);
    TRMPLINE(c)->mHalfWidth = lineLength >> 1;
    rowStep = lineLength / 3;

    {
        s32 column;
        s32 rowFrac, colWeight, colFrac;
        s32 accum;
        s32 negRowFrac;
        s32 rowAbs, colAbs;
        s32 colW, invColWeight, rowW, cornerW, weightA, weightB, weightSq;

        row = 0;
        accum = 0;
        rowFrac = -0x180;
        cell = TRMPLINE(c)->mVerts;
        mirror = TRMPLINE(c)->mRestVerts;
        do {
            column = 0;
            negRowFrac = 0 - rowFrac;
            colFrac = -0x180;
            colWeight = 0;
            do {
                colAbs = (colFrac < 0) ? (0 - colFrac) : colFrac;
                colW = 0x1000 - (0x180 - colAbs);
                invColWeight = 0x1000 - colWeight;
                rowAbs = (rowFrac < 0) ? negRowFrac : rowFrac;
                rowW = 0x1000 - (0x180 - rowAbs);
                cornerW = FX_MUL(colW, rowW);
                flag = 0;
                weightA = FX_MUL(invColWeight, cornerW);
                weightSq = FX_MUL(cornerW, cornerW);
                weightB = FX_MUL(colWeight, cornerW);

                cell->x = FX_MUL(endPosition.x, weightB);
                cell->x += FX_MUL(startPosition.x, weightA);
                cell->y = FX_MUL(endPosition.y, weightB);
                cell->y += FX_MUL(startPosition.y, weightA);

                cell->z = accum - TRMPLINE(c)->mHalfWidth;
                cell->z = FX_MUL(cell->z, weightSq);

                mirror->x = cell->x;
                mirror->y = cell->y;
                mirror->z = cell->z;

                column++;
                colFrac += 0x100;
                colWeight += 0x555;
                cell++;
                mirror++;
            } while (column < 4);
            row++;
            accum += rowStep;
            rowFrac += 0x100;
        } while (row < 4);
    }

    TRMPLINE(c)->mWobblePhase = (s16)flag;
    TRMPLINE(c)->mWobbleSpeed = 0x1400;
    TRMPLINE(c)->mAlpha = 0x1F;
    TrmpLine_PointDistance(c, &TRMPLINE(c)->mContactPoint, &TRMPLINE(c)->mContactMotion);
    TrmpLine_ComputeNormals(c);

    TRMPLINE(c)->mScale.x = 0x1000;
    TRMPLINE(c)->mScale.y = 0x1000;
    TRMPLINE(c)->mScale.z = 0x1000;

    bestObj = 0;
    selectedAcross = 0;
    selectedAlong = 0;
    selectedPosition.x = 0;
    selectedPosition.y = 0;
    selectedPosition.z = 0;
    selectedVelocity.x = 0;
    selectedVelocity.y = 0;
    selectedVelocity.z = 0;

    jumperIndex = 0;
    do {
        jumper = sTrmpJumpers[jumperIndex];
        if (jumper != 0) {
            if (((TrampolineObjectCallbacks *)jumper)->IsActive() != 0) {
                jumper = sTrmpJumpers[jumperIndex];
                positionPtr = ((TrampolineObjectCallbacks *)jumper)->GetPos();
                relativePosition.x = positionPtr->x;
                relativePosition.y = positionPtr->y;
                relativePosition.z = positionPtr->z;
                jumper = sTrmpJumpers[jumperIndex];
                velocityPtr = ((TrampolineObjectCallbacks *)jumper)->GetVelocity();
                velocity.x = velocityPtr->x;
                velocity.y = velocityPtr->y;
                velocity.z = velocityPtr->z;

                SubVec3(&relativePosition, &TRMPLINE(c)->mCenter, &relativePosition);
                along = DotVec3(&TRMPLINE(c)->mAxis, &relativePosition);
                across = DotVec3(&TRMPLINE(c)->mNormal, &relativePosition);
                normalVelocity = DotVec3(&TRMPLINE(c)->mNormal, &velocity);
                DotVec3(&TRMPLINE(c)->mAxis, &velocity);

                if (normalVelocity < 0x100 && across <= selectedAcross && across > -0x24000) {
                    s32 halfLength = TRMPLINE(c)->mHalfWidth;
                    if (along > -halfLength && along < halfLength) {
                        selectedPosition = relativePosition;
                        selectedVelocity = velocity;
                        bestObj = sTrmpJumpers[jumperIndex];
                        selectedAcross = across;
                        selectedAlong = along;
                    }
                }
            }
        }
        jumperIndex++;
    } while (jumperIndex < 5);

    if (bestObj != 0) {
        TRMPLINE(c)->mContactPoint.x = selectedPosition.x;
        TRMPLINE(c)->mContactPoint.y = selectedPosition.y;
        TRMPLINE(c)->mContactPoint.z = selectedPosition.z;
        TRMPLINE(c)->mContactMotion.x = selectedVelocity.x;
        TRMPLINE(c)->mContactMotion.y = selectedVelocity.y;
        TRMPLINE(c)->mContactMotion.z = selectedVelocity.z;

        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        edgeRatio = cstd::fdiv(absoluteAlong, TRMPLINE(c)->mHalfWidth);
        halfLength = TRMPLINE(c)->mHalfWidth;
        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
        if (offsetTimesHalfLength < 0x400000) {
            ((Contact *)bestObj)->contactState = 2;
            D3D_PlaySePannedLoop(0x1B1, TRMPLINE(c)->mCenter.x,
                                 (s32)(sTrmpLineWeakPitchEnd * edgeRatio +
                                       sTrmpLineWeakPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        } else {
            ((Contact *)bestObj)->contactState = 1;
            D3D_PlaySePannedLoop(0x1AE, TRMPLINE(c)->mCenter.x,
                                 (s32)(sTrmpLineStrongPitchEnd * edgeRatio +
                                       sTrmpLineStrongPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        }
        ((Contact *)bestObj)->lineNormal.x = TRMPLINE(c)->mNormal.x;
        ((Contact *)bestObj)->lineNormal.y = TRMPLINE(c)->mNormal.y;
        ((Contact *)bestObj)->lineNormal.z = TRMPLINE(c)->mNormal.z;
        ((Contact *)bestObj)->alongLine = selectedAlong;
        ((Contact *)bestObj)->lineHalfLength = TRMPLINE(c)->mHalfWidth;
        TRMPLINE(c)->mStatus = 3;
        TrmpLine_PointDistance(c, &TRMPLINE(c)->mContactPoint, &TRMPLINE(c)->mContactMotion);
        TrmpLine_SetStateSettle(c);
        return;
    }

    TRMPLINE(c)->mLifeTimer = 0x258;
    TRMPLINE(c)->mStatus = 1;
    TrmpLine_SetStateFlex(c);
}
}

namespace s020d09e0 {
struct V3 {
    int z, y, x;
};
extern "C" extern void func_020553a4(void *p);
extern "C" extern void func_0203cd80(struct V3 *out, int a, int b);
extern "C" extern void TrmpLine_Draw(char *p);
extern "C" {
extern char data_0209b3ec[];
}
// @symbol TrmpLine_DrawAll
extern "C" void TrmpLine_DrawAll(void) {
    struct V3 s;
    *(volatile int *)0x4000440 = 2;
    func_020553a4(data_0209b3ec);
    s.z = 0;
    s.y = 0;
    s.x = 0xfffff008;
    func_0203cd80(&s, -0x2000, 0xfffff008);
    *(volatile int *)0x40004c8 = (((short)s.z >> 3) & 0x3ff) | ((((short)s.y >> 3) & 0x3ff) << 10) |
                                 ((((short)s.x >> 3) & 0x3ff) << 20) | 0x40000000;
    *(volatile int *)0x40004cc = 0x40007fff;
    int i;
    char *p = (char *)sTrmpLines;
    for (i = 0; i < 4; i++) {
        if (TRMPLINE(p)->mStatus != 0) {
            TrmpLine_Draw(p);
        }
        p += 0x32c;
    }
}
}

namespace s020d0ac0 {
extern "C" extern void TrmpLine_Update(void *c);
// @symbol TrmpLine_UpdateAll
extern "C" void TrmpLine_UpdateAll(void) {
    int i = 0;
    char *p = (char *)sTrmpLines;
    do {
        if (TRMPLINE(p)->mStatus != 0)
            TrmpLine_Update(p);
        i++;
        p += 0x32c;
    } while (i < 4);
}
}

namespace s020d0b04 {

// @symbol TrmpLine_ClearAll
extern "C" void TrmpLine_ClearAll(void) {
    int i = 0;
    unsigned char *p = (unsigned char *)sTrmpLines;
    int r0 = i;
    do {
        i++;
        TRMPLINE(p)->mStatus = r0;
        p += 0x32c;
    } while (i < 3);
}
}

namespace s020d0b2c {
extern "C" {
extern unsigned int func_02045a50(const void *src, unsigned int size);
extern char sTrmpLineTex[];
extern void *sTrmpLineTexAddr;
extern char sTrmpLinePltt[];
extern void *sTrmpLinePlttAddr;
}
// @symbol TrmpLine_LoadTextures
extern "C" void TrmpLine_LoadTextures(void) {
    sTrmpLineTexAddr = (void *)dExtSimpleModel_c::LoadTextureToVram(sTrmpLineTex, 0x400);
    sTrmpLinePlttAddr = (void *)func_02045a50(sTrmpLinePltt, 0x20);
}
}

namespace s020d0b78 {
extern "C" {
extern short sTrmpStartLinePoints[];
}
extern "C" {
extern short sTrmpStartLine[];
}
extern "C" extern void TrmpLine_Build(short *g, short *a, short *b);
// @symbol TrmpLine_CreateStartLineT2
extern "C" void TrmpLine_CreateStartLineT2(void) {
    short a[2];
    short b[2];
    a[0] = 0x10;
    a[1] = 0xb0;
    b[0] = 0xf0;
    b[1] = 0xb0;
    TrmpLine_Build(sTrmpStartLinePoints, a, b);
    // sTrmpStartLine is the slot's +0x27c: mBouncesLeft, mBounceCount, mSlotIndex.
    sTrmpStartLine[0x54] = 3;
    sTrmpStartLine[0x55] = 0;
    sTrmpStartLine[0x51] = 3;
}
}

namespace s020d0bd8 {
extern "C" {
extern short sTrmpStartLinePoints[];
}
extern "C" {
extern short sTrmpStartLine[];
}
extern "C" extern void TrmpLine_Build(short *g, short *a, short *b);
// @symbol TrmpLine_CreateStartLineT1
extern "C" void TrmpLine_CreateStartLineT1(void) {
    short a[2];
    short b[2];
    a[0] = 0x18;
    a[1] = 0xb0;
    b[0] = 0xe8;
    b[1] = 0xb0;
    TrmpLine_Build(sTrmpStartLinePoints, a, b);
    // sTrmpStartLine is the slot's +0x27c: mBouncesLeft, mBounceCount, mSlotIndex.
    sTrmpStartLine[0x54] = 3;
    sTrmpStartLine[0x55] = 0;
    sTrmpStartLine[0x51] = 3;
}
}

namespace s020d0c38 {
/*
 * Try to place a new line segment between two 16-bit points.
 * Rejects an X span shorter than 8, a zero direction, or a normalized
 * direction whose absolute X component is below 1401. When the full
 * vector is shorter than 0x30000, rescale the direction using a length
 * with Y weighted by 0xc00 (0.75), then rewrite the endpoints around
 * the midpoint. Then find the first free slot (mStatus == 0)
 * among the three, rejecting it if the
 * segment crosses any active (state 1) slot's own segment (mStart/mEnd),
 * and initialises the slot through TrmpLine_Build, returning its
 * address (0 when none is free).
 *
 * Shape notes: the midpoint pair needs named x<<12 / y<<12 temps so the
 * y shift lands in place; the re-scale block reads direction[1] inline
 * at both the multiply and the two-word copy into scaledDirection
 * (a named copy of it takes r4 ahead of the umull low word).
 */
extern "C" extern void func_0203b958(s16 *o, s16 *a, s16 *b);
extern "C" extern int func_0203d434(int *in);
extern "C" extern int Vec2_Len(int *v);
extern "C" extern void func_0203d630(int *p, int m);
extern "C" extern void func_0203d704(int *o, int *a, int *b);
extern "C" extern void Vec2_Sub(int *o, int *a, int *b);
extern "C" extern void TrmpLine_Build(s16 *slot, s16 *a, s16 *b);

extern "C" {
extern s16 data_ov006_02140cb4[];
}
extern "C" {
extern s16 data_ov006_02140cb6[];
}
extern "C" {
extern s16 data_ov006_02140cae[];
}

// @symbol TrmpLine_Create
extern "C" int TrmpLine_Create(s16 *startPoint, s16 *endPoint) {
    s16 deltaPixels[2];
    int delta[2];
    int direction[2];
    int midpoint[2];
    int scaledDirection[2];
    int adjustedStart[2];
    int adjustedEnd[2];
    s16 testDirection[2];
    s16 firstOffset[2];
    s16 secondOffset[2];
    s16 otherDirection[2];
    s16 firstEndpointOffset[2];
    s16 secondEndpointOffset[2];
    s16 startCopy[2];
    s16 endCopy[2];
    s16 deltaX;
    s16 deltaY;
    int absoluteX;
    int slotIndex;
    int otherSlotIndex;
    char *candidateSlot;
    char *p;
    int firstCross;
    int secondCross;
    long long weightedYProduct;
    int weightedY;
    int scaledDirectionLength;
    int slotOffset;
    s16 *newLine;

    func_0203b958(deltaPixels, startPoint, endPoint);

    deltaX = deltaPixels[0];
    if (deltaX < 0)
        absoluteX = (s16)(-deltaX);
    else
        absoluteX = deltaX;
    if (absoluteX < 8)
        return 0;

    deltaY = deltaPixels[1];
    delta[0] = (int)deltaX << 12;
    delta[1] = (int)deltaY << 12;
    direction[0] = ((int)deltaX >> 1) << 12;
    direction[1] = ((int)deltaY >> 1) << 12;

    {
        int ay = (s16)startPoint[1];
        int by = (s16)endPoint[1];
        int ax = (s16)startPoint[0];
        int bx = (s16)endPoint[0];
        int y = (ay + by) >> 1;
        int x = (ax + bx) >> 1;
        int x12 = x << 12;
        int y12 = y << 12;
        midpoint[0] = x12;
        midpoint[1] = y12;
    }

    if (func_0203d434(direction) == 0)
        return 0;

    {
        int absoluteDirectionX = direction[0];
        if (absoluteDirectionX < 0)
            absoluteDirectionX = -absoluteDirectionX;
        if (absoluteDirectionX < 1401)
            return 0;
    }

    if (Vec2_Len(delta) < 0x30000) {
        weightedYProduct = (long long)direction[1] * 0xc00 + 0x800;
        weightedY = (int)(weightedYProduct >> 12);
        scaledDirection[0] = direction[0];
        scaledDirection[1] = direction[1];
        scaledDirection[1] = weightedY;
        scaledDirectionLength = Vec2_Len(scaledDirection);
        func_0203d630(direction, cstd::fdiv(0x18000, scaledDirectionLength));

        func_0203d704(adjustedStart, midpoint, direction);
        Vec2_Sub(adjustedEnd, midpoint, direction);

        startPoint[0] = (s16)(adjustedStart[0] >> 12);
        startPoint[1] = (s16)(adjustedStart[1] >> 12);
        endPoint[0] = (s16)(adjustedEnd[0] >> 12);
        endPoint[1] = (s16)(adjustedEnd[1] >> 12);
    }

    candidateSlot = (char *)sTrmpLines;
    slotIndex = 0;
    do {
        if (TRMPLINE(candidateSlot)->mStatus == 0) {
            p = (char *)sTrmpLines;
            otherSlotIndex = 0;
            do {
                if (slotIndex != otherSlotIndex) {
                    if (TRMPLINE(p)->mStatus == 1) {
                        func_0203b958(testDirection, endPoint, startPoint);
                        func_0203b958(firstOffset, TRMPLINE(p)->mStart, startPoint);
                        func_0203b958(secondOffset, TRMPLINE(p)->mEnd, startPoint);

                        {
                            s16 ax = testDirection[0];
                            s16 ay = testDirection[1];
                            s16 bx = firstOffset[0];
                            s16 by = firstOffset[1];
                            s16 cy = secondOffset[1];
                            firstCross = (int)ax * (int)by - (int)ay * (int)bx;
                            {
                                s16 cx = secondOffset[0];
                                secondCross = (int)ax * (int)cy - (int)ay * (int)cx;
                            }
                        }
                        if (firstCross * secondCross <= 0) {
                            func_0203b958(otherDirection, TRMPLINE(p)->mEnd, TRMPLINE(p)->mStart);
                            testDirection[0] = otherDirection[0];
                            testDirection[1] = otherDirection[1];
                            func_0203b958(firstEndpointOffset, startPoint, TRMPLINE(p)->mStart);
                            firstOffset[0] = firstEndpointOffset[0];
                            firstOffset[1] = firstEndpointOffset[1];
                            func_0203b958(secondEndpointOffset, endPoint, TRMPLINE(p)->mStart);
                            secondOffset[0] = secondEndpointOffset[0];
                            secondOffset[1] = secondEndpointOffset[1];

                            {
                                s16 ax = testDirection[0];
                                s16 ay = testDirection[1];
                                s16 bx = firstOffset[0];
                                s16 by = firstOffset[1];
                                s16 cy = secondOffset[1];
                                firstCross = (int)ax * (int)by - (int)ay * (int)bx;
                                {
                                    s16 cx = secondOffset[0];
                                    secondCross = (int)ax * (int)cy - (int)ay * (int)cx;
                                }
                            }
                            if (firstCross * secondCross <= 0)
                                return 0;
                        }
                    }
                }
                otherSlotIndex++;
                p += 0x32c;
            } while (otherSlotIndex < 4);

            startCopy[0] = startPoint[0];
            startCopy[1] = startPoint[1];
            endCopy[0] = endPoint[0];
            endCopy[1] = endPoint[1];
            slotOffset = slotIndex * 0x32c;
            newLine = (s16 *)((int)sTrmpLines + slotOffset);
            TrmpLine_Build(newLine, startCopy, endCopy);
            // sTrmpLines + 0x324, 0x326 and 0x31e, addressed through their own
            // symbols: mBouncesLeft, mBounceCount and mSlotIndex.
            *(s16 *)((int)data_ov006_02140cb4 + slotOffset) = 1;
            *(s16 *)((int)data_ov006_02140cb6 + slotOffset) = 0;
            *(s16 *)((int)data_ov006_02140cae + slotOffset) = (s16)slotIndex;
            return (int)newLine;
        }
        slotIndex++;
        candidateSlot += 0x32c;
    } while (slotIndex < 3);

    return 0;
}
}

/* The four line slots. mwcc builds them in this file's static initializer
   with __cxa_vec_ctor and registers their cleanup thunk, __arraydtor. */
TrmpLine sTrmpLines[4];

/* The constructor and destructor are inline: mwcc emits a used inline body
   at the end of the file, after the static initializer and __arraydtor, which
   is where the cartridge has D1 and then C1. Out-of-line bodies would come
   ahead of __arraydtor and bring a D2/C2 pair with no ROM home. */
// @symbol _ZN8TrmpLineD1Ev
inline TrmpLine::~TrmpLine() {}

// @symbol _ZN8TrmpLineC1Ev
inline TrmpLine::TrmpLine() {
    mStatus = 0;
}

/* The ring model handles. The initializer builds them after sTrmpLines,
   in this order. */
TrmpItemRingModelFile sTrmpItemRingAModelFile(0x21a);
TrmpItemRingModelFile sTrmpItemRingBModelFile(0x21b);
TrmpItemRingModelFile sTrmpItemRingCModelFile(0x21c);
