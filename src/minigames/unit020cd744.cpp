//cpp
/* The ov006 stylus-drawn trampolines, 0x020cd744..0x020d1018.
 *
 * A line slot is a 4x4 patch: positions at +0x5c, normals at +0x1dc, packed
 * normals at +0x2dc. Four 0x32c-byte slots live in sTrmpLines. Trampoline2's
 * floating rings are 0x1d0-byte TrmpItem records in the scene's array at
 * sTrmpItems. No RTTI names a type in this run.
 *
 * `#pragma defer_codegen off` keeps the bodies in ROM-ascending order. The file
 * owns its static initializer, its .ctor word and its .bss: the line slots, the
 * three ring model handles, the counters, sTrmp2MarioPtrs (read only by
 * d_mg_trmpln2_mario) and the function-local unit vectors.
 *
 * Leftovers:
 * - TrmpItem_Init's fifth parameter is short in its definition and int at the
 *   TrmpItem_Spawn call site: retail's callee loads it with ldrsh and the
 *   caller stores the full word, so the two spellings are both load-bearing.
 * - The pmf_* records are {function, adjustment} pairs. The dispatch reads the
 *   pair and calls through an object overlay; spelling it as a real
 *   `void (TrmpItem::*StateFn)()` member pointer emits the ARM this-adjustment
 *   and no longer fits the ROM function (measured on TrmpItem_Update).
 * - Vector3 locals emit an empty destructor, licensed as a deadstrip duplicate
 *   of arm9:0x020072c0. TrmpLine_Build's never-written stack slot is held by
 *   Spare4, whose empty destructor is deadstripped as compiler-only output.
 * - The vector helpers (Vec3_Sub, Vec2_Sub and the rest) are one-function C
 *   sources in src/ with no owning header, so they are declared here.
 */

#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g3imm.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/hw/registers.h"
#include "MgCamera.h"
#include "decl_common.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx_cp.h"
#include "TrmpLine.h"
#include "TrmpItem.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "math/Approach.h"
#include "math/Quaternion.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "MgCamera.h"

/* Typed access at the two boundaries that still take a raw pointer: the
   void * entry points decl_common.h declares (MotionHover, MotionSway,
   UpdateWithJumper) and the char * slot walk in TrmpLine_Create. */
#define TRMPLINE(p) ((TrmpLine *)(p))
#define TRMPITEM(p) ((TrmpItem *)(p))

#pragma defer_codegen off

/* The jumper a line bounces: a trampoline Mario, read through the five virtual
   slots of the vtable at 0x0213b2c4 (dMgTrmpl3DMario_c) or 0x0213b2e0
   (dMgTrmpln2Mario_c). Only the consumed prefix is described. */
struct TrampolineObjectCallbacks {
    virtual Vector3 *GetPos();
    virtual Vector3 *GetPrevPos();
    virtual Vector3 *GetVelocity();
    virtual int IsActive();
    virtual void Clear();
};

/* The two item counters on the jumper, named as in dMgTrmpln2Mario_c.h. */
struct TrampolineJumperCounts : TrampolineObjectCallbacks {
    char pad_04[0x20 - 0x04];
    u16 mItemHitCount;  /* +0x20 items hit; doubles the score */
    s16 mItemPushCount; /* +0x22 item pushes this frame */
};

/* The jumper fields a caught line writes: the same word prefix as the two
   Mario records (contact normal at +0x04, offset, half width, contact kind). */
struct TrampolineContactView : TrampolineObjectCallbacks {
    Vector3 lineNormal; /* +0x04 */
    int alongLine;      /* +0x10 dot with the line direction */
    int lineHalfLength; /* +0x14 */
    u16 contactState;   /* +0x18 0 none, 1 hit, 2 hard hit, 3 line end */
};

/* A 2D point or projection: two Fix12 words. Deliberately POD, not Vector2,
   whose declared destructor would make every local here register a cleanup. */
struct V2 {
    int x, y;
};

/* A line patch vertex: the same three-Fix12i spelling as TrmpLine::Vtx. */
typedef TrmpLine::Vtx Vec3;

/* The object a state record is installed in, seen only as its first two words:
   both update functions call through a member pointer read from the record. */
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};

/* The three ring model handles (files 0x21a..0x21c), defined at the end of the
   file. The wrapper's constructor and destructor alias the ROM resource-family
   pair func_02017acc / func_02017ab4 (see the manifest's
   undefined_symbol_aliases). */
struct TrmpItemRingModelFile : SharedFilePtr {
    TrmpItemRingModelFile(u32 fileID);
    ~TrmpItemRingModelFile();
};
extern TrmpItemRingModelFile sTrmpItemRingAModelFile;
extern TrmpItemRingModelFile sTrmpItemRingBModelFile;
extern TrmpItemRingModelFile sTrmpItemRingCModelFile;

/* The four line slots, defined beside TrmpLine's constructor below. */
extern TrmpLine sTrmpLines[4];

/* A state record: the {function, adjustment} member pointer the setters copy
   into TrmpItem::mState and TrmpLine::mState, and the update functions call.
   The array spelling is load-bearing: splitting it into two int members
   changes the setters' copy shape (2 words differ in 14 functions). */
struct StateRecord {
    int w[2];
};

extern "C" {

/* This unit's own globals (its .bss claim). sTrmp2MarioPtrs is read only by
   d_mg_trmpln2_mario. */
void *sTrmpLinePlttAddr;
void *sTrmpLineTexAddr;
int sTrmpItemsHit;
int sTrmpItemsActive;
char *sTrmpItems;
int sTrmp2Score;
int sTrmpItemCount;
void *sTrmp2MarioPtrs[5];
void *sTrmpJumpers[5];

/* Other units' data. */
extern MgCamera *sMgSubCamera;
extern int sTrmpItemHitsOff; /* d_mg_trmpln2_mario: stops item scoring */
extern int sTrmpItemPatrolLimits[];
extern int sTrmpItemPatrolSpeed[];
extern int sTrmpLineFlexWeights[];
extern unsigned short sTrmpLineDiffuse[];
extern unsigned short sTrmpLineAmbient[];
extern int sTrmpLineTexCoords[];
extern char sTrmpLineTex[];
extern char sTrmpLinePltt[];
extern BTA_File sTrmpItemTextureAnim;
extern int sTrmpLinePushWeakPitchMid;
extern int sTrmpLinePushWeakPitchEnd;
extern int sTrmpLinePushStrongPitchMid;
extern int sTrmpLinePushStrongPitchEnd;
extern int sTrmpLineWeakPitchEnd;
extern int sTrmpLineWeakPitchMid;
extern int sTrmpLineStrongPitchEnd;
extern int sTrmpLineStrongPitchMid;
extern Vector3 data_020a0ebc;
extern Matrix4x3 data_0209b3ec; /* the view matrix, Graphics3D.cpp */
extern int data_02092768[4];
extern int data_020a0db0;

/* The state records installed by the setters, compared by the updates. */
extern StateRecord pmf_TrmpItem_GrowScale;
extern StateRecord pmf_TrmpItem_StateAppear;
extern StateRecord pmf_TrmpItem_StateAppear_2;
extern StateRecord pmf_TrmpItem_GrowScale_2;
extern StateRecord pmf_TrmpItem_MotionSpinHover;
extern StateRecord pmf_TrmpItem_MotionSpinDrift;
extern StateRecord pmf_TrmpItem_MotionSpinSway;
extern StateRecord pmf_TrmpItem_MotionHover;
extern StateRecord pmf_TrmpItem_MotionSway;
extern StateRecord pmf_TrmpItem_MotionSpin;
extern StateRecord pmf_TrmpItem_MotionBob;
extern StateRecord pmf_TrmpItem_MotionPatrol;
extern StateRecord pmf_TrmpItem_MotionStill;
extern StateRecord pmf_TrmpLine_StateFade;
extern StateRecord pmf_TrmpLine_StateSettle;
extern StateRecord pmf_TrmpLine_StateFlex;

/* Other units' functions. */
void Jump2Popup_Spawn(int, int, int);
void D3D_PlaySePannedLoop(int, int, int);
void Trmp2Spinner_InitAll(int);
void func_0203b958(short *, short *, short *);
int func_0203d524(V2 *, V2 *);
void func_0203d704(int *, int *, int *);
void func_0203cd80(int *, short);
void Matrix4x3_FromQuaternion(const Quaternion *, Matrix4x3 *);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, int, int, int);
unsigned int func_02045a50(const void *, unsigned int);

