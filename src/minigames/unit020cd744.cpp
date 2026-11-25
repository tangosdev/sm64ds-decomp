//cpp
/* The ov006 stylus-drawn trampolines, 0x020cd744..0x020d1018.
 *
 * A line slot is a 4x4 patch: positions at +0x5c, normals at +0x1dc, packed
 * normals at +0x2dc. Four 0x32c-byte slots live in sTrmpLines. Trampoline2's
 * floating rings are 0x1d0-byte TrmpItem records in the scene's array at
 * sTrmpItems. No RTTI names a type in this run. The per-record helpers are
 * methods of TrmpItem and TrmpLine (include/TrmpItem.h, include/TrmpLine.h);
 * the pool walkers are static members.
 *
 * `#pragma defer_codegen off` keeps the bodies in ROM-ascending order. The file
 * owns its static initializer, its .ctor word and its .bss: the line slots, the
 * three ring model handles, the counters, sTrmp2MarioPtrs (read only by
 * d_mg_trmpln2_mario) and the function-local unit vectors.
 *
 * Leftovers:
 * - TrmpItem::Init's fifth parameter is short: retail's callee loads it with
 *   ldrsh, and TrmpItem::Spawn takes the scene tables' short motion type, so
 *   the call passes the value without a narrowing step.
 * - Vector3 locals emit an empty destructor, licensed as a deadstrip duplicate
 *   of arm9:0x020072c0. TrmpLine::Build's never-written stack slot is held by
 *   Spare4, whose empty destructor is deadstripped as compiler-only output.
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
#include "Vector2Math.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/Quaternion.h"
#include "math/MtxStack.h"
#include "MgCamera.h"

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

/* A line patch vertex: the same three-fx32 spelling as TrmpLine::Vtx. */
typedef TrmpLine::Vtx Vec3;

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

/* This unit's own globals (its .bss claim). sTrmp2MarioPtrs is read only by
   d_mg_trmpln2_mario. */
void *sTrmpLinePlttAddr;
void *sTrmpLineTexAddr;
int sTrmpItemsHit;
int sTrmpItemsActive;
TrmpItem *sTrmpItems;
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
char sTrmpLinePltt[32] = {
    0x1f, 0x00, 0x3f, 0x00, 0x5f, 0x04, 0x9f, 0x0c, 0xff, 0x14, 0x1f, 0x1d, 0x5f, 0x25, 0x9f, 0x31,
    0x1f, 0x42, 0x5f, 0x4a, 0xbf, 0x52, 0xff, 0x5e, 0x7f, 0x6f, 0x9f, 0x73, 0xdf, 0x7b, 0xff, 0x7f,
};
char sTrmpLineTex[0x400] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbd, 0x78, 0x34, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0x69, 0x12, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x27, 0x01, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x28, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x86, 0xca, 0xfe,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0xda, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0x71, 0xfb, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x9f, 0x01, 0x00, 0x00, 0x00, 0x50, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x3b, 0x01, 0x00, 0x00, 0x00, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xdf, 0x05, 0x00, 0x00, 0x00, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x10, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x2b, 0x01, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xbf, 0x01, 0x00, 0x00, 0x50, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xa1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x2c, 0x01, 0x00, 0x00, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa7, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x09, 0x00, 0x00, 0x10, 0xfb, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x41, 0xfb, 0xff, 0xff,
    0xff, 0xff, 0x06, 0x00, 0x00, 0x60, 0xff, 0xff, 0xff, 0xff, 0xff, 0x6f, 0x00, 0xa3, 0xff, 0xff,
    0xff, 0xdf, 0x12, 0x00, 0x00, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x19, 0x00, 0x30, 0xfb, 0xff,
    0xff, 0xbf, 0x00, 0x00, 0x00, 0xd3, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x00, 0x10, 0xb5, 0xff,
    0xff, 0x8f, 0x00, 0x00, 0x00, 0xf6, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x02, 0x00, 0x00, 0x41, 0xe9,
    0xff, 0x7f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x3b, 0x00, 0x00, 0x10, 0x00, 0x63,
    0xff, 0x4f, 0x00, 0x00, 0x00, 0xfa, 0xff, 0xff, 0xff, 0xff, 0x06, 0x00, 0x62, 0x04, 0x00, 0x00,
    0xff, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xbf, 0x13, 0x00, 0xb4, 0x1a, 0x11, 0x00,
    0xff, 0x2f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x6f, 0x02, 0x01, 0xf7, 0x9f, 0x03, 0x00,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xef, 0x18, 0x00, 0x30, 0xfa, 0xff, 0x28, 0x00,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xcf, 0x14, 0x00, 0x50, 0xfc, 0xff, 0x7f, 0x00,
    0xff, 0x2f, 0x01, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x8f, 0x01, 0x00, 0x70, 0xff, 0xff, 0xff, 0x36,
    0xff, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x3b, 0x00, 0x10, 0x90, 0xff, 0xff, 0xff, 0x1d,
    0xff, 0x4f, 0x00, 0x00, 0x00, 0xfa, 0xff, 0xff, 0x17, 0x00, 0x00, 0xb4, 0xff, 0xff, 0xff, 0xcf,
    0xff, 0x7f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xbf, 0x13, 0x00, 0x00, 0xd6, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0x8f, 0x00, 0x00, 0x00, 0xf6, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xe8, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xbf, 0x00, 0x00, 0x10, 0xd3, 0xff, 0xaf, 0x00, 0x01, 0x30, 0xfb, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xdf, 0x02, 0x00, 0x00, 0xa0, 0xff, 0xef, 0x3a, 0x00, 0x50, 0xfe, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x06, 0x00, 0x00, 0x60, 0xff, 0xff, 0xbe, 0x06, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x09, 0x00, 0x00, 0x10, 0xfb, 0xff, 0xff, 0x9d, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0xf7, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xa1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xbf, 0x01, 0x00, 0x00, 0x50, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x2b, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x10, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xdf, 0x05, 0x00, 0x00, 0x00, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x3b, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x9f, 0x01, 0x00, 0x00, 0x00, 0x50, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0x71, 0xfb, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0xda, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x86, 0xca, 0xfe,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0x69, 0x02, 0x00, 0x00, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbd, 0x78, 0x34, 0x02,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
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
extern Matrix4x3 gViewMtx;       /* the view matrix, Graphics3D.cpp */
extern Quaternion data_02092768; /* identity */
extern int gGlobalTimer;