/* Vec3/Vector2 helpers: one-function C sources in src/, no owning header. */
void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);
void Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *);
void Vec3_MulScalar(Vector3 *, const Vector3 *, int);
void Vec3_MulScalarInPlace(Vector3 *, int);
void VEC_CrossProduct(const Vector3 *, const Vector3 *, Vector3 *);
void VEC_Normalize(const Vector3 *, Vector3 *);
int NormalizeVec3IfNonZero(Vector3 *);
void Vec2_Sub(int *, int *, int *);

/* This file's own bodies, declared so ROM order can stay ascending. */
void TrmpItem_MotionHover(void *);
void TrmpItem_MotionSway(void *);
void TrmpItem_SetMotionPatrol(TrmpItem *);
void TrmpItem_SetMotionBob(TrmpItem *);
void TrmpItem_SetMotionSpin(TrmpItem *);
void TrmpItem_SetMotionStill(TrmpItem *);
void TrmpItem_SetMotionSway(TrmpItem *);
void TrmpItem_SetMotionSpinDrift(TrmpItem *);
void TrmpItem_SetMotionSpinSway(TrmpItem *);
void TrmpItem_SetMotionSpinHover(TrmpItem *);
void TrmpItem_StartAppear(TrmpItem *);
void TrmpItem_Hide(TrmpItem *);
void TrmpItem_Load(TrmpItem *);
void TrmpItem_UpdateMatrix(TrmpItem *);
void TrmpItem_Update(TrmpItem *);
void TrmpItem_ReflectOffLine(TrmpItem *item, TrampolineObjectCallbacks *jumper);
int TrmpItem_HitTest2D(TrmpItem *item, TrampolineObjectCallbacks *jumper, int arg);
void TrmpItem_ProjectOnLine(TrmpItem *item, TrampolineObjectCallbacks *jumper, V2 *current,
                            V2 *previous);
void TrmpItem_ProjectToScreen(TrmpItem *item, unsigned short score);
void TrmpLine_ComputeNormals(TrmpLine *line);
void TrmpLine_PointDistance(TrmpLine *line, void *referencePoint, const Vector3 *offset);
void TrmpLine_Draw(TrmpLine *line);
void TrmpLine_SetStateFade(TrmpLine *);
void TrmpLine_SetStateSettle(TrmpLine *);
void TrmpLine_SetStateFlex(TrmpLine *);
int TrmpLine_PushJumper(TrmpLine *);
void TrmpLine_Build(TrmpLine *line, short *startPoint, short *endPoint);
void TrmpLine_Update(void *);
void TrmpLine_UpdateAll(void);
void TrmpLine_ClearAll(u32);
void TrmpLine_CreateStartLineT2(void);
void TrmpLine_CreateStartLineT1(void);
}

extern "C" void TrmpItem_GrowScale(TrmpItem *c) {
    ApproachLinear(c->mScale.x, 0x1200, 0x80);
    ApproachLinear(c->mScale.z, 0x1200, 0x80);
    ApproachLinear(c->mScale.y, 0x20000, 0x400);
    if (ApproachLinear2(*(int *)&c->mAlpha, 0, 1) == 0)
        return;
    c->mActive = 0;
    ApproachLinear(sTrmpItemsActive, 0, 1);
}

extern "C" void TrmpItem_ProjectToScreen(TrmpItem *c, unsigned short scoreValue) {
    int screenX, screenY;
    if (c->mPos.y > 0) {
        Camera_ProjectToScreen(sMgSubCamera, &c->mPos, &screenX, &screenY);
        screenY = screenY - (Mg_GetTopScreenYOffset() + 0xc0);
    } else {
        Camera_ProjectToScreen((MgCamera *)sMgMainCamera, &c->mPos, &screenX, &screenY);
    }
    Jump2Popup_Spawn(screenX << 0xc, screenY << 0xc, (short)scoreValue);
    *(StateRecord *)c = pmf_TrmpItem_GrowScale;
}

extern "C" void TrmpItem_MotionSpinHover(TrmpItem *arg) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28((int *)&arg->mForward, 0x100);
    func_0203ce80(&normalCopy, &arg->mForward);
    func_0203cf00(&tangent, &arg->mForward, &sUnitZ);
    arg->mSide.x = tangent.x;
    arg->mSide.y = tangent.y;
    arg->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &arg->mSide);
    Quaternion_FromVector3(arg->mRotation, &sUnitY, &arg->mForward);
    Quaternion_Normalize(arg->mRotation);
    TrmpItem_MotionHover(arg);
}

extern "C" void TrmpItem_SetMotionSpinHover(TrmpItem *c) {
    c->mPhase = 0;
    *(StateRecord *)c = pmf_TrmpItem_MotionSpinHover;
}

extern "C" void TrmpItem_MotionSpinDrift(TrmpItem *self) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy, tangent, tangentCopy;

    func_0203cc28((int *)&self->mForward, 0x100);
    func_0203ce80(&normalCopy, &self->mForward);
    func_0203cf00(&tangent, &self->mForward, &sUnitZ);
    self->mSide.x = tangent.x;
    self->mSide.y = tangent.y;
    self->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &self->mSide);
    Quaternion_FromVector3(self->mRotation, &sUnitY, &self->mForward);
    Quaternion_Normalize(self->mRotation);
}

extern "C" void TrmpItem_SetMotionSpinDrift(TrmpItem *p) {
    *(StateRecord *)p->mState = pmf_TrmpItem_MotionSpinDrift;
}

extern "C" void TrmpItem_MotionSpinSway(TrmpItem *arg) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28((int *)&arg->mForward, 0x100);
    func_0203ce80(&normalCopy, &arg->mForward);
    func_0203cf00(&tangent, &arg->mForward, &sUnitZ);
    arg->mSide.x = tangent.x;
    arg->mSide.y = tangent.y;
    arg->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &arg->mSide);
    Quaternion_FromVector3(arg->mRotation, &sUnitY, &arg->mForward);
    Quaternion_Normalize(arg->mRotation);
    TrmpItem_MotionSway(arg);
}

extern "C" void TrmpItem_SetMotionSpinSway(TrmpItem *c) {
    c->mPhase = 0x2000;
    *(StateRecord *)c = pmf_TrmpItem_MotionSpinSway;
}

extern "C" void TrmpItem_MotionHover(void *arg0) {
    TRMPITEM(arg0)->mPhase += 0x100;
    if (TRMPITEM(arg0)->mPhase & 0x8000) {
        TRMPITEM(arg0)->mVel.y = 0xc00;
    } else {
        TRMPITEM(arg0)->mVel.y = -0xc00;
    }
}

extern "C" void TrmpItem_SetMotionHover(TrmpItem *c) {
    c->mPhase = 0;
    *(StateRecord *)c = pmf_TrmpItem_MotionHover;
}

extern "C" void TrmpItem_MotionSway(void *self) {
    int distanceFromCenterY;
    int xSpeed = 0x400;

    TRMPITEM(self)->mPhase += 0x40;

    distanceFromCenterY = TRMPITEM(self)->mPos.y - 0x74000;
    if (distanceFromCenterY < 0)
        distanceFromCenterY = -distanceFromCenterY;
    xSpeed += FX_Div(distanceFromCenterY, 0x4c000) >> 3;

    if (TRMPITEM(self)->mPhase & 0x4000)
        TRMPITEM(self)->mVel.x = xSpeed;
    else
        TRMPITEM(self)->mVel.x = -xSpeed;
}

extern "C" void TrmpItem_SetMotionSway(TrmpItem *c) {
    c->mPhase = 0x2000;
    *(StateRecord *)c = pmf_TrmpItem_MotionSway;
}

extern "C" void TrmpItem_MotionSpin(TrmpItem *self) {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy, tangent, tangentCopy;

    func_0203cc28((int *)&self->mForward, 0x100);
    func_0203ce80(&normalCopy, &self->mForward);
    func_0203cf00(&tangent, &self->mForward, &sUnitZ);
    self->mSide.x = tangent.x;
    self->mSide.y = tangent.y;
    self->mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &self->mSide);
    Quaternion_FromVector3(self->mRotation, &sUnitY, &self->mForward);
    Quaternion_Normalize(self->mRotation);
}

extern "C" void TrmpItem_SetMotionSpin(TrmpItem *c) {
    c->mVel.y = 0;
    *(StateRecord *)c = pmf_TrmpItem_MotionSpin;
}

extern "C" void TrmpItem_MotionBob(TrmpItem *item) {
    int y = item->mPos.y;
    if (y > 0x80000) {
        item->mVel.y = -0x1000;
        return;
    }
    if (y < -0x60000)
        item->mVel.y = 0x1000;
}

extern "C" void TrmpItem_SetMotionBob(TrmpItem *c) {
    c->mVel.y = 0x1000;
    *(StateRecord *)c = pmf_TrmpItem_MotionBob;
}

extern "C" void TrmpItem_MotionPatrol(TrmpItem *item) {
    int itemType = item->mSizeIdx;
    if (item->mPos.x > sTrmpItemPatrolLimits[itemType]) {
        item->mVel.x = -sTrmpItemPatrolSpeed[itemType];
        return;
    }
    if (item->mPos.x < -sTrmpItemPatrolLimits[itemType]) {
        item->mVel.x = sTrmpItemPatrolSpeed[itemType];
    }
}

extern "C" void TrmpItem_SetMotionPatrol(TrmpItem *c) {
    int itemType = c->mSizeIdx;
    c->mVel.x = sTrmpItemPatrolSpeed[itemType];
    *(StateRecord *)c = pmf_TrmpItem_MotionPatrol;
}

extern "C" void TrmpItem_MotionStill(void) {}

extern "C" void TrmpItem_SetMotionStill(TrmpItem *p) {
    *(StateRecord *)p->mState = pmf_TrmpItem_MotionStill;
}

extern "C" void TrmpItem_StateAppear(TrmpItem *c) {
    int yScaleReady = ApproachLinear(c->mScale.y, 0x1000, 0xc0);
    int xScaleReady = ApproachLinear(c->mScale.x, 0x1000, 0x180);
    int zScaleReady = ApproachLinear(c->mScale.z, 0x1000, 0x180);
    int responseScaleReady = ApproachLinear(c->mAppearScale, 0x800, 0x60);
    if (yScaleReady == 0)
        return;
    if (xScaleReady == 0)
        return;
    if ((zScaleReady & responseScaleReady) == 0)
        return;
    switch (c->mMotionKind) {
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

extern "C" void TrmpItem_StartAppear(TrmpItem *c) {
    ApproachLinear(sTrmpItemsActive, sTrmpItemCount, 1);
    c->mAlpha = 0x1f;
    c->mVel.x = 0;
    c->mVel.y = 0;
    c->mVel.z = 0;
    *(StateRecord *)c = pmf_TrmpItem_StateAppear_2;
}

extern "C" void TrmpItem_ReflectOffLine(TrmpItem *item, TrampolineObjectCallbacks *jumper) {
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
            if (item->mState[1] == growState[1])
                return;
            if (stateWord == 0)
                return;
        }
    }

    position = jumper->GetPos();
    velocity = jumper->GetVelocity();

    pushSum = data_020a0ebc;
    Vec3_Sub(&relativePosition, position, &item->mPos);
    positiveEndOffset = relativePosition;
    negativeEndOffset = relativePosition;
    itemToJumper = relativePosition;

    Vec3_MulScalar(&halfSpan, &item->mSide, sTrmpItemHitWidths[item->mSizeIdx]);
    VEC_Add(&negativeEndOffset, &halfSpan, &negativeEndOffset);
    VEC_Subtract(&positiveEndOffset, &halfSpan, &positiveEndOffset);

    negativeEndOffset.y += 0xE000;
    positiveEndOffset.y += 0xE000;
    negativeEndOffset.y >>= 1;
    positiveEndOffset.y >>= 1;

    negativeEndDistance = VEC_Mag(&negativeEndOffset);
    positiveEndDistance = VEC_Mag(&positiveEndOffset);
    along = VEC_DotProduct(&itemToJumper, &item->mSide);
    across = VEC_DotProduct(&itemToJumper, &item->mForward);

    if (negativeEndDistance < 0xE000 && NormalizeVec3IfNonZero(&negativeEndOffset) != 0) {
        Vec3_MulScalarInPlace(&negativeEndOffset, 0xE000 - negativeEndDistance);
        negativeEndOffset.y <<= 1;
        Vec3_MulScalar(&negativeEndCorrection, &negativeEndOffset, item->mAppearScale);
        VEC_Add(position, &negativeEndCorrection, position);
        VEC_Add(&pushSum, &negativeEndOffset, &pushSum);
        ((TrampolineJumperCounts *)jumper)->mItemPushCount += 1;
    } else if (positiveEndDistance < 0xE000 && NormalizeVec3IfNonZero(&positiveEndOffset) != 0) {
        Vec3_MulScalarInPlace(&positiveEndOffset, 0xE000 - positiveEndDistance);
        positiveEndOffset.y <<= 1;
        Vec3_MulScalar(&positiveEndCorrection, &positiveEndOffset, item->mAppearScale);
        VEC_Add(position, &positiveEndCorrection, position);
        VEC_Add(&pushSum, &positiveEndOffset, &pushSum);
        ((TrampolineJumperCounts *)jumper)->mItemPushCount += 1;
    }

    Vec3_MulScalar(&endVelocityChange, &pushSum, 0x100);
    VEC_Add(velocity, &endVelocityChange, velocity);

    {
        int x = velocity->x;
        if (x < -0x1000)
            x = -0x1000;
        else if (x > 0x1000)
            x = 0x1000;
        velocity->x = x;
    }

    {
        int maxYVelocity = sTrmp2RiseSpeedMax;
        int minYVelocity = sTrmp2FallSpeed;
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
    if (along >= sTrmpItemHitWidths[item->mSizeIdx])
        return;

    Vec3_MulScalar(&normalVelocityChange, &item->mForward, 0x70);
    if (across > 0)
        Vec3_MulScalarInPlace(&normalVelocityChange, -0x1000);
    {
        int yChange = normalVelocityChange.y;
        if (yChange > 0x80)
            yChange = 0x80;
        normalVelocityChange.y = yChange;
    }
    VEC_Add(velocity, &normalVelocityChange, velocity);
}

/* The ROM re-reads sTrmpItems and recomputes the element address at each use;
   caching it in a local reallocates the whole loop (load-bearing). */
#define ELEM ((char *)sTrmpItems + itemIndex * 0x1d0)

extern "C" void TrmpItem_UpdateWithJumper(void *object, int arg1) {
    TrampolineJumperCounts *jumper = (TrampolineJumperCounts *)object;
    int itemIndex;
    int doubling;
    int scoreIncrement;
    int hitCount;
    int scoreDoublings;

    if (sTrmpItemHitsOff != 0)
        return;
    jumper->mItemPushCount = 0;

    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        if (TRMPITEM(ELEM)->mActive == 0)
            continue;
        TrmpItem_ReflectOffLine((TrmpItem *)ELEM, jumper);
        if (TrmpItem_HitTest2D((TrmpItem *)ELEM, jumper, arg1) == 0)
            continue;
        ApproachLinear(sTrmpItemsHit, 0x270f, 1);
        hitCount = jumper->mItemHitCount;
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
        Trmp2Spinner_InitAll(jumper->mItemHitCount);
        TrmpItem_ProjectToScreen((TrmpItem *)ELEM, (short)scoreIncrement);
        jumper->mItemHitCount++;
    }
}