void TrmpItem::GrowScale() {
    ApproachLinear(mScale.x, 0x1200, 0x80);
    ApproachLinear(mScale.z, 0x1200, 0x80);
    ApproachLinear(mScale.y, 0x20000, 0x400);
    if (ApproachLinear2(mAlpha, 0, 1) == 0)
        return;
    mActive = 0;
    ApproachLinear(sTrmpItemsActive, 0, 1);
}

void TrmpItem::ProjectToScreen(unsigned short scoreValue) {
    int screenX, screenY;
    if (mPos.y > 0) {
        Camera_ProjectToScreen(sMgSubCamera, &mPos, &screenX, &screenY);
        screenY = screenY - (Mg_GetTopScreenYOffset() + 0xc0);
    } else {
        Camera_ProjectToScreen(sMgMainCamera, &mPos, &screenX, &screenY);
    }
    Jump2Popup_Spawn(screenX << 0xc, screenY << 0xc, (short)scoreValue);
    mState = &TrmpItem::GrowScale;
}

void TrmpItem::MotionSpinHover() {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28(&mForward, 0x100);
    func_0203ce80(&normalCopy, &mForward);
    Vec3_Cross(&tangent, &mForward, &sUnitZ);
    mSide.x = tangent.x;
    mSide.y = tangent.y;
    mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &mSide);
    Quaternion_FromVector3(mRotation, &sUnitY, &mForward);
    Quaternion_Normalize(mRotation);
    MotionHover();
}

void TrmpItem::SetMotionSpinHover() {
    mPhase = 0;
    mState = &TrmpItem::MotionSpinHover;
}

void TrmpItem::MotionSpinDrift() {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy, tangent, tangentCopy;

    func_0203cc28(&mForward, 0x100);
    func_0203ce80(&normalCopy, &mForward);
    Vec3_Cross(&tangent, &mForward, &sUnitZ);
    mSide.x = tangent.x;
    mSide.y = tangent.y;
    mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &mSide);
    Quaternion_FromVector3(mRotation, &sUnitY, &mForward);
    Quaternion_Normalize(mRotation);
}

void TrmpItem::SetMotionSpinDrift() {
    mState = &TrmpItem::MotionSpinDrift;
}

void TrmpItem::MotionSpinSway() {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy;
    struct Vector3 tangent;
    struct Vector3 tangentCopy;
    func_0203cc28(&mForward, 0x100);
    func_0203ce80(&normalCopy, &mForward);
    Vec3_Cross(&tangent, &mForward, &sUnitZ);
    mSide.x = tangent.x;
    mSide.y = tangent.y;
    mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &mSide);
    Quaternion_FromVector3(mRotation, &sUnitY, &mForward);
    Quaternion_Normalize(mRotation);
    MotionSway();
}

void TrmpItem::SetMotionSpinSway() {
    mPhase = 0x2000;
    mState = &TrmpItem::MotionSpinSway;
}

void TrmpItem::MotionHover() {
    mPhase += 0x100;
    if (mPhase & 0x8000) {
        mVel.y = 0xc00;
    } else {
        mVel.y = -0xc00;
    }
}

void TrmpItem::SetMotionHover() {
    mPhase = 0;
    mState = &TrmpItem::MotionHover;
}

void TrmpItem::MotionSway() {
    int distanceFromCenterY;
    int xSpeed = 0x400;

    mPhase += 0x40;

    distanceFromCenterY = mPos.y - 0x74000;
    if (distanceFromCenterY < 0)
        distanceFromCenterY = -distanceFromCenterY;
    xSpeed += FX_Div(distanceFromCenterY, 0x4c000) >> 3;

    if (mPhase & 0x4000)
        mVel.x = xSpeed;
    else
        mVel.x = -xSpeed;
}

void TrmpItem::SetMotionSway() {
    mPhase = 0x2000;
    mState = &TrmpItem::MotionSway;
}