// The caller supplies a third argument; this implementation does not read it.
extern "C" int TrmpItem_HitTest2D(TrmpItem *item, TrampolineObjectCallbacks *jumper, int) {
    struct V2 currentProjection, previousProjection, lineStart, lineEnd;
    struct V2 testDirection, firstOffset, secondOffset;
    struct V2 movementDirection, toEnd, toStart;
    int stateWord = *(int *)item;
    int currentSide, previousSide, endSide, startSide;
    int result;

    {
        StateRecord *appearState = &pmf_TrmpItem_StateAppear;
        StateRecord *growState;
        if ((stateWord == appearState->w[0] &&
             (item->mState[1] == appearState->w[1] || stateWord == 0)) ||
            (growState = &pmf_TrmpItem_GrowScale_2,
             stateWord == growState->w[0] &&
                 (item->mState[1] == growState->w[1] || stateWord == 0)))
            return 0;
    }

    result = 0;
    TrmpItem_ProjectOnLine(item, jumper, &currentProjection, &previousProjection);

    {
        int *halfLengths = sTrmpItemHitWidths;
        lineStart.x = -halfLengths[item->mSizeIdx];
        lineStart.y = 0;
        lineEnd.x = halfLengths[item->mSizeIdx];
        lineEnd.y = 0;
    }

    // The projections must straddle the line without lying on it together.
    Vec2_Sub((int *)&testDirection, (int *)&lineEnd, (int *)&lineStart);
    Vec2_Sub((int *)&firstOffset, (int *)&currentProjection, (int *)&lineStart);
    Vec2_Sub((int *)&secondOffset, (int *)&previousProjection, (int *)&lineStart);
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
    Vec2_Sub((int *)&movementDirection, (int *)&currentProjection, (int *)&previousProjection);
    testDirection = movementDirection;
    Vec2_Sub((int *)&toEnd, (int *)&lineEnd, (int *)&currentProjection);
    firstOffset = toEnd;
    Vec2_Sub((int *)&toStart, (int *)&lineStart, (int *)&currentProjection);
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

extern "C" void TrmpItem_ProjectOnLine(TrmpItem *item, TrampolineObjectCallbacks *jumper,
                                       V2 *currentProjection, V2 *previousProjection) {
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
    Vec3_Sub(&currentRelative, &currentPosition, &item->mPos);
    Vec3_Sub(&previousRelative, &previousPosition, &item->mPrevPos);
    // Only the current relative position receives this vertical offset.
    currentRelative.y += 0xc000;
    across = VEC_DotProduct(&item->mForward, &currentRelative);
    currentProjection->x = VEC_DotProduct(&item->mSide, &currentRelative);
    currentProjection->y = across;
    across = VEC_DotProduct(&item->mPrevForward, &previousRelative);
    previousProjection->x = VEC_DotProduct(&item->mPrevSide, &previousRelative);
    previousProjection->y = across;
}

extern "C" void TrmpItem_UpdateMatrix(TrmpItem *self) {
    Matrix4x3 rotation;
    Matrix4x3_FromQuaternion(reinterpret_cast<const Quaternion *>(self->mRotation), &rotation);
    Matrix4x3_FromTranslation(&data_020a0e68, self->mPos.x, self->mPos.y, self->mPos.z);
    MTX_Concat43(&rotation, &data_020a0e68, &data_020a0e68);
    self->mModel->mat4x3 = data_020a0e68;
    self->mTexAnim.Update(self->mModel->data);
    self->mModel->ApplyOpacity(self->mAlpha, 0);
    self->mModel->Render(&self->mScale);
}

extern "C" void TrmpItem_Update(TrmpItem *item) {
    PMF *state = (PMF *)item;
    (((C *)item)->**state)();
    VEC_Add(&item->mPos, &item->mVel, &item->mPos);
    item->mTexAnim.Advance();
    item->mPrevPos.x = item->mPos.x;
    item->mPrevPos.y = item->mPos.y;
    item->mPrevPos.z = item->mPos.z;
    item->mPrevForward.x = item->mForward.x;
    item->mPrevForward.y = item->mForward.y;
    item->mPrevForward.z = item->mForward.z;
    item->mPrevSide.x = item->mSide.x;
    item->mPrevSide.y = item->mSide.y;
    item->mPrevSide.z = item->mSide.z;
}

extern "C" void TrmpItem_Init(TrmpItem *self, int *position, int *normal, int itemType,
                              short motionType) {
    /* The axis the starting rotation is built about, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);

    self->mPos.x = position[0];
    self->mPos.y = position[1];
    self->mPos.z = position[2];
    self->mForward.x = normal[0];
    self->mForward.y = normal[1];
    self->mForward.z = normal[2];
    /* Load-bearing: both component-pointer copies below. Plain field copies
       or a Vector3 assignment reallocate the whole function. */
    {
        int *normalComponents = (int *)&self->mForward;
        self->mSide.x = normalComponents[0];
        self->mSide.y = normalComponents[1];
        self->mSide.z = normalComponents[2];
    }
    func_0203cc28((int *)&self->mSide, -0x4000);
    self->mPrevPos.x = self->mPos.x;
    self->mPrevPos.y = self->mPos.y;
    self->mPrevPos.z = self->mPos.z;
    {
        int *cachedPosition = (int *)&self->mPrevPos;
        self->mSpawnPos.x = cachedPosition[0];
        self->mSpawnPos.y = cachedPosition[1];
        self->mSpawnPos.z = cachedPosition[2];
    }
    self->mPrevForward.x = self->mForward.x;
    self->mPrevForward.y = self->mForward.y;
    self->mPrevForward.z = self->mForward.z;
    self->mPrevSide.x = self->mSide.x;
    self->mPrevSide.y = self->mSide.y;
    self->mPrevSide.z = self->mSide.z;
    self->mRotation[0] = data_02092768[0];
    self->mRotation[1] = data_02092768[1];
    self->mRotation[2] = data_02092768[2];
    self->mRotation[3] = data_02092768[3];
    Quaternion_FromVector3(self->mRotation, &sUnitY, &self->mForward);
    self->mSizeIdx = itemType;
    self->mMotionKind = motionType;
    self->mActive = 1;
    switch (self->mSizeIdx) {
    case 0:
        self->mModel = &self->mRingA;
        break;
    case 1:
        self->mModel = &self->mRingB;
        break;
    case 2:
    default:
        self->mModel = &self->mRingC;
        break;
    }
    self->mScale.x = data_020a0ebc.x;
    self->mScale.y = data_020a0ebc.y;
    self->mScale.z = data_020a0ebc.z;
    self->mAppearScale = 0;
    self->mUnusedInit = 0x5a;
    TrmpItem_StartAppear(self);
}

extern "C" void TrmpItem_Hide(TrmpItem *p) {
    p->mActive = 0;
}

extern "C" void TrmpItem_Load(TrmpItem *c) {
    c->mRingA.SetFile((BMD_File *)sTrmpItemRingAModelFile.filePtr, 1, -1);
    c->mRingB.SetFile((BMD_File *)sTrmpItemRingBModelFile.filePtr, 1, -1);
    c->mRingC.SetFile((BMD_File *)sTrmpItemRingCModelFile.filePtr, 1, -1);
    c->mRingA.SetPolygonID(1);
    c->mRingB.SetPolygonID(2);
    c->mRingC.SetPolygonID(3);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)sTrmpItemRingAModelFile.filePtr, sTrmpItemTextureAnim);
    c->mTexAnim.SetFile(sTrmpItemTextureAnim, 0, 0x1000, 0);
    TrmpItem_Hide(c);
}

extern "C" void TrmpItem_UpdateMatricesAll(void) {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        TrmpItem *p = (TrmpItem *)(sTrmpItems + itemIndex * 0x1d0);
        if (p->mActive != 0)
            TrmpItem_UpdateMatrix(p);
    }
}

extern "C" void TrmpItem_UpdateAll(void) {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        TrmpItem *p = (TrmpItem *)(sTrmpItems + itemIndex * 0x1d0);
        if (p->mActive != 0)
            TrmpItem_Update(p);
    }
}

extern "C" void TrmpItem_HideAll(u32) {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++)
        TrmpItem_Hide((TrmpItem *)(sTrmpItems + itemIndex * 0x1d0));
    sTrmpItemsActive = 0;
    sTrmpItemsHit = 0;
    sTrmp2Score = 0;
}

extern "C" void TrmpItem_ReleaseFiles(void) {
    sTrmpItemRingAModelFile.Release();
    sTrmpItemRingBModelFile.Release();
    sTrmpItemRingCModelFile.Release();
}

extern "C" void TrmpItem_LoadAll(char *items, int count) {
    int itemIndex;
    dExtSimpleModel_c::LoadFile(sTrmpItemRingAModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingBModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingCModelFile);
    sTrmpItems = items;
    sTrmpItemCount = count;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++)
        TrmpItem_Load((TrmpItem *)(sTrmpItems + itemIndex * 0x1d0));
}

/* The call below needs the initializer's fifth parameter as a full word: retail
   stores the word here and the callee loads it with ldrsh, so the prototype
   this caller sees disagrees with the definition's short. Two declarations of
   one extern "C" function cannot share a scope, so the wide one is local. */
namespace trmpItemSpawn {
extern "C" void TrmpItem_Init(TrmpItem *item, int *position, int *normal, int itemType,
                              int motionType);
}

extern "C" char *TrmpItem_Spawn(int position, int *normal, int itemType, int motionType) {
    int itemIndex = 0;
    char *base;
    int itemOffset;
    if (sTrmpItemCount > 0) {
        base = sTrmpItems;
        do {
            if (((TrmpItem *)base)->mActive == 0) {
                itemOffset = itemIndex * 0x1d0;
                trmpItemSpawn::TrmpItem_Init((TrmpItem *)(sTrmpItems + itemOffset), (int *)position,
                                             normal, itemType, motionType);
                return sTrmpItems + itemOffset;
            }
            itemIndex++;
            base += 0x1d0;
        } while (itemIndex < sTrmpItemCount);
    }
    return 0;
}

extern "C" void TrmpLine_PointDistance(TrmpLine *sl, void *referencePoint,
                                       const Vector3 *offsetVector) {
    int row;
    int negatedProjection = -VEC_DotProduct(offsetVector, &sl->mNormal);
    int *amplitude = sl->mFlexAmplitude;
    char *restVertex = (char *)sl->mRestVerts;
    int weightedAmplitude;
    int *gridWeight = sTrmpLineFlexWeights;
    for (row = 0; row < 4; row++) {
        int column;
        for (column = 0; column < 4; column++) {
            Vector3 delta;
            VEC_Subtract((Vector3 *)restVertex, (Vector3 *)referencePoint, &delta);
            /* The long long widening is load-bearing (measured 2026-11-06).
               tools/delaunder.py accepts its removal because it byte-checks only
               one function of this TU; the per-function gate reports NO-REPRO. */
            int halfLength = (long long)(sl->mHalfWidth);
            int distance = VEC_Mag(&delta);
            int distanceWeight = FX_Div(halfLength, halfLength + distance);
            int weightedProjection = FX_MUL(negatedProjection, *gridWeight);
            weightedAmplitude = FX_MUL(weightedProjection, distanceWeight);
            *amplitude = weightedAmplitude;
            restVertex += 0xc;
            gridWeight++;
            amplitude++;
        }
    }
}

extern "C" void TrmpLine_ComputeNormals(TrmpLine *self) {
    Vector3 columnEdge;
    Vector3 rowEdge;
    char *p;
    Vector3 *normal;
    int *packedNormal;
    int row;
    int column;

    p = (char *)self->mVerts;
    normal = (Vector3 *)self->mNormals;
    packedNormal = self->mPackedNormals;

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
                VEC_Subtract((Vector3 *)p, (Vector3 *)p + 1, &columnEdge);
            else if (column == 3)
                VEC_Subtract((Vector3 *)p - 1, (Vector3 *)p, &columnEdge);
            else
                VEC_Subtract((Vector3 *)p - 1, (Vector3 *)p + 1, &columnEdge);

            if (row == 0)
                VEC_Subtract((Vector3 *)p, (Vector3 *)p + 4, &rowEdge);
            else if (row == 3)
                VEC_Subtract((Vector3 *)p - 4, (Vector3 *)p, &rowEdge);
            else
                VEC_Subtract((Vector3 *)p - 4, (Vector3 *)p + 4, &rowEdge);

            VEC_CrossProduct(&columnEdge, &rowEdge, normal);
            VEC_Normalize(normal, normal);

            normal->x = FX_MUL(normal->x, 0xff8);
            normal->y = FX_MUL(normal->y, 0xff8);
            normal->z = FX_MUL(normal->z, 0xff8);

            *packedNormal = ((normal->x >> 3) & 0x3ff) | (((normal->y >> 3) & 0x3ff) << 10) |
                            (((normal->z >> 3) & 0x3ff) << 20);

            normal++;
            packedNormal++;
            p += 0xc;
        }
    }
}