void TrmpItem::MotionSpin() {
    /* The spin axis and the reference direction, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);
    static Vector3 sUnitZ(0, 0, 0x1000);

    struct Vector3 normalCopy, tangent, tangentCopy;

    func_0203cc28(&mForward, 0x100);
    func_0203ce80(&normalCopy, &mForward);
    Vec3_Cross(&tangent, &mForward, &sUnitZ);
    mSide.x = tangent.x;
    mSide.y = tangent.y;
    mSide.z = tangent.z;
    func_0203ce80(&tangentCopy, &mSide);
    Quaternion_FromVector3(mRotation, &sUnitY, &mForward);
    Quaternion_Normalize(mRotation);
}

void TrmpItem::SetMotionSpin() {
    mVel.y = 0;
    mState = &TrmpItem::MotionSpin;
}

void TrmpItem::MotionBob() {
    int y = mPos.y;
    if (y > 0x80000) {
        mVel.y = -0x1000;
        return;
    }
    if (y < -0x60000)
        mVel.y = 0x1000;
}

void TrmpItem::SetMotionBob() {
    mVel.y = 0x1000;
    mState = &TrmpItem::MotionBob;
}

void TrmpItem::MotionPatrol() {
    int itemType = mSizeIdx;
    if (mPos.x > sTrmpItemPatrolLimits[itemType]) {
        mVel.x = -sTrmpItemPatrolSpeed[itemType];
        return;
    }
    if (mPos.x < -sTrmpItemPatrolLimits[itemType]) {
        mVel.x = sTrmpItemPatrolSpeed[itemType];
    }
}

void TrmpItem::SetMotionPatrol() {
    int itemType = mSizeIdx;
    mVel.x = sTrmpItemPatrolSpeed[itemType];
    mState = &TrmpItem::MotionPatrol;
}

void TrmpItem::MotionStill() {}

void TrmpItem::SetMotionStill() {
    mState = &TrmpItem::MotionStill;
}

void TrmpItem::StateAppear() {
    int yScaleReady = ApproachLinear(mScale.y, 0x1000, 0xc0);
    int xScaleReady = ApproachLinear(mScale.x, 0x1000, 0x180);
    int zScaleReady = ApproachLinear(mScale.z, 0x1000, 0x180);
    int responseScaleReady = ApproachLinear(mAppearScale, 0x800, 0x60);
    if (yScaleReady == 0)
        return;
    if (xScaleReady == 0)
        return;
    if ((zScaleReady & responseScaleReady) == 0)
        return;
    switch (mMotionKind) {
    case 1:
        SetMotionPatrol();
        break;
    case 2:
        SetMotionBob();
        break;
    case 3:
        SetMotionSpin();
        break;
    case 0:
        SetMotionStill();
        break;
    case 4:
        SetMotionSway();
        break;
    case 6:
        SetMotionHover();
        break;
    case 8:
        SetMotionSpinDrift();
        break;
    case 5:
        SetMotionSpinSway();
        break;
    case 7:
        SetMotionSpinHover();
        break;
    }
}

void TrmpItem::StartAppear() {
    ApproachLinear(sTrmpItemsActive, sTrmpItemCount, 1);
    mAlpha = 0x1f;
    mVel.x = 0;
    mVel.y = 0;
    mVel.z = 0;
    mState = &TrmpItem::StateAppear;
}

void TrmpItem::ReflectOffLine(TrampolineObjectCallbacks *jumper) {
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

    if (mState == &TrmpItem::GrowScale)
        return;

    position = jumper->GetPos();
    velocity = jumper->GetVelocity();

    pushSum = data_020a0ebc;
    Vec3_Sub(&relativePosition, position, &mPos);
    positiveEndOffset = relativePosition;
    negativeEndOffset = relativePosition;
    itemToJumper = relativePosition;

    Vec3_MulScalar(&halfSpan, &mSide, sTrmpItemHitWidths[mSizeIdx]);
    VEC_Add(&negativeEndOffset, &halfSpan, &negativeEndOffset);
    VEC_Subtract(&positiveEndOffset, &halfSpan, &positiveEndOffset);

    negativeEndOffset.y += 0xE000;
    positiveEndOffset.y += 0xE000;
    negativeEndOffset.y >>= 1;
    positiveEndOffset.y >>= 1;

    negativeEndDistance = VEC_Mag(&negativeEndOffset);
    positiveEndDistance = VEC_Mag(&positiveEndOffset);
    along = VEC_DotProduct(&itemToJumper, &mSide);
    across = VEC_DotProduct(&itemToJumper, &mForward);

    if (negativeEndDistance < 0xE000 && NormalizeVec3IfNonZero(&negativeEndOffset) != 0) {
        Vec3_MulScalarInPlace(&negativeEndOffset, 0xE000 - negativeEndDistance);
        negativeEndOffset.y <<= 1;
        Vec3_MulScalar(&negativeEndCorrection, &negativeEndOffset, mAppearScale);
        VEC_Add(position, &negativeEndCorrection, position);
        VEC_Add(&pushSum, &negativeEndOffset, &pushSum);
        ((TrampolineJumperCounts *)jumper)->mItemPushCount += 1;
    } else if (positiveEndDistance < 0xE000 && NormalizeVec3IfNonZero(&positiveEndOffset) != 0) {
        Vec3_MulScalarInPlace(&positiveEndOffset, 0xE000 - positiveEndDistance);
        positiveEndOffset.y <<= 1;
        Vec3_MulScalar(&positiveEndCorrection, &positiveEndOffset, mAppearScale);
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
    if (along >= sTrmpItemHitWidths[mSizeIdx])
        return;

    Vec3_MulScalar(&normalVelocityChange, &mForward, 0x70);
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

/* The ROM re-reads sTrmpItems and recomputes the element address at each use,
   so UpdateWithJumper indexes sTrmpItems[itemIndex] afresh each time; caching
   the element in a local reallocates the whole loop. */

void TrmpItem::UpdateWithJumper(TrampolineObjectCallbacks *jumperObject, int arg1) {
    TrampolineJumperCounts *jumper = (TrampolineJumperCounts *)jumperObject;
    int itemIndex;
    int doubling;
    int scoreIncrement;
    int hitCount;
    int scoreDoublings;

    if (sTrmpItemHitsOff != 0)
        return;
    jumper->mItemPushCount = 0;

    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        if (sTrmpItems[itemIndex].mActive == 0)
            continue;
        sTrmpItems[itemIndex].ReflectOffLine(jumper);
        if (sTrmpItems[itemIndex].HitTest2D(jumper, arg1) == 0)
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
            D3D_PlaySeAtX(0x131, sTrmpItems[itemIndex].mPos.x);
            break;
        case 1:
            D3D_PlaySeAtX(0x132, sTrmpItems[itemIndex].mPos.x);
            break;
        case 2:
            D3D_PlaySeAtX(0x133, sTrmpItems[itemIndex].mPos.x);
            break;
        case 3:
            D3D_PlaySeAtX(0x134, sTrmpItems[itemIndex].mPos.x);
            break;
        case 4:
        default:
            D3D_PlaySeAtX(0x135, sTrmpItems[itemIndex].mPos.x);
            break;
        }
        ApproachLinear(sTrmp2Score, 0xf423f, scoreIncrement);
        Trmp2Spinner_InitAll(jumper->mItemHitCount);
        sTrmpItems[itemIndex].ProjectToScreen((short)scoreIncrement);
        jumper->mItemHitCount++;
    }
}

// The caller supplies a third argument; this implementation does not read it.
int TrmpItem::HitTest2D(TrampolineObjectCallbacks *jumper, int) {
    Vector2 currentProjection, previousProjection, lineStart, lineEnd;
    Vector2 testDirection, firstOffset, secondOffset;
    Vector2 movementDirection, toEnd, toStart;
    int currentSide, previousSide, endSide, startSide;
    int result;

    if (mState == &TrmpItem::StateAppear || mState == &TrmpItem::GrowScale)
        return 0;

    result = 0;
    ProjectOnLine(jumper, &currentProjection, &previousProjection);

    {
        int *halfLengths = sTrmpItemHitWidths;
        lineStart.x = -halfLengths[mSizeIdx];
        lineStart.y = 0;
        lineEnd.x = halfLengths[mSizeIdx];
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
    if (currentSide * previousSide <= 0 && (currentSide != 0 || previousSide != 0)) {

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
    }
    return result;
}

void TrmpItem::ProjectOnLine(TrampolineObjectCallbacks *jumper, Vector2 *currentProjection,
                             Vector2 *previousProjection) {
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
    Vec3_Sub(&currentRelative, &currentPosition, &mPos);
    Vec3_Sub(&previousRelative, &previousPosition, &mPrevPos);
    // Only the current relative position receives this vertical offset.
    currentRelative.y += 0xc000;
    across = VEC_DotProduct(&mForward, &currentRelative);
    currentProjection->x = VEC_DotProduct(&mSide, &currentRelative);
    currentProjection->y = across;
    across = VEC_DotProduct(&mPrevForward, &previousRelative);
    previousProjection->x = VEC_DotProduct(&mPrevSide, &previousRelative);
    previousProjection->y = across;
}

void TrmpItem::UpdateMatrix() {
    Matrix4x3 rotation;
    Matrix4x3_FromQuaternion(reinterpret_cast<const Quaternion *>(mRotation), &rotation);
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x, mPos.y, mPos.z);
    MTX_Concat43(&rotation, &gScratchMtx, &gScratchMtx);
    mModel->mat4x3 = gScratchMtx;
    mTexAnim.Update(mModel->data);
    mModel->ApplyOpacity(mAlpha, 0);
    mModel->Render(&mScale);
}