/* Loads the object's placement and scale matrices, then draws its 4x4 vertex
 * patch (positions mVerts, packed normals mPackedNormals) as three triangle
 * strips per face, the back face first with negated normals. Each normal
 * pointer steps in its own statement after the REG_NORMAL write; stepping it
 * inside the write reorders the vertex loads. */
extern "C" void TrmpLine_Draw(TrmpLine *obj) {
    int i;
    Matrix4x3 m2;
    Matrix4x3 m1;
    Matrix4x3_FromTranslation(&data_020a0e68, obj->mCenter.x, obj->mCenter.y, obj->mCenter.z);
    MTX_Concat43(&data_020a0e68, &data_0209b3ec, &m1);
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, obj->mScale.x, obj->mScale.y, obj->mScale.z);
    MTX_Concat43(&data_020a0e68, &data_0209b3ec, &m2);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(m1.m);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_LoadMtx43(m2.m);
    G3_Scale(0x100000, 0x100000, 0x100000);
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();
    G3_TexImageParam(GX_TEXFMT_PLTT16, GX_TEXGEN_NORMAL, GX_TEXSIZE_S32, GX_TEXSIZE_T64,
                     GX_TEXREPEAT_ST, GX_TEXFLIP_S, GX_TEXPLTTCOLOR0_USE, (u32)sTrmpLineTexAddr);
    G3_TexPlttBase((u32)sTrmpLinePlttAddr, GX_TEXFMT_PLTT16);
    {
        int polygonId = obj->mSlotIndex;
        unsigned short *diffuse = sTrmpLineDiffuse;
        /* Load-bearing launder: a plain mAlpha read reorders the
           POLYGON_ATTR loads (28 words differ). */
        volatile unsigned char *alpha = &obj->mAlpha;
        int polygonAlpha = *alpha;
        REG_POLYGON_ATTR = (((polygonId + 1) << 24) | 0x82) | (polygonAlpha << 16);
        {
            unsigned bounce = obj->mBounceCount;
            REG_DIF_AMB = diffuse[bounce] | (sTrmpLineAmbient[bounce] << 16);
        }
    }
    REG_SPE_EMI = 0x8000;

    /* back face: row i then row i+1, normals negated */
    for (i = 0; i < 3; i++) {
        Vec3 *v0 = &obj->mVerts[i * 4];
        Vec3 *v1 = &obj->mVerts[(i + 1) * 4];
        int *n0 = &obj->mPackedNormals[i * 4];
        int *n1 = &obj->mPackedNormals[(i + 1) * 4];
        int k;
        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
        for (k = 0; k < 4; k++) {
            REG_TEXCOORD = sTrmpLineTexCoords[i * 4 + k];
            REG_NORMAL = -*n0 & 0x3fffffff;
            n0++;
            G3_Vtx(v0->x >> 8, v0->y >> 8, v0->z >> 8);
            v0++;
            REG_TEXCOORD = sTrmpLineTexCoords[(i + 1) * 4 + k];
            REG_NORMAL = -*n1 & 0x3fffffff;
            n1++;
            G3_Vtx(v1->x >> 8, v1->y >> 8, v1->z >> 8);
            v1++;
        }
        G3_End();
    }

    /* front face: row i+1 then row i */
    for (i = 0; i < 3; i++) {
        Vec3 *v0 = &obj->mVerts[i * 4];
        Vec3 *v1 = &obj->mVerts[(i + 1) * 4];
        int *n0 = &obj->mPackedNormals[i * 4];
        int *n1 = &obj->mPackedNormals[(i + 1) * 4];
        int k;
        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
        for (k = 0; k < 4; k++) {
            REG_TEXCOORD = sTrmpLineTexCoords[(i + 1) * 4 + k];
            REG_NORMAL = *n1;
            n1++;
            G3_Vtx(v1->x >> 8, v1->y >> 8, v1->z >> 8);
            v1++;
            REG_TEXCOORD = sTrmpLineTexCoords[i * 4 + k];
            REG_NORMAL = *n0;
            n0++;
            G3_Vtx(v0->x >> 8, v0->y >> 8, v0->z >> 8);
            v0++;
        }
        G3_End();
    }
}

struct C;
extern "C" void TrmpLine_Update(void *object) {
    C *line = (C *)object;
    (line->*(line->pmf))();
}

extern "C" void TrmpLine_StateFade(TrmpLine *c) {
    if (ApproachLinear2(*(int *)&c->mAlpha, 0, 1) != 0) {
        c->mStatus = 0;
        return;
    }
    c->mScale.x += 0x80;
    c->mScale.y += 0x80;
    c->mScale.z += 0x80;
    TrmpLine_ComputeNormals(c);
}

extern "C" void TrmpLine_SetStateFade(TrmpLine *p) {
    *(StateRecord *)p->mState = pmf_TrmpLine_StateFade;
}

extern "C" void TrmpLine_StateSettle(TrmpLine *c) {
    int normalX, normalY;
    int negativeSine;
    int deflectionX, deflectionY;
    u16 phase;
    int row, column;
    int targetAmplitude, decayStep;
    int *restPosition, *position, *amplitude;

    if ((data_020a0db0 & 1) != 0) {
        if (ApproachLinear2(*(int *)&c->mAlpha, 0, 1) != 0) {
            c->mStatus = 0;
        }
    }

    c->mWobblePhase += c->mWobbleSpeed;
    phase = c->mWobblePhase;

    normalX = c->mNormal.x;
    normalY = c->mNormal.y;

    negativeSine = -FX_SinIdx(phase >> 4);

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    restPosition = (int *)c->mRestVerts;
    position = (int *)c->mVerts;
    amplitude = c->mFlexAmplitude;

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

extern "C" void TrmpLine_SetStateSettle(TrmpLine *p) {
    *(StateRecord *)p->mState = pmf_TrmpLine_StateSettle;
}

extern "C" void TrmpLine_StateFlex(TrmpLine *o) {
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

    TrmpLine_PushJumper(o);

    o->mWobblePhase += o->mWobbleSpeed;

    normalX = o->mNormal.x;
    normalY = o->mNormal.y;

    angleIndex = o->mWobblePhase >> 4;
    sine = FX_SinIdx(angleIndex);
    negativeSine = -(int)sine;

    restPosition = (Vec3 *)o->mRestVerts;
    position = (Vec3 *)o->mVerts;

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    amplitude = o->mFlexAmplitude;

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

    if (o->mSlotIndex == 3)
        return;

    if (ApproachLinear2(o->mLifeTimer, 0, 1) == 0)
        return;

    TrmpLine_SetStateFade(o);
}

extern "C" void TrmpLine_SetStateFlex(TrmpLine *p) {
    *(StateRecord *)p->mState = pmf_TrmpLine_StateFlex;
}

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

// Consumed prefix only; no complete jumper-object size is implied.

extern "C" int TrmpLine_PushJumper(TrmpLine *c) {
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
        if (((TrampolineContactView *)sTrmpJumpers[jumperIndex])->IsActive() == 0)
            continue;
        jumpers = (TrampolineContactView **)sTrmpJumpers;

        currentPositionPtr = jumpers[jumperIndex]->GetPos();
        currentPos.x = currentPositionPtr->x;
        currentPos.y = currentPositionPtr->y;
        currentPos.z = currentPositionPtr->z;

        previousPositionPtr = jumpers[jumperIndex]->GetPrevPos();
        previousPos.x = previousPositionPtr->x;
        previousPos.y = previousPositionPtr->y;
        previousPos.z = previousPositionPtr->z;

        Vec3_Sub(&movementDelta, &currentPos, &previousPos);
        VEC_Subtract(&currentPos, &c->mCenter, &currentPos);
        VEC_Subtract(&previousPos, &c->mCenter, &previousPos);

        // Test the previous-to-current segment against the c in its own frame.
        currentAlong = VEC_DotProduct(&c->mAxis, &currentPos);
        currentAcross = VEC_DotProduct(&c->mNormal, &currentPos);
        previousAlong = VEC_DotProduct(&c->mAxis, &previousPos);
        previousAcross = VEC_DotProduct(&c->mNormal, &previousPos);
        VEC_DotProduct(&c->mNormal, &movementDelta);
        VEC_DotProduct(&c->mAxis, &movementDelta);

        currentProjection.y = currentAcross;
        previousProjection.x = previousAlong;
        currentProjection.x = currentAlong;
        previousProjection.y = previousAcross;

        lineStart.x = -(c->mHalfWidth);
        lineStart.y = 0;
        lineEnd.x = c->mHalfWidth;
        lineEnd.y = 0;

        Vec2_Sub((int *)&edgeDirection, (int *)&lineStart, (int *)&lineEnd);
        Vec2_Sub((int *)&firstOffset, (int *)&lineStart, (int *)&previousProjection);
        Vec2_Sub((int *)&secondOffset, (int *)&lineStart, (int *)&currentProjection);

        crossedLine = 0;
        previousSide = func_0203d524(&edgeDirection, &firstOffset);
        currentSide = func_0203d524(&edgeDirection, &secondOffset);
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

            startSide = func_0203d524(&edgeDirection, &firstOffset);
            endSide = func_0203d524(&edgeDirection, &secondOffset);
            startSide = CLAMP(startSide, -1, 1);
            endSide = CLAMP(endSide, -1, 1);
            if (startSide * endSide <= 0)
                crossedLine = 1;
        }

        if (crossedLine != 0) {
            int offsetTimesHalfLength, edgeRatio;
            c->mContactPoint.x = currentPos.x;
            c->mContactPoint.y = currentPos.y;
            c->mContactPoint.z = currentPos.z;
            c->mContactMotion.x = movementDelta.x;
            c->mContactMotion.y = movementDelta.y;
            c->mContactMotion.z = movementDelta.z;

            edgeRatio = FX_Div((currentAlong < 0) ? -currentAlong : currentAlong, c->mHalfWidth);
            {
                int halfLength = c->mHalfWidth;
                int absoluteAlong = (currentAlong < 0) ? -currentAlong : currentAlong;
                offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
            }

            if (offsetTimesHalfLength < 0x400000) {
                jumpers[jumperIndex]->contactState = 2;
                D3D_PlaySePannedLoop(0x1b1, c->mCenter.x,
                                     (sTrmpLinePushWeakPitchEnd * edgeRatio +
                                      sTrmpLinePushWeakPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            } else {
                jumpers[jumperIndex]->contactState = 1;
                D3D_PlaySePannedLoop(0x1ae, c->mCenter.x,
                                     (sTrmpLinePushStrongPitchEnd * edgeRatio +
                                      sTrmpLinePushStrongPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            }

            {
                Vector3 *lineNormal = &jumpers[jumperIndex]->lineNormal;
                lineNormal->x = c->mNormal.x;
                lineNormal->y = c->mNormal.y;
                lineNormal->z = c->mNormal.z;
                jumpers[jumperIndex]->alongLine = currentAlong;
                jumpers[jumperIndex]->lineHalfLength = c->mHalfWidth;
                c->mWobblePhase = 0;
                TrmpLine_PointDistance(c, &c->mContactPoint, &c->mContactMotion);

                c->mBouncesLeft -= 1;
                if (c->mBouncesLeft == 0) {
                    c->mStatus = 3;
                    TrmpLine_SetStateSettle(c);
                    return 1;
                }
                c->mBounceCount += 1;
            }
        } else {
            Vec3_MulScalar(&halfLineForStart, &c->mAxis, c->mHalfWidth);
            Vec3_Add(&fromStart, &currentPos, &halfLineForStart);
            Vec3_MulScalar(&halfLineForEnd, &c->mAxis, c->mHalfWidth);
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

struct Spare4 {
    s32 v;
    ~Spare4() {}
};

extern "C" void TrmpLine_Build(TrmpLine *c, short *startPoint, short *endPoint) {
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

    c->mStart[0] = startPoint[0];
    c->mStart[1] = startPoint[1];
    c->mEnd[0] = endPoint[0];
    c->mEnd[1] = endPoint[1];

    startPoint[0] = (startPoint[0] - 0x80);
    startPoint[1] = (s16)(0 - startPoint[1]);
    endPoint[0] = (endPoint[0] - 0x80);
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
    c->mCenter.x = midpoint.x;
    c->mCenter.y = midpoint.y;
    c->mCenter.z = midpoint.z;

    Vec3_Sub(&lineSpan, &endPosition, &startPosition);
    c->mAxis.x = lineSpan.x;
    c->mAxis.y = lineSpan.y;
    c->mAxis.z = lineSpan.z;

    func_0203ce80(&directionCopy, &c->mAxis);

    zAxis.x = 0;
    zAxis.y = 0;
    zAxis.z = 0x1000;
    func_0203cf00(&lineNormal, &c->mAxis, &zAxis);
    c->mNormal.x = lineNormal.x;
    c->mNormal.y = lineNormal.y;
    c->mNormal.z = lineNormal.z;

    if (c->mNormal.y < 0) {
        c->mNormal.x = 0 - c->mNormal.x;
        c->mNormal.y = 0 - c->mNormal.y;
    }

    c->mContactPoint.x = 0;
    c->mContactPoint.y = 0;
    c->mContactPoint.z = 0;
    c->mContactMotion.x = 0 - c->mNormal.x;
    c->mContactMotion.y = 0 - c->mNormal.y;
    c->mContactMotion.z = 0 - c->mNormal.z;
    Vec3_MulScalarInPlace(&c->mContactMotion, 0x800);

    VEC_Subtract(&startPosition, &c->mCenter, &startPosition);
    VEC_Subtract(&endPosition, &c->mCenter, &endPosition);
    lineLength = Vec3_Dist(&startPosition, &endPosition);
    c->mHalfWidth = lineLength >> 1;
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
        cell = c->mVerts;
        mirror = c->mRestVerts;
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

                cell->z = accum - c->mHalfWidth;
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

    c->mWobblePhase = (s16)flag;
    c->mWobbleSpeed = 0x1400;
    c->mAlpha = 0x1F;
    TrmpLine_PointDistance(c, &c->mContactPoint, &c->mContactMotion);
    TrmpLine_ComputeNormals(c);

    c->mScale.x = 0x1000;
    c->mScale.y = 0x1000;
    c->mScale.z = 0x1000;

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

                VEC_Subtract(&relativePosition, &c->mCenter, &relativePosition);
                along = VEC_DotProduct(&c->mAxis, &relativePosition);
                across = VEC_DotProduct(&c->mNormal, &relativePosition);
                normalVelocity = VEC_DotProduct(&c->mNormal, &velocity);
                VEC_DotProduct(&c->mAxis, &velocity);

                if (normalVelocity < 0x100 && across <= selectedAcross && across > -0x24000) {
                    s32 halfLength = c->mHalfWidth;
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
        c->mContactPoint.x = selectedPosition.x;
        c->mContactPoint.y = selectedPosition.y;
        c->mContactPoint.z = selectedPosition.z;
        c->mContactMotion.x = selectedVelocity.x;
        c->mContactMotion.y = selectedVelocity.y;
        c->mContactMotion.z = selectedVelocity.z;

        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        edgeRatio = FX_Div(absoluteAlong, c->mHalfWidth);
        halfLength = c->mHalfWidth;
        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
        if (offsetTimesHalfLength < 0x400000) {
            ((TrampolineContactView *)bestObj)->contactState = 2;
            D3D_PlaySePannedLoop(0x1B1, c->mCenter.x,
                                 (sTrmpLineWeakPitchEnd * edgeRatio +
                                  sTrmpLineWeakPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        } else {
            ((TrampolineContactView *)bestObj)->contactState = 1;
            D3D_PlaySePannedLoop(0x1AE, c->mCenter.x,
                                 (sTrmpLineStrongPitchEnd * edgeRatio +
                                  sTrmpLineStrongPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        }
        ((TrampolineContactView *)bestObj)->lineNormal.x = c->mNormal.x;
        ((TrampolineContactView *)bestObj)->lineNormal.y = c->mNormal.y;
        ((TrampolineContactView *)bestObj)->lineNormal.z = c->mNormal.z;
        ((TrampolineContactView *)bestObj)->alongLine = selectedAlong;
        ((TrampolineContactView *)bestObj)->lineHalfLength = c->mHalfWidth;
        c->mStatus = 3;
        TrmpLine_PointDistance(c, &c->mContactPoint, &c->mContactMotion);
        TrmpLine_SetStateSettle(c);
        return;
    }

    c->mLifeTimer = 0x258;
    c->mStatus = 1;
    TrmpLine_SetStateFlex(c);
}

extern "C" void TrmpLine_DrawAll(void) {
    int light[3];
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&data_0209b3ec);
    light[0] = 0;
    light[1] = 0;
    light[2] = -0xff8;
    func_0203cd80(light, -0x2000);
    G3_LightVector(GX_LIGHTID_1, light[0], light[1], light[2]);
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
    int i;
    for (i = 0; i < 4; i++) {
        if (sTrmpLines[i].mStatus != 0) {
            TrmpLine_Draw(&sTrmpLines[i]);
        }
    }
}

extern "C" void TrmpLine_UpdateAll(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (sTrmpLines[i].mStatus != 0)
            TrmpLine_Update(&sTrmpLines[i]);
    }
}

extern "C" void TrmpLine_ClearAll(u32) {
    int i;
    for (i = 0; i < 3; i++) {
        sTrmpLines[i].mStatus = 0;
    }
}

extern "C" void TrmpLine_LoadTextures(void) {
    sTrmpLineTexAddr = (void *)dExtSimpleModel_c::LoadTextureToVram(sTrmpLineTex, 0x400);
    sTrmpLinePlttAddr = (void *)func_02045a50(sTrmpLinePltt, 0x20);
}

/* The start line always takes the fourth slot, which TrmpLine_Create never hands out. */
extern "C" void TrmpLine_CreateStartLineT2(void) {
    short start[2];
    short end[2];
    start[0] = 0x10;
    start[1] = 0xb0;
    end[0] = 0xf0;
    end[1] = 0xb0;
    TrmpLine_Build(&sTrmpLines[3], start, end);
    sTrmpLines[3].mBouncesLeft = 3;
    sTrmpLines[3].mBounceCount = 0;
    sTrmpLines[3].mSlotIndex = 3;
}

/* The start line always takes the fourth slot, which TrmpLine_Create never hands out. */
extern "C" void TrmpLine_CreateStartLineT1(void) {
    short start[2];
    short end[2];
    start[0] = 0x18;
    start[1] = 0xb0;
    end[0] = 0xe8;
    end[1] = 0xb0;
    TrmpLine_Build(&sTrmpLines[3], start, end);
    sTrmpLines[3].mBouncesLeft = 3;
    sTrmpLines[3].mBounceCount = 0;
    sTrmpLines[3].mSlotIndex = 3;
}

/* Preserve the earlier local-static uniquifiers in this ordered TU. */
#include "Vector2Math.h"

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
    char *newLine;

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
        int ay = startPoint[1];
        int by = endPoint[1];
        int ax = startPoint[0];
        int bx = endPoint[0];
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

    if (Vec2_Len((const Vector2 *)delta) < 0x30000) {
        weightedYProduct = (long long)direction[1] * 0xc00 + 0x800;
        weightedY = (int)(weightedYProduct >> 12);
        scaledDirection[0] = direction[0];
        scaledDirection[1] = direction[1];
        scaledDirection[1] = weightedY;
        scaledDirectionLength = Vec2_Len((const Vector2 *)scaledDirection);
        func_0203d630(direction, FX_Div(0x18000, scaledDirectionLength));

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
            newLine = (char *)sTrmpLines + slotOffset;
            TrmpLine_Build((TrmpLine *)newLine, startCopy, endCopy);
            /* Load-bearing: the ROM adds slotOffset as an integer to each field's
               own address constant. sTrmpLines[slotIndex].field (999 words) and
               char * arithmetic (7 words) both differ. */
            *(s16 *)((int)&sTrmpLines[0].mBouncesLeft + slotOffset) = 1;
            *(u16 *)((int)&sTrmpLines[0].mBounceCount + slotOffset) = 0;
            *(s16 *)((int)&sTrmpLines[0].mSlotIndex + slotOffset) = (s16)slotIndex;
            return (int)newLine;
        }
        slotIndex++;
        candidateSlot += 0x32c;
    } while (slotIndex < 3);

    return 0;
}

/* The four line slots. mwcc builds them in this file's static initializer
   with __cxa_vec_ctor and registers their cleanup thunk, __arraydtor. */
TrmpLine sTrmpLines[4];

/* The constructor and destructor are inline: mwcc emits a used inline body
   at the end of the file, after the static initializer and __arraydtor, which
   is where the cartridge has D1 and then C1. Out-of-line bodies would come
   ahead of __arraydtor and bring a D2/C2 pair with no ROM home. */
inline TrmpLine::~TrmpLine() {}

inline TrmpLine::TrmpLine() {
    mStatus = 0;
}

/* The ring model handles. The initializer builds them after sTrmpLines,
   in this order. */
TrmpItemRingModelFile sTrmpItemRingAModelFile(0x21a);
TrmpItemRingModelFile sTrmpItemRingBModelFile(0x21b);
TrmpItemRingModelFile sTrmpItemRingCModelFile(0x21c);