void TrmpItem::Update() {
    (this->*mState)();
    VEC_Add(&mPos, &mVel, &mPos);
    mTexAnim.Advance();
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    mPrevForward.x = mForward.x;
    mPrevForward.y = mForward.y;
    mPrevForward.z = mForward.z;
    mPrevSide.x = mSide.x;
    mPrevSide.y = mSide.y;
    mPrevSide.z = mSide.z;
}

void TrmpItem::Init(const VecFx32 *pos, const Vector3 *normal, int itemType, short motion) {
    /* The axis the starting rotation is built about, built on first use. */
    static Vector3 sUnitY(0, 0x1000, 0);

    mPos.x = pos->x;
    mPos.y = pos->y;
    mPos.z = pos->z;
    mForward.x = normal->x;
    mForward.y = normal->y;
    mForward.z = normal->z;
    {
        Vector3 *normalComponents = &mForward;
        mSide.x = normalComponents->x;
        mSide.y = normalComponents->y;
        mSide.z = normalComponents->z;
    }
    func_0203cc28(&mSide, -0x4000);
    mPrevPos.x = mPos.x;
    mPrevPos.y = mPos.y;
    mPrevPos.z = mPos.z;
    {
        Vector3 *cachedPosition = &mPrevPos;
        mSpawnPos.x = cachedPosition->x;
        mSpawnPos.y = cachedPosition->y;
        mSpawnPos.z = cachedPosition->z;
    }
    mPrevForward.x = mForward.x;
    mPrevForward.y = mForward.y;
    mPrevForward.z = mForward.z;
    mPrevSide.x = mSide.x;
    mPrevSide.y = mSide.y;
    mPrevSide.z = mSide.z;
    mRotation[0] = data_02092768.x;
    mRotation[1] = data_02092768.y;
    mRotation[2] = data_02092768.z;
    mRotation[3] = data_02092768.w;
    Quaternion_FromVector3(mRotation, &sUnitY, &mForward);
    mSizeIdx = itemType;
    mMotionKind = motion;
    mActive = 1;
    switch (mSizeIdx) {
    case 0:
        mModel = &mRingA;
        break;
    case 1:
        mModel = &mRingB;
        break;
    case 2:
    default:
        mModel = &mRingC;
        break;
    }
    mScale.x = data_020a0ebc.x;
    mScale.y = data_020a0ebc.y;
    mScale.z = data_020a0ebc.z;
    mAppearScale = 0;
    mUnusedInit = 0x5a;
    StartAppear();
}

void TrmpItem::Hide() {
    mActive = 0;
}

void TrmpItem::Load() {
    mRingA.SetFile((BMD_File *)sTrmpItemRingAModelFile.filePtr, 1, -1);
    mRingB.SetFile((BMD_File *)sTrmpItemRingBModelFile.filePtr, 1, -1);
    mRingC.SetFile((BMD_File *)sTrmpItemRingCModelFile.filePtr, 1, -1);
    mRingA.SetPolygonID(1);
    mRingB.SetPolygonID(2);
    mRingC.SetPolygonID(3);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)sTrmpItemRingAModelFile.filePtr, sTrmpItemTextureAnim);
    mTexAnim.SetFile(sTrmpItemTextureAnim, 0, 0x1000, 0);
    Hide();
}

void TrmpItem::UpdateMatricesAll() {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        TrmpItem *p = &sTrmpItems[itemIndex];
        if (p->mActive != 0)
            p->UpdateMatrix();
    }
}

void TrmpItem::UpdateAll() {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++) {
        TrmpItem *p = &sTrmpItems[itemIndex];
        if (p->mActive != 0)
            p->Update();
    }
}

void TrmpItem::HideAll(u32) {
    int itemIndex;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++)
        sTrmpItems[itemIndex].Hide();
    sTrmpItemsActive = 0;
    sTrmpItemsHit = 0;
    sTrmp2Score = 0;
}

void TrmpItem::ReleaseFiles() {
    sTrmpItemRingAModelFile.Release();
    sTrmpItemRingBModelFile.Release();
    sTrmpItemRingCModelFile.Release();
}

void TrmpItem::LoadAll(char *items, int count) {
    int itemIndex;
    dExtSimpleModel_c::LoadFile(sTrmpItemRingAModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingBModelFile);
    dExtSimpleModel_c::LoadFile(sTrmpItemRingCModelFile);
    sTrmpItems = (TrmpItem *)items;
    sTrmpItemCount = count;
    for (itemIndex = 0; itemIndex < sTrmpItemCount; itemIndex++)
        sTrmpItems[itemIndex].Load();
}

TrmpItem *TrmpItem::Spawn(const VecFx32 *pos, const Vector3 *normal, int itemType, short motion) {
    int itemIndex = 0;
    TrmpItem *base;
    if (sTrmpItemCount > 0) {
        base = sTrmpItems;
        do {
            if (base->mActive == 0) {
                sTrmpItems[itemIndex].Init(pos, normal, itemType, motion);
                return &sTrmpItems[itemIndex];
            }
            itemIndex++;
            base++;
        } while (itemIndex < sTrmpItemCount);
    }
    return 0;
}

void TrmpLine::PointDistance(Vector3 *referencePoint, const Vector3 *offsetVector) {
    int row;
    int negatedProjection = -VEC_DotProduct(offsetVector, &mNormal);
    int *amplitude = mFlexAmplitude;
    char *restVertex = (char *)mRestVerts;
    int weightedAmplitude;
    int *gridWeight = sTrmpLineFlexWeights;
    for (row = 0; row < 4; row++) {
        int column;
        for (column = 0; column < 4; column++) {
            Vector3 delta;
            VEC_Subtract((Vector3 *)restVertex, referencePoint, &delta);
            /* The long long widening is load-bearing (measured 2026-11-06).
               tools/delaunder.py accepts its removal because it byte-checks only
               one function of this TU; the per-function gate reports NO-REPRO. */
            int halfLength = (long long)(mHalfWidth);
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

void TrmpLine::ComputeNormals() {
    Vector3 columnEdge;
    Vector3 rowEdge;
    char *p;
    Vector3 *normal;
    int *packedNormal;
    int row;
    int column;

    p = (char *)mVerts;
    normal = (Vector3 *)mNormals;
    packedNormal = mPackedNormals;

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            int *components;

            components = (int *)&columnEdge;
            components[0] = 0;
            components[1] = 0;
            components[2] = 0;
            components = (int *)&rowEdge;
            components[0] = 0;
            components[1] = 0;
            components[2] = 0;

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
void TrmpLine::Draw() {
    int i;
    Matrix4x3 m2;
    Matrix4x3 m1;
    Matrix4x3_FromTranslation(&gScratchMtx, mCenter.x, mCenter.y, mCenter.z);
    MTX_Concat43(&gScratchMtx, &gViewMtx, &m1);
    Matrix4x3_ApplyInPlaceToScale(&gScratchMtx, mScale.x, mScale.y, mScale.z);
    MTX_Concat43(&gScratchMtx, &gViewMtx, &m2);
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
        int polygonId = mSlotIndex;
        unsigned short *diffuse = sTrmpLineDiffuse;
        /* Load-bearing launder: a plain mAlpha read reorders the
           POLYGON_ATTR loads (28 words differ). r63: a const pointer to
           mAlpha, an int or u8 local, and the attribute bits in SDK
           order all leave 30 words different. */
        /* Kept (#821 r79): an inline LineAlpha(this) returning int or unsigned char, held in
           polygonAlpha or written into the attribute, is 30 words off. */
        volatile unsigned char *alpha = &mAlpha;
        int polygonAlpha = *alpha;
        REG_POLYGON_ATTR = (((polygonId + 1) << 24) | 0x82) | (polygonAlpha << 16);
        {
            unsigned bounce = mBounceCount;
            REG_DIF_AMB = diffuse[bounce] | (sTrmpLineAmbient[bounce] << 16);
        }
    }
    REG_SPE_EMI = 0x8000;

    /* back face: row i then row i+1, normals negated */
    for (i = 0; i < 3; i++) {
        Vec3 *v0 = &mVerts[i * 4];
        Vec3 *v1 = &mVerts[(i + 1) * 4];
        int *n0 = &mPackedNormals[i * 4];
        int *n1 = &mPackedNormals[(i + 1) * 4];
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
        Vec3 *v0 = &mVerts[i * 4];
        Vec3 *v1 = &mVerts[(i + 1) * 4];
        int *n0 = &mPackedNormals[i * 4];
        int *n1 = &mPackedNormals[(i + 1) * 4];
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

void TrmpLine::Update() {
    (this->*mState)();
}

void TrmpLine::StateFade() {
    if (ApproachLinear2(mAlpha, 0, 1) != 0) {
        mStatus = 0;
        return;
    }
    mScale.x += 0x80;
    mScale.y += 0x80;
    mScale.z += 0x80;
    ComputeNormals();
}

void TrmpLine::SetStateFade() {
    mState = &TrmpLine::StateFade;
}

void TrmpLine::StateSettle() {
    int normalX, normalY;
    int negativeSine;
    int deflectionX, deflectionY;
    u16 phase;
    int row, column;
    int targetAmplitude, decayStep;
    int *restPosition, *position, *amplitude;

    if ((gGlobalTimer & 1) != 0) {
        if (ApproachLinear2(mAlpha, 0, 1) != 0) {
            mStatus = 0;
        }
    }

    mWobblePhase += mWobbleSpeed;
    phase = mWobblePhase;

    normalX = mNormal.x;
    normalY = mNormal.y;

    negativeSine = -FX_SinIdx(phase >> 4);

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    restPosition = (int *)mRestVerts;
    position = (int *)mVerts;
    amplitude = mFlexAmplitude;

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

    ComputeNormals();
}

void TrmpLine::SetStateSettle() {
    mState = &TrmpLine::StateSettle;
}

void TrmpLine::StateFlex() {
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

    PushJumper();

    mWobblePhase += mWobbleSpeed;

    normalX = mNormal.x;
    normalY = mNormal.y;

    angleIndex = mWobblePhase >> 4;
    sine = FX_SinIdx(angleIndex);
    negativeSine = -sine;

    restPosition = (Vec3 *)mRestVerts;
    position = (Vec3 *)mVerts;

    deflectionX = FX_MUL(normalX, negativeSine);
    deflectionY = FX_MUL(normalY, negativeSine);

    amplitude = mFlexAmplitude;

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

    ComputeNormals();

    if (mSlotIndex == 3)
        return;

    if (ApproachLinear2(mLifeTimer, 0, 1) == 0)
        return;

    SetStateFade();
}

void TrmpLine::SetStateFlex() {
    mState = &TrmpLine::StateFlex;
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
 * hoist in the compiler's own order; each jumper is read straight out of
 * sTrmpJumpers (no named pool local), so the pool address is a compiler
 * temporary and ranks below currentAlong, as the VEC_DotProduct results
 * returned through math/CalcLib.h's inlines do; the +0x320/+0x324 halfword
 * accesses go through line directly, which yields the ROM's shared
 * line+0x300 base and the copied read.
 */
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))

// Consumed prefix only; no complete jumper-object size is implied.

int TrmpLine::PushJumper() {
    int jumperIndex;
    int currentAlong;

    jumperIndex = 0;
    do {
        int currentAcross, previousAlong, previousAcross;
        Vector3 currentPos, previousPos, movementDelta;
        Vector2 previousProjection, currentProjection;
        Vector2 lineStart, lineEnd;
        Vector2 edgeDirection, firstOffset, secondOffset;
        int crossedLine, previousSide, currentSide;
        Vector3 fromStart, fromEnd, halfLineForStart, halfLineForEnd;
        Vector3 *currentPositionPtr;
        Vector3 *previousPositionPtr;

        if (sTrmpJumpers[jumperIndex] == 0)
            continue;
        if (((TrampolineContactView *)sTrmpJumpers[jumperIndex])->IsActive() == 0)
            continue;

        currentPositionPtr = ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->GetPos();
        currentPos.x = currentPositionPtr->x;
        currentPos.y = currentPositionPtr->y;
        currentPos.z = currentPositionPtr->z;

        previousPositionPtr = ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->GetPrevPos();
        previousPos.x = previousPositionPtr->x;
        previousPos.y = previousPositionPtr->y;
        previousPos.z = previousPositionPtr->z;

        Vec3_Sub(&movementDelta, &currentPos, &previousPos);
        VEC_Subtract(&currentPos, &mCenter, &currentPos);
        VEC_Subtract(&previousPos, &mCenter, &previousPos);

        // Test the previous-to-current segment against the c in its own frame.
        currentAlong = VEC_DotProduct(&mAxis, &currentPos);
        currentAcross = VEC_DotProduct(&mNormal, &currentPos);
        previousAlong = VEC_DotProduct(&mAxis, &previousPos);
        previousAcross = VEC_DotProduct(&mNormal, &previousPos);
        VEC_DotProduct(&mNormal, &movementDelta);
        VEC_DotProduct(&mAxis, &movementDelta);

        currentProjection.y = currentAcross;
        previousProjection.x = previousAlong;
        currentProjection.x = currentAlong;
        previousProjection.y = previousAcross;

        lineStart.x = -(mHalfWidth);
        lineStart.y = 0;
        lineEnd.x = mHalfWidth;
        lineEnd.y = 0;

        Vec2_Sub(&edgeDirection, &lineStart, &lineEnd);
        Vec2_Sub(&firstOffset, &lineStart, &previousProjection);
        Vec2_Sub(&secondOffset, &lineStart, &currentProjection);

        crossedLine = 0;
        previousSide = func_0203d524(&edgeDirection, &firstOffset);
        currentSide = func_0203d524(&edgeDirection, &secondOffset);
        previousSide = CLAMP(previousSide, -1, 1);
        currentSide = CLAMP(currentSide, -1, 1);

        if (previousSide * currentSide <= 0 && previousSide > currentSide) {
            Vector2 edge, e1, e2;
            int startSide, endSide;
            Vec2_Sub(&edge, &previousProjection, &currentProjection);
            edgeDirection = edge;
            Vec2_Sub(&e1, &previousProjection, &lineStart);
            firstOffset = e1;
            Vec2_Sub(&e2, &previousProjection, &lineEnd);
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
            mContactPoint.x = currentPos.x;
            mContactPoint.y = currentPos.y;
            mContactPoint.z = currentPos.z;
            mContactMotion.x = movementDelta.x;
            mContactMotion.y = movementDelta.y;
            mContactMotion.z = movementDelta.z;

            edgeRatio = FX_Div((currentAlong < 0) ? -currentAlong : currentAlong, mHalfWidth);
            {
                int halfLength = mHalfWidth;
                int absoluteAlong = (currentAlong < 0) ? -currentAlong : currentAlong;
                offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
            }

            if (offsetTimesHalfLength < 0x400000) {
                ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->contactState = 2;
                D3D_PlaySePannedLoop(0x1b1, mCenter.x,
                                     (sTrmpLinePushWeakPitchEnd * edgeRatio +
                                      sTrmpLinePushWeakPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            } else {
                ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->contactState = 1;
                D3D_PlaySePannedLoop(0x1ae, mCenter.x,
                                     (sTrmpLinePushStrongPitchEnd * edgeRatio +
                                      sTrmpLinePushStrongPitchMid * (0x1000 - edgeRatio)) >>
                                         12);
            }

            {
                Vector3 *lineNormal =
                    &((TrampolineContactView *)sTrmpJumpers[jumperIndex])->lineNormal;
                lineNormal->x = mNormal.x;
                lineNormal->y = mNormal.y;
                lineNormal->z = mNormal.z;
                ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->alongLine = currentAlong;
                ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->lineHalfLength = mHalfWidth;
                mWobblePhase = 0;
                PointDistance(&mContactPoint, &mContactMotion);

                mBouncesLeft -= 1;
                if (mBouncesLeft == 0) {
                    mStatus = 3;
                    SetStateSettle();
                    return 1;
                }
                mBounceCount += 1;
            }
        } else {
            Vec3_MulScalar(&halfLineForStart, &mAxis, mHalfWidth);
            Vec3_Add(&fromStart, &currentPos, &halfLineForStart);
            Vec3_MulScalar(&halfLineForEnd, &mAxis, mHalfWidth);
            Vec3_Sub(&fromEnd, &currentPos, &halfLineForEnd);

            if (fromStart.y < 0 && fromStart.y > -0x30000) {
                if (fromStart.x > -0x8000 && fromStart.x < 0) {
                    if (movementDelta.x > 0) {
                        ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->contactState = 3;
                    }
                }
            } else {
                if (fromEnd.y < 0 && fromEnd.y > -0x30000 && fromEnd.x > 0 && fromEnd.x < 0x8000 &&
                    movementDelta.x < 0) {
                    ((TrampolineContactView *)sTrmpJumpers[jumperIndex])->contactState = 3;
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
 *   Kept (#821 r82): plain startPoint[0] > endPoint[0], and the whole swap as
 *   an inline helper, both forward the stores -> 0x7f8 of 0x800.
 *
 * `colWeight += 0x555` sits at the tail of the inner loop with the other two
 * induction steps. Placed between the two `cell->x` statements instead, which is
 * where the earlier drafts had it, the whole 0x340..0x3e4 window schedules one
 * slot early and 35 words differ. Nothing else moves that residue: 246 pragma
 * names at on and off, every declaration order and type name, the loop form, the
 * pointer form and the multiply operand order are all inert on it.
 */

/* Kept (#821 r72): no spare4, or a Vector2 in its place, keeps Build at 0x800
   but changes its bytes. */
struct Spare4 {
    s32 v;
    ~Spare4() {}
};

void TrmpLine::Build(short *startPoint, short *endPoint) {
    Spare4 spare4;
    Vector3 startPosition, endPosition;
    Vector3 selectedPosition, selectedVelocity;
    Vector3 relativePosition, velocity;
    Vector3 endpointSum, midpoint, lineSpan, directionCopy, zAxis, lineNormal;
    Vector3 spareVec;
    short swapX, swapY;
    fx32 lineLength, rowStep;
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

    mStart[0] = startPoint[0];
    mStart[1] = startPoint[1];
    mEnd[0] = endPoint[0];
    mEnd[1] = endPoint[1];

    startPoint[0] = startPoint[0] - 0x80;
    startPoint[1] = -startPoint[1];
    endPoint[0] = endPoint[0] - 0x80;
    endPoint[1] = -endPoint[1];

    const s16 *startCmp = startPoint;
    const s16 *endCmp = endPoint;
    if (*startCmp > *endCmp) {
        swapX = startPoint[0];
        swapY = startPoint[1];
        startPoint[0] = endPoint[0];
        startPoint[1] = endPoint[1];
        endPoint[0] = swapX;
        endPoint[1] = swapY;
    }

    {
        s32 ax = startPoint[0] << 12;
        s32 ay = startPoint[1] << 12;
        startPosition.x = ax;
        startPosition.y = ay;
        startPosition.z = 0;
    }
    {
        s32 bx = endPoint[0] << 12;
        s32 by = endPoint[1] << 12;
        endPosition.x = bx;
        endPosition.y = by;
        endPosition.z = 0;
    }

    Vec3_Add(&endpointSum, &startPosition, &endPosition);
    Vec3_MulScalar(&midpoint, &endpointSum, 0x800);
    mCenter.x = midpoint.x;
    mCenter.y = midpoint.y;
    mCenter.z = midpoint.z;

    Vec3_Sub(&lineSpan, &endPosition, &startPosition);
    mAxis.x = lineSpan.x;
    mAxis.y = lineSpan.y;
    mAxis.z = lineSpan.z;

    func_0203ce80(&directionCopy, &mAxis);

    zAxis.x = 0;
    zAxis.y = 0;
    zAxis.z = 0x1000;
    Vec3_Cross(&lineNormal, &mAxis, &zAxis);
    mNormal.x = lineNormal.x;
    mNormal.y = lineNormal.y;
    mNormal.z = lineNormal.z;

    if (mNormal.y < 0) {
        mNormal.x = 0 - mNormal.x;
        mNormal.y = 0 - mNormal.y;
    }

    mContactPoint.x = 0;
    mContactPoint.y = 0;
    mContactPoint.z = 0;
    mContactMotion.x = 0 - mNormal.x;
    mContactMotion.y = 0 - mNormal.y;
    mContactMotion.z = 0 - mNormal.z;
    Vec3_MulScalarInPlace(&mContactMotion, 0x800);

    VEC_Subtract(&startPosition, &mCenter, &startPosition);
    VEC_Subtract(&endPosition, &mCenter, &endPosition);
    lineLength = Vec3_Dist(&startPosition, &endPosition);
    mHalfWidth = lineLength >> 1;
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
        cell = mVerts;
        mirror = mRestVerts;
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

                cell->z = accum - mHalfWidth;
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

    mWobblePhase = (s16)flag;
    mWobbleSpeed = 0x1400;
    mAlpha = 0x1F;
    PointDistance(&mContactPoint, &mContactMotion);
    ComputeNormals();

    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;

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

                VEC_Subtract(&relativePosition, &mCenter, &relativePosition);
                along = VEC_DotProduct(&mAxis, &relativePosition);
                across = VEC_DotProduct(&mNormal, &relativePosition);
                normalVelocity = VEC_DotProduct(&mNormal, &velocity);
                VEC_DotProduct(&mAxis, &velocity);

                if (normalVelocity < 0x100 && across <= selectedAcross && across > -0x24000) {
                    s32 halfLength = mHalfWidth;
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
        mContactPoint.x = selectedPosition.x;
        mContactPoint.y = selectedPosition.y;
        mContactPoint.z = selectedPosition.z;
        mContactMotion.x = selectedVelocity.x;
        mContactMotion.y = selectedVelocity.y;
        mContactMotion.z = selectedVelocity.z;

        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        edgeRatio = FX_Div(absoluteAlong, mHalfWidth);
        halfLength = mHalfWidth;
        absoluteAlong = (selectedAlong < 0) ? (0 - selectedAlong) : selectedAlong;
        offsetTimesHalfLength = FX_MUL(absoluteAlong, halfLength);
        if (offsetTimesHalfLength < 0x400000) {
            ((TrampolineContactView *)bestObj)->contactState = 2;
            D3D_PlaySePannedLoop(0x1B1, mCenter.x,
                                 (sTrmpLineWeakPitchEnd * edgeRatio +
                                  sTrmpLineWeakPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        } else {
            ((TrampolineContactView *)bestObj)->contactState = 1;
            D3D_PlaySePannedLoop(0x1AE, mCenter.x,
                                 (sTrmpLineStrongPitchEnd * edgeRatio +
                                  sTrmpLineStrongPitchMid * (0x1000 - edgeRatio)) >>
                                     0xC);
        }
        ((TrampolineContactView *)bestObj)->lineNormal.x = mNormal.x;
        ((TrampolineContactView *)bestObj)->lineNormal.y = mNormal.y;
        ((TrampolineContactView *)bestObj)->lineNormal.z = mNormal.z;
        ((TrampolineContactView *)bestObj)->alongLine = selectedAlong;
        ((TrampolineContactView *)bestObj)->lineHalfLength = mHalfWidth;
        mStatus = 3;
        PointDistance(&mContactPoint, &mContactMotion);
        SetStateSettle();
        return;
    }

    mLifeTimer = 600;
    mStatus = 1;
    SetStateFlex();
}

void TrmpLine::DrawAll() {
    Vector3 light;
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&gViewMtx);
    light.x = 0;
    light.y = 0;
    light.z = -0xff8;
    func_0203cd80(&light, -0x2000);
    G3_LightVector(GX_LIGHTID_1, light.x, light.y, light.z);
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
    int i;
    for (i = 0; i < 4; i++) {
        if (sTrmpLines[i].mStatus != 0) {
            sTrmpLines[i].Draw();
        }
    }
}

void TrmpLine::UpdateAll() {
    int i;
    for (i = 0; i < 4; i++) {
        if (sTrmpLines[i].mStatus != 0)
            sTrmpLines[i].Update();
    }
}

void TrmpLine::ClearAll(u32) {
    int i;
    for (i = 0; i < 3; i++) {
        sTrmpLines[i].mStatus = 0;
    }
}

void TrmpLine::LoadTextures() {
    sTrmpLineTexAddr = (void *)dExtSimpleModel_c::LoadTextureToVram(sTrmpLineTex, 0x400);
    sTrmpLinePlttAddr = (void *)func_02045a50(sTrmpLinePltt, 0x20);
}

/* The start line always takes the fourth slot, which TrmpLine::Create never hands out. */
void TrmpLine::CreateStartLineT2() {
    short start[2];
    short end[2];
    start[0] = 0x10;
    start[1] = 0xb0;
    end[0] = 0xf0;
    end[1] = 0xb0;
    sTrmpLines[3].Build(start, end);
    sTrmpLines[3].mBouncesLeft = 3;
    sTrmpLines[3].mBounceCount = 0;
    sTrmpLines[3].mSlotIndex = 3;
}

/* The start line always takes the fourth slot, which TrmpLine::Create never hands out. */
void TrmpLine::CreateStartLineT1() {
    short start[2];
    short end[2];
    start[0] = 0x18;
    start[1] = 0xb0;
    end[0] = 0xe8;
    end[1] = 0xb0;
    sTrmpLines[3].Build(start, end);
    sTrmpLines[3].mBouncesLeft = 3;
    sTrmpLines[3].mBounceCount = 0;
    sTrmpLines[3].mSlotIndex = 3;
}

/*
 * Try to place a new line segment between two 16-bit points.
 * Rejects an X span shorter than 8, a zero direction, or a normalized
 * direction whose absolute X component is below 1401. When the full
 * vector is shorter than 0x30000, rescale the direction using a length
 * with Y weighted by 0xc00 (0.75), then rewrite the endpoints around
 * the midpoint. Then find the first free slot (mStatus == 0)
 * among the three, rejecting it if the
 * segment crosses any active (state 1) slot's own segment (mStart/mEnd),
 * and initialises the slot through TrmpLine::Build, returning its
 * address (0 when none is free).
 *
 * Shape notes: the midpoint pair needs named x<<12 / y<<12 temps so the
 * y shift lands in place; the re-scale block reads direction.y inline
 * at both the multiply and the two-word copy into scaledDirection
 * (a named copy of it takes r4 ahead of the umull low word).
 */

int TrmpLine::Create(s16 *startPoint, s16 *endPoint) {
    s16 deltaPixels[2];
    Vector2 delta;
    Vector2 direction;
    Vector2 midpoint;
    Vector2 scaledDirection;
    Vector2 adjustedStart;
    Vector2 adjustedEnd;
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
    TrmpLine *candidateSlot;
    TrmpLine *p;
    int firstCross;
    int secondCross;
    long long weightedYProduct;
    int weightedY;
    int scaledDirectionLength;
    int slotOffset;
    TrmpLine *newLine;

    func_0203b958(deltaPixels, startPoint, endPoint);

    deltaX = deltaPixels[0];
    if (deltaX < 0)
        absoluteX = (s16)(-deltaX);
    else
        absoluteX = deltaX;
    if (absoluteX < 8)
        return 0;

    deltaY = deltaPixels[1];
    delta.x = deltaX << 12;
    delta.y = deltaY << 12;
    direction.x = (deltaX >> 1) << 12;
    direction.y = (deltaY >> 1) << 12;

    {
        int ay = startPoint[1];
        int by = endPoint[1];
        int ax = startPoint[0];
        int bx = endPoint[0];
        int y = (ay + by) >> 1;
        int x = (ax + bx) >> 1;
        int x12 = x << 12;
        int y12 = y << 12;
        midpoint.x = x12;
        midpoint.y = y12;
    }

    if (func_0203d434(&direction) == 0)
        return 0;

    {
        int absoluteDirectionX = direction.x;
        if (absoluteDirectionX < 0)
            absoluteDirectionX = -absoluteDirectionX;
        if (absoluteDirectionX < 1401)
            return 0;
    }

    if (Vec2_Len(&delta) < 0x30000) {
        weightedYProduct = (long long)direction.y * 0xc00 + 0x800;
        weightedY = (int)(weightedYProduct >> 12);
        scaledDirection.x = direction.x;
        scaledDirection.y = direction.y;
        scaledDirection.y = weightedY;
        scaledDirectionLength = Vec2_Len(&scaledDirection);
        func_0203d630(&direction, FX_Div(0x18000, scaledDirectionLength));

        func_0203d704(&adjustedStart, &midpoint, &direction);
        Vec2_Sub(&adjustedEnd, &midpoint, &direction);

        startPoint[0] = (s16)(adjustedStart.x >> 12);
        startPoint[1] = (s16)(adjustedStart.y >> 12);
        endPoint[0] = (s16)(adjustedEnd.x >> 12);
        endPoint[1] = (s16)(adjustedEnd.y >> 12);
    }

    candidateSlot = sTrmpLines;
    slotIndex = 0;
    do {
        if (candidateSlot->mStatus == 0) {
            p = sTrmpLines;
            otherSlotIndex = 0;
            do {
                if (slotIndex != otherSlotIndex) {
                    if (p->mStatus == 1) {
                        func_0203b958(testDirection, endPoint, startPoint);
                        func_0203b958(firstOffset, p->mStart, startPoint);
                        func_0203b958(secondOffset, p->mEnd, startPoint);

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
                            func_0203b958(otherDirection, p->mEnd, p->mStart);
                            testDirection[0] = otherDirection[0];
                            testDirection[1] = otherDirection[1];
                            func_0203b958(firstEndpointOffset, startPoint, p->mStart);
                            firstOffset[0] = firstEndpointOffset[0];
                            firstOffset[1] = firstEndpointOffset[1];
                            func_0203b958(secondEndpointOffset, endPoint, p->mStart);
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
                p++;
            } while (otherSlotIndex < 4);

            startCopy[0] = startPoint[0];
            startCopy[1] = startPoint[1];
            endCopy[0] = endPoint[0];
            endCopy[1] = endPoint[1];
            slotOffset = slotIndex * 0x32c;
            newLine = &sTrmpLines[slotIndex];
            newLine->Build(startCopy, endCopy);
            *(s16 *)((int)&sTrmpLines[0].mBouncesLeft + slotOffset) = 1;
            *(u16 *)((int)&sTrmpLines[0].mBounceCount + slotOffset) = 0;
            *(s16 *)((int)&sTrmpLines[0].mSlotIndex + slotOffset) = (s16)slotIndex;
            return (int)newLine;
        }
        slotIndex++;
        candidateSlot++;
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

/* Lane goals, then the saved copy at +0x10 (sAmidaLaneSaved). */
int sAmidaLaneTargets[8];
