//cpp
#include "dFdBrightness_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "math/Approach.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g3x.h"
#include "nitro/fx/fx_cp.h"
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_trampoline2_AssetId {
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCG_BIN = 0x00a2,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCL_BIN = 0x00a3,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NSC_BIN = 0x00a4,
    ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN = 0x0108,
    ASSET_MINI_STAR_BMD = 0x0219
};

#pragma opt_loop_invariants off
/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in.
 *
 * Functions are in reverse ROM order; do not reorder.
 * opt_loop_invariants is off for the whole file. Measured 2026-11-06:
 * PaintSquare is the only function in the TU whose bytes move when
 * invariants are hoisted, and the setting still cannot be narrowed to it --
 * this TU leaves codegen deferred, so mwccarm reads the state at the end of
 * the file and an `on` placed after PaintSquare (or after the function
 * following it) puts hoisting back for PaintSquare as well. TUBUILD CONFLICT
 * lines stay; the manifest pairs them.
 *
 * leftovers:
 * - 0x7acc has no matched read.
 * - NewCleanParticle takes Fix12 by value, which costs the caller bytes
 *   (notes/mwccarm-codegen.md 6az).
 * - The 0x78-element pool helpers stay free functions over char *; they
 *   read the record through Trmp2Spinner.h's view. Their
 *   pointer locals (Init's p1..p3, Update's p), StatePlay's level pointer
 *   and its unread raw hold the pinned @1110: folding any of them into a
 *   direct member expression renumbers it (measured).
 * - TrmpItem's +0x1a8 array (five Vector2s, mUnusedVec) has no known role.
 */

#include "common.h"
#include "types.h"
#include "dScMgTrampoline2_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "TrmpItem.h"
#include "TrmpLine.h"
#include "Trmp2Spinner.h"
#include "decl_common.h"
#include "dMgTrmplnShared.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "math/MtxStack.h"
#include "LevelRng.h"
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"

namespace ns_020ca7b8 {
int Trmp2Mario_Hide();
}
using ns_020ca7b8::Trmp2Mario_Hide;
namespace ns_020ca840 {
void Trmp2Mario_EnterAll();
}
using ns_020ca840::Trmp2Mario_EnterAll;
namespace ns_020ca8e0 {
void Trmp2Mario_MoveToStart();
}
using ns_020ca8e0::Trmp2Mario_MoveToStart;
namespace ns_020caa08 {
void Trmp2Mario_PlaceAtStart();
}
using ns_020caa08::Trmp2Mario_PlaceAtStart;
namespace ns_020cac30 {
void Trmp2Mario_UpdateAll();
}
using ns_020cac30::Trmp2Mario_UpdateAll;

/* local: the ROM re-reads the slot byte at each use here, so keep the read volatile. */
#define ACTIVE_SLOT (*(volatile unsigned char *)&gActivePlayerSlot)
/* local: these readers only reproduce through a 4-byte-record view of the lane. */
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))

/* The forward declarations below are local stand-ins for types that have
 * no header yet (issue #2497). */

/* The cartridge's ctor/dtor imports are the functions the wrapper's aliases
 * name. */
struct Trampoline2ModelFilePtr : SharedFilePtr {
    Trampoline2ModelFilePtr(u32 fileID);
    ~Trampoline2ModelFilePtr();
};

struct BMD_File;

struct Vector3_16f;


struct Tbl {
    int a, b, c;
};

typedef unsigned char u8;

typedef unsigned short u16;

/* A scripted wave supplies one position and three parameters per item.
 * Positions are Q12; angles rotate the item's forward vector in the XY plane. */
struct Trampoline2WaveDef {
    int count;
    const VecFx32 *positions;
    const short *angles;
    const short *itemTypes;
    const short *motionTypes;
};

/* TUBUILD CONFLICT -- alternate body of struct 'dExtSimpleModel_c', from the legacy file for Trmp2Spinner_Destruct, NOT applied:
struct dExtSimpleModel_c { ~dExtSimpleModel_c(); };
*/

void Trmp2Spinner_Update(char *obj);
void Trmp2Spinner_Init(void *obj, int hitCount);
void Trmp2Spinner_SetModel(void *self);
int Trmp2_ColumnForKind(int kind);
unsigned short Trmp2_OccupancyBits(int column, int side);
int Trmp2_PickFaceA(int kind);
int Trmp2_PickFaceB(int kind);
dScMgTrampoline2_c *dScMgTrampoline2_c_classInit(void);

extern "C" {
extern void Trmp2Spinner_UpdateMatrix(char *c);
extern Trampoline2ModelFilePtr sTrmp2StarModelFile;
extern int sTrmp2SpinnerCount;
extern unsigned char *sTrmp2Spinners;
extern int sTrmp2BrushWord[];
extern const u16 sTrmp2BrushPltt[];
extern dFdBrightness_c *gSceneFader;
extern int sTrmp2TouchBlocked;
void func_0203cd80(int *m, short angle);
unsigned int d_s_mg_trampoline2NewCleanParticleHold(unsigned int a, unsigned int b, int c, int d,
                                                    int e, const Vector3_16f *f);
void Jump2Popup_UpdateAll(void);
extern const VecFx32 sTrmp2RandomSpawnPositions[];
extern void func_0203cc28(int *vector, int angle);
extern int sTrmpItemsHit;
extern int sTrmp2Lives;
extern int sTrmp2Score;
extern int sTrmp2MarioCount;
extern int sTrmpItemsActive;
extern int sTrmp2ScoreGates[];
extern const Trampoline2WaveDef sTrmp2SpawnDefs[];
extern s16 sTrmp2IntroFromX;
extern s16 sTrmp2IntroToX;
extern s16 sTrmpStartTouchY;
extern short sTrmpStartTouchX;
extern volatile s16 data_020a0dbc[];
extern u8 gMainScreenLayers;
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *ptr);
extern int sTrmp2NcgByLanguage[];
extern int sTrmpPuffEnglish[];
extern int sTrmpPuffSpanish[];
extern int sTrmpPuffFrench[];
extern int sTrmpPuffGerman[];
extern int sTrmpPuffItalian[];
extern OamAttr *sMgTextSpritesEnglish[];
extern OamAttr *sMgTextSpritesSpanish[];
extern OamAttr *sMgTextSpritesFrench[];
extern OamAttr *sMgTextSpritesGerman[];
extern OamAttr *sMgTextSpritesItalian[];
/* Both callers use the existing ov006 helpers' void(void) contracts. The
 * unrelated daPly_c::St_Null_Init at the same RAM address belongs to ov002. */

/* Data owned by this production TU; the original TU grouping is inferred. */
int sTrmp2TouchBlocked = 1;
BaseSpawnInfo<dScMgTrampoline2_c> g_profile_MG_TRAMPOLINE2 = {
    dScMgTrampoline2_c_classInit,
    0x181,
    0x181,
};
Tbl sTrmp2SpinnerSpeeds = {0x600, 0xc00, 0xe00};
int sTrmp2ScoreGates[] = {1000, 5000, 10000, 20000};
int sTrmp2PuffXByLanguage[] = {(int)sTrmpPuffEnglish, (int)sTrmpPuffSpanish, (int)sTrmpPuffFrench,
                               (int)sTrmpPuffGerman, (int)sTrmpPuffItalian};
int sTrmp2NcgByLanguage[] = {0x107, 0x109, 0x104, 0x105, 0x106};
OamAttr **sTrmp2TextSpritesByLanguage[] = {sMgTextSpritesEnglish, sMgTextSpritesSpanish,
                                           sMgTextSpritesFrench, sMgTextSpritesGerman,
                                           sMgTextSpritesItalian};
}

namespace s20ad90c {
void Mg_CommitRecords(void);
}
using s20ad90c::Mg_CommitRecords;
namespace s20ad674 {
int GetGameLanguage(void);
}
using s20ad674::GetGameLanguage;
namespace s20afcf8 {
void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);
}
using s20afcf8::DrawOamSprite;
namespace s20afa20 {
void RenderOamMainScreen(OamAttr *a0, int a1, int a2, int a3, int a4);
}
using s20afa20::RenderOamMainScreen;
int func_020126e8(int a);
int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

/* The three models, mTexAnim and mUnusedVec are all built before the
   empty body, in declaration order. */
TrmpItem::TrmpItem() {}

/* `new dScMgTrampoline2_c` reproduces it: the inline dScMgD3DBase_c
   constructor (dScMgBase_c's C2, its vtable, mDrawSubScreen, the empty loop
   over the two cameras, mParticle), then this class's vtable and the six
   element arrays. Historical alias: MgTrampolineTerror_Spawn. */
dScMgTrampoline2_c *dScMgTrampoline2_c_classInit(void) {
    return new dScMgTrampoline2_c;
}

void dScMgTrampoline2_c::InitDisplay() {
    void *objChar, *objPltt, *bgChar, *bgScr, *bgPltt;
    volatile u16 sp0;

    objChar = LoadFile(sTrmp2NcgByLanguage[GetGameLanguage()]);
    objPltt = LoadFile(ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN);
    bgChar = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCG_BIN);
    bgScr = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NSC_BIN);
    bgPltt = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCL_BIN);

    G2_SetBG2Priority(2);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                         GX_BG_CHARBASE_0x10000);
    G2_BG2Mosaic(0);

    {
        char *dst = (char *)G2_GetBG2CharPtr();
        sp0 = 0;
        MI_CpuFill16(dst, sp0, 0x4000);
    }
    DecompressLZ16(bgChar, ((char *)G2_GetBG2CharPtr() + 0x4000));
    DecompressLZ16(bgScr, G2_GetBG2ScrPtr());
    GX_LoadBGPltt(bgPltt, 0x1a0, 0x60);

    G2_SetBG3Priority(1);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                         GX_BG_CHARBASE_0x18000);
    G2_BG3Mosaic(0);

    DecompressLZ16(objChar, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(objPltt, 0, 0x60);

    Deallocate(bgChar);
    Deallocate(bgScr);
    Deallocate(bgPltt);
    Deallocate(objChar);
    Deallocate(objPltt);

    InitBrush(3);
}

/* Vtable slot 0 (the name is inferred): load the screens, set up both
 * cameras and the object pools, then start a round through StartRound. */
s32 dScMgTrampoline2_c::InitResources() {
    s32 fov;

    InitDisplay();
    gMainScreenLayers =
        DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ;
    G3X_SetFog(false, 0, 2, 0x1000);
    G3X_AlphaBlend(true);
    InitialiseVramGlobals();
    G2_SetBG0Priority(1);
    fov = FX_Div(0xc0000, data_02082414);
    this->mCameras[0].mLookAt.x = 0;
    this->mCameras[0].mLookAt.y = -0x64000;
    this->mCameras[0].mLookAt.z = 0;
    this->mCameras[0].mPos.x = 0;
    this->mCameras[0].mPos.y = 0;
    this->mCameras[0].mPos.z = fov;
    this->mCameras[0].mFovAngle = 0x400;
    Camera_UpdateMatrices(&mCameras[0]);
    this->mCameras[1].mLookAt.x = 0;
    this->mCameras[1].mLookAt.y = 0x82000;
    this->mCameras[1].mLookAt.z = 0;
    this->mCameras[1].mPos.x = 0;
    this->mCameras[1].mPos.y = 0;
    this->mCameras[1].mPos.z = fov;
    this->mCameras[1].mFovAngle = 0x400;
    Camera_UpdateMatrices(&mCameras[1]);
    if (Trmp2Mario_InitPool(mMarios, 5) == 0)
        return 0;
    TrmpFx_InitPool(mScorePopups, 5);
    TrmpLine::LoadTextures();
    TrmpItem::LoadAll((char *)mItems, 10);
    Jump2Popup_InitPool((int)mPopups, 10);
    data_ov006_021421b4 = 0;
    Trmp2Spinner_LoadAll((int)mSpinners, 0x14);
    Mg_SetTopScreenYOffset(0x10);
    StartRound(-1);
    return 1;
}

/* Vtable slot 18: reset the round. */
void dScMgTrampoline2_c::StartRound(int /* arg */) {
    G2_SetBlendAlpha(1, 0x2e, 0x10, 0x10);

    mTouchStartX = data_020a0dbc[0];
    mTouchStartY = data_020a0dbc[1];
    mTouchX = data_020a0dbc[0];
    mTouchY = data_020a0dbc[1];

    mLevel = 0;
    if (mLevel > 0x270e)
        mLevel = 0x270e;

    Trmp2Mario_SetTuning(0x1000);
    Trmp2Mario_ResetAll(mLevel);
    sTrmp2MarioCount = 1;
    TrmpLine::ClearAll(mLevel);
    TrmpItem::HideAll(mLevel);
    Jump2Popup_ClearAll();
    Trmp2Spinner_HideAll();
    TrmpFx_ClearAll();
    Puff_ClearAll(0, sTrmp2PuffXByLanguage[GetGameLanguage()]);

    mInputEnabled = 0;
    mRoundOver = 0;
    mPattern = 0;
    mScriptDone = 0;
    mWaveStep = 0;
    mRamp = 0x14000;

    {
        volatile u16 z;
        char *dst = (char *)G2_GetBG3CharPtr();
        z = 0;
        MI_CpuFill16(dst, z, 0x6000);
    }

    BeginIntro();
}

/* CanEndState: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::CanEndState(int /* mode */) {
    Puff_ClearAll(0, sTrmp2PuffXByLanguage[GetGameLanguage()]);
    return 1;
}

void dScMgTrampoline2_c::BeginIntro() {
    short a, b;
    mTimer = 0x5a;
    a = sTrmpStartTouchX;
    mTouchStartX = a;
    b = sTrmpStartTouchY;
    mTouchStartY = b;
    mTouchX = a;
    mTouchY = b;
    mDragSoundHandle = 0;
    mScoreGate = 0x3e8;
    mState = &dScMgTrampoline2_c::StateIntro;
}

void dScMgTrampoline2_c::StateIntro() {
    int counter;

    TrmpLine::UpdateAll();
    mTimer -= 1;
    counter = mTimer;

    if (counter == 0) {
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }

        {
            int dst = (int)G2_GetBG3CharPtr();
            volatile u16 tmp = 0;
            MI_CpuFill16((void *)dst, tmp, 0x6000);
        }

        TrmpLine::CreateStartLineT2();
        func_02012718(0x1af, 0x8000);
        BeginPlay();
        return;
    }

    {
        int t = FX_Div(counter << 12, 0x5a000);
        volatile s16 old9c = mTouchX;
        volatile s16 old9e = mTouchY;
        int mixRaw = sTrmp2IntroToX * t + sTrmp2IntroFromX * (0x1000 - t);
        mTouchX = (mixRaw >> 12);
        mTouchY = sTrmpStartTouchY;

        mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650.value) & ~0x80000000) >> 19) -
               0x800)
              << 2) >>
             12);

        Mg_DrawLine(old9c, old9e, mTouchX, mTouchY, 2, 0xc);

        mDragSoundHandle =
            func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0, func_020126e8(mixRaw), 0, 0);
    }
}

void dScMgTrampoline2_c::BeginPlay() {
    mTimer = 0xb4;
    mInputEnabled = 1;
    /* The callee ignores r0; this + 0x7b00 lands mid-element in mScorePopups. */
    Trmp2Mario_PlaceAtStart();
    mState = &dScMgTrampoline2_c::StatePlay;
}

void dScMgTrampoline2_c::StatePlay() {
    char *raw = (char *)this; /* unread; deleting it renumbers the pinned @1110 */
    int old;

    old = sTrmpItemsHit;
    TrmpLine::UpdateAll();
    TrmpItem::UpdateAll();
    Trmp2Mario_UpdateAll();
    if (old < sTrmpItemsHit) {
        int n = sTrmpItemsHit - old;
        int i;
        for (i = 0; i < n; i++)
            ApproachLinear(mRamp, 0x7000, -0x800);
        if (sTrmpItemsHit == 3)
            mPromptEnabled = 0;
    }
    Trmp2Mario_SetTuning((sTrmpItemsHit / 10) * 64 + 0x1000);
    if (sTrmp2Lives == 0) {
        mInputEnabled = 0;
        BeginResults();
        return;
    }
    if (sTrmp2Score >= mScoreGate && sTrmp2MarioCount < 5) {
        unsigned int v;
        Trmp2Mario_MoveToStart();
        {
            u32 *p = &mLevel;
            *p = *p + 1;
        }
        if (mLevel > 0x270e)
            mLevel = 0x270e;
        v = mLevel;
        if (v < 4)
            mScoreGate = sTrmp2ScoreGates[v];
        else
            mScoreGate = 0x7fffffff;
        return;
    }
    if (sTrmpItemsActive != 0)
        return;
    if (ApproachLinear(mTimer, 0, 1) == 0)
        return;
    if (mScriptDone == 0) {
        const Trampoline2WaveDef *d = &sTrmp2SpawnDefs[mPattern];
        int i;
        VecFx32 vecA;
        for (i = 0; i < d->count; i++) {
            vecA.x = 0;
            vecA.y = 0x1000;
            vecA.z = 0;
            func_0203cc28((int *)&vecA, d->angles[i]);
            TrmpItem::Spawn((int)&d->positions[i], (int *)&vecA, d->itemTypes[i],
                            d->motionTypes[i]);
        }
        if (ApproachLinear(mPattern, 6, 1)) {
            mScriptDone = 1;
            mWaveStep = 0;
        }
    } else {
        if (ApproachLinear(mWaveStep, 4, 1)) {
            unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff;
            const Trampoline2WaveDef *d = &sTrmp2SpawnDefs[(int)((rnd >> 19) * 6) >> 12];
            int i;
            VecFx32 vecB;
            for (i = 0; i < d->count; i++) {
                vecB.x = 0;
                vecB.y = 0x1000;
                vecB.z = 0;
                func_0203cc28((int *)&vecB, d->angles[i]);
                TrmpItem::Spawn((int)&d->positions[i], (int *)&vecB, d->itemTypes[i],
                                d->motionTypes[i]);
            }
            mWaveStep = 0;
        } else {
            SpawnWave();
        }
    }
    mTimer = 0x78;
}

void dScMgTrampoline2_c::BeginResults() {
    Trmp2Mario_EnterAll();
    mTimer = 0x5a;
    mState = &dScMgTrampoline2_c::StateResults;
}

void dScMgTrampoline2_c::StateResults() {
    TrmpItem::UpdateAll();
    Trmp2Mario_UpdateAll();
    if (ApproachLinear(mTimer, 0, 1) == 0)
        return;
    if (Trmp2Mario_Hide() == 0)
        return;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    mTimer = 0xb4;
    mState = &dScMgTrampoline2_c::StateWaitExit;
}

void dScMgTrampoline2_c::StateWaitExit() {
    int idx, b;
    mTimer -= 1;
    if (mTimer != 0) {
        idx = ACTIVE_SLOT;
        b = 0;
        if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    {
        int r;
        volatile unsigned short h;
        r = (int)G2_GetBG3CharPtr();
        h = 0;
        MI_CpuFill16((void *)r, h, 0x6000);
    }
    mRoundOver = 1;
    mState = &dScMgTrampoline2_c::StateDone;
}

void dScMgTrampoline2_c::StateDone() {}

void dScMgTrampoline2_c::UpdateTouchInput() {
    int i;
    int b;

    if (mInputEnabled == 0 || mTouchReleased != 0) {
        mTouching = 0;
        return;
    }

    i = ACTIVE_SLOT;
    b = (TOUCH_REC(gTouchHeld)[i][0] != 0 && TOUCH_REC(gTouchEdge)[i][0] != 0);
    if (b) {
        mTouchX = TOUCH_REC(gTouchX)[i][0];
        mTouchStartX = mTouchX;
        mTouchY = TOUCH_REC(gTouchY)[i][0];
        mTouchStartY = mTouchY;
        mTouching = 1;
        mDragSoundHandle = 0;
    }

    if (mTouching != 1)
        return;

    i = ACTIVE_SLOT;
    if (TOUCH_REC(gTouchHeld)[i][0] != 0) {
        Mg_DrawLine(mTouchX, mTouchY, TOUCH_REC(gTouchX)[i][0], TOUCH_REC(gTouchY)[i][0], 2, 4);
        i = ACTIVE_SLOT;
        mTouchX = TOUCH_REC(gTouchX)[i][0];
        mTouchY = TOUCH_REC(gTouchY)[i][0];
        mDragSoundHandle =
            func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0, func_020126e8(mTouchX << 12), 0, 0);
    } else {
        b = (TOUCH_REC(gTouchHeld)[i][0] == 0 && TOUCH_REC(gTouchEdge)[i][0] != 0);
        if (b != 0) {
            mTouchReleased = 1;
        }
    }
}

int Trmp2_ColumnForKind(int x) {
    switch (x) {
    case 0:
    case 1:
    case 2:
        return 0;
    case 5:
    case 8:
        return 1;
    case 3:
    case 4:
        return 2;
    case 6:
    case 9:
        return 3;
    case 7:
    case 10:
        return 4;
    }
    return 0;
}

unsigned short Trmp2_OccupancyBits(int sel, int b) {
    unsigned short r = 0;
    switch (sel) {
    case 0:
        if (b == 0)
            r |= 4;
        break;
    case 1:
        if (b == 0)
            r |= 0x60;
        else
            r |= 2;
        break;
    case 2:
        if (b == 0)
            r |= 0x48;
        break;
    case 3:
        if (b == 0)
            r |= 0x100;
        else
            r |= 0x28;
        break;
    case 4:
        if (b == 1)
            r |= 0x80;
        break;
    }
    return r;
}

int Trmp2_PickFaceA(int x) {
    int r;
    if (x < 5) {
        r = (((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13);
        return r < 0xc00 ? 4 : 5;
    }
    r = (((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13);
    return r < 0xc00 ? 6 : 7;
}

int Trmp2_PickFaceB(int a) {
    int r;
    if (a < 5) {
        r = (((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13);
        if (r < 0xc00)
            return 4;
        if (r < 0xe00)
            return 5;
        return 8;
    }
    r = (((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13);
    if (r < 0xc00)
        return 6;
    if (r < 0xe00)
        return 8;
    return 7;
}

void dScMgTrampoline2_c::SpawnWave() {
    int occupancy;
    int column;
    int kind;
    int kinds[7];
    short faces[7];
    VecFx32 up;
    int i;
    int j;
    int variant;

    occupancy = 0;
    for (i = 0; i < 5; i++) {
        do {
            kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >>
                          0x13) *
                         11) >>
                   12;
        } while ((kind == 7 || kind == 10) &&
                 (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >>
                       0x13) > 0x400);

        for (;;) {
            for (j = 0; j < i; j++) {
                if (kind == kinds[j]) {
                    kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650.value) &
                                   0x7fffffff) >>
                                  0x13) *
                                 11) >>
                           12;
                    j = -1;
                }
            }

            column = Trmp2_ColumnForKind(kind);
            {
                int shift = column * 2;
                int rightBit = 2 << shift;
                int leftBit = 1 << shift;
                int savedOccupancy;

                if ((occupancy & rightBit) != 0 && (occupancy & leftBit) != 0) {
                    do {
                        if (ApproachLinear(kind, 11, 1) != 0)
                            kind = 0;
                    } while ((kind == 7 || kind == 10) &&
                             (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) &
                                    0x7fffffff) >>
                                   0x13) > 0x800);
                    continue;
                }

                if ((savedOccupancy = (int)occupancy, (savedOccupancy & rightBit) != 0)) {
                    kinds[i] = kind;
                    faces[i] = 8;
                    occupancy |= Trmp2_OccupancyBits(column, 1);
                } else if ((savedOccupancy & leftBit) != 0) {
                    kinds[i] = kind;
                    faces[i] = Trmp2_PickFaceA(kind);
                    occupancy |= Trmp2_OccupancyBits(column, 0);
                } else {
                    kinds[i] = kind;
                    faces[i] = Trmp2_PickFaceB(kind);
                    switch (faces[i]) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        occupancy |= Trmp2_OccupancyBits(column, 0);
                        break;
                    default:
                        occupancy |= Trmp2_OccupancyBits(column, 1);
                        break;
                    }
                }
            }
            break;
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (kind >= 5 || ((kind & 1) != 0 && (faces[i] == 8 || faces[i] == 5))) {
            func_0203cc28((int *)&up, 0x4000);
        }

        variant = 1;
        if (faces[i] == 6)
            variant = 2;
        TrmpItem::Spawn((int)&sTrmp2RandomSpawnPositions[kinds[i]], (int *)&up, variant, faces[i]);
    }
}

/* Steps the score-pop list, the particle, and the current state.
   mRamp is written into the particle's byte at +0x58. */
s32 dScMgTrampoline2_c::Behavior() {
    int saved = sTrmp2Score;

    TrmpFx_UpdateAll();
    Jump2Popup_UpdateAll();
    Trmp2Spinner_UpdateAll();
    mParticleID =
        Particle::System::NewCleanParticle(mParticleID, 0xf0, 0x280000, 0x700000, -0x580000, 0);
    Particle::System *trail = Particle::System::FromUniqueID(mParticleID);
    if (trail != 0) {
        trail->emitInterval = mRamp >> 12;
        ApproachLinear(mRamp, 0x14000, 0x200);
    }
    (this->*mState)();
    UpdateTouchInput();
    if (saved != sTrmp2Score) {
        Mg_SubmitScore(sTrmp2Score);
    }
    return 1;
}

/* Vtable slot 9. Light 0 points down -z tilted -0x4000 about x; lights 0 and
 * 1 are white. */
s32 dScMgTrampoline2_c::Render() {
    int m[3];
    int count;
    int a1v;
    int i;

    m[0] = 0;
    m[1] = 0;
    m[2] = -0xff8;
    func_0203cd80(m, -0x4000);

    G3_LightVector(GX_LIGHTID_0, m[0], m[1], m[2]);
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));

    TrmpFx_DrawAll();
    Jump2Popup_DrawAll();

    if (mDrawSubScreen == 1) {
        count = sTrmp2Lives;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite(sTrmp2TextSpritesByLanguage[idx][1], a1v, 0xc, 0);
            } else {
                RenderOamMainScreen(sTrmpLifeIconSprite, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    Mg_DrawTopRightCounter(sTrmp2Score, 6);

    if (mRoundOver == 0) {
        Trmp2Mario_DrawAll();
        TrmpLine::DrawAll();
        TrmpItem::UpdateMatricesAll();
    }
    Trmp2Spinner_UpdateMatricesAll();
    return 1;
}

/* Vtable slot 3: two teardown calls. */
s32 dScMgTrampoline2_c::CleanupResources() {
    TrmpItem::ReleaseFiles();
    Mg_CommitRecords();
    return 1;
}

/* OnGraphCallback2: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback2() {
    if (!dScMgD3DBase_c::OnGraphCallback2())
        return 0;
    if (mMenuOpen == 0) {
        if (mDrawSubScreen == 0) {
            gMainScreenLayers = gMainScreenLayers & ~DISPLAY_LAYER_BG3;
            SetBg2Offset(0, 0);
            G2_SetBG2Offset(0, 0);
        } else {
            gMainScreenLayers = gMainScreenLayers | DISPLAY_LAYER_BG3;
            SetBg2Offset(0, 0xc0);
            G2_SetBG2Offset(0, 0xc0);
        }
    }
    return 1;
}

/* OnGraphCallback3: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback3() {
    return dScMgD3DBase_c::OnGraphCallback3() != 0;
}

/* OnGraphCallback0: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback0() {

    u16 buf[5];

    if (!dScMgBase_c::OnGraphCallback0())
        return 0;

    if (gSceneFader->IsAtStart()) {
        sTrmp2TouchBlocked = 0;
    } else if (gSceneFader->IsBetweenStartAndEnd()) {
        sTrmp2TouchBlocked = 1;
    }

    if (sTrmp2TouchBlocked != 0)
        return 0;

    if (mMenuOpen != 0)
        return 0;

    if (mTouchReleased != 0) {
        s16 a = *(volatile s16 *)&mTouchStartX;
        if ((a < 0x18 && *(volatile s16 *)&mTouchX < 0x18) ||
            (a > 0xe8 && *(volatile s16 *)&mTouchX > 0xe8)) {
            func_02012790(0xe);
        } else {
            buf[1] = mTouchStartX;
            buf[2] = mTouchStartY;
            buf[3] = mTouchX;
            buf[4] = mTouchY;
            if (TrmpLine::Create((s16 *)&buf[1], (s16 *)&buf[3])) {
                func_02012718(0x1af, (mTouchStartX + mTouchX) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        {
            char *dst = (char *)G2_GetBG3CharPtr();
            buf[0] = 0;
            MI_CpuFill16(dst, buf[0], 0x6000);
        }
        mTouching = 0;
        mTouchReleased = 0;
    }
    return 1;
}

void dScMgTrampoline2_c::InitBrush(int layer) {
    dScMgTrampoline2_c *self = this;
    volatile unsigned short t0;
    volatile unsigned short t1;
    int v = 0x7000;
    char *scrPtr;
    char *charPtr;
    int i;

    self->mBrushOnMain = 1;
    self->mBrushLayer = layer;

    switch (self->mBrushLayer) {
    case 0:
        scrPtr = (char *)G2_GetBG0ScrPtr();
        charPtr = (char *)G2_GetBG0CharPtr();
        break;
    case 1:
        scrPtr = (char *)G2_GetBG1ScrPtr();
        charPtr = (char *)G2_GetBG1CharPtr();
        break;
    case 2:
        scrPtr = (char *)G2_GetBG2ScrPtr();
        charPtr = (char *)G2_GetBG2CharPtr();
        break;
    case 3:
        scrPtr = (char *)G2_GetBG3ScrPtr();
        charPtr = (char *)G2_GetBG3CharPtr();
        break;
    default:
        return;
    }

    for (i = 0; i < 0x300; i++) {
        t0 = v;
        MI_CpuFill16(scrPtr, t0, 2);
        v = (unsigned short)(v + 1);
        scrPtr += 2;
    }

    t1 = 0;
    MI_CpuFill16(charPtr, t1, 0x6000);
    GX_LoadBGPltt(sTrmp2BrushPltt, 0xe0, 0x20);
}

/* Vtable slot 34 (name not evidenced): plot an n by n square of 4-bit pixels
 * into the current touch-screen layer. The same body as dScMgTrampoline_c's,
 * with its own scratch buffer. */
void dScMgTrampoline2_c::PaintSquare(int x_base, int y, int val, int n) {
    void *obj = this;

    int half;
    int x0;
    int j;
    int x;
    int *addr;
    int i;
    int yy;
    int mask = 0xf;

    half = n / 2;
    j = 0;
    if (j < n) {
        yy = y - half;
        x0 = x_base - half;
        do {
            for (i = 0; i < n; i++) {
                x = i + x0;
                if (x >= 0 && x < 0x100) {
                    if (yy >= 0 && yy < 0xc0) {
                        void *ret;
                        int mode = ((dScMgTrampoline2_c *)obj)->mBrushLayer;

                        switch (mode) {
                        case 0:
                            ret = G2_GetBG0CharPtr();
                            break;
                        case 1:
                            ret = G2_GetBG1CharPtr();
                            break;
                        case 2:
                            ret = (char *)G2_GetBG2CharPtr();
                            break;
                        case 3:
                            ret = G2_GetBG3CharPtr();
                            break;
                        default:
                            return;
                        }

                        addr = (int *)((char *)ret + ((x / 8) + (yy / 8) * 32) * 32 + (yy & 7) * 4);

                        MI_CpuCopy32(addr, sTrmp2BrushWord, 4);
                        sTrmp2BrushWord[0] = (sTrmp2BrushWord[0] & (-1 ^ (mask << ((x & 7) * 4)))) |
                                             (val << ((x & 7) * 4));
                        MI_CpuCopy32(sTrmp2BrushWord, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

#define SPINNER(p) ((Trmp2Spinner *)(p))

Trmp2Spinner::Trmp2Spinner() {
    mLifeTimer = 0;
}

/* mModel's destructor runs after the body. */
Trmp2Spinner::~Trmp2Spinner() {
    sTrmp2StarModelFile.Release();
}

extern "C" {
void Trmp2Spinner_LoadAll(int x, int count) {
    int i;
    sTrmp2Spinners = (unsigned char *)x;
    sTrmp2SpinnerCount = count;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        Trmp2Spinner_SetModel((sTrmp2Spinners + i * 0x78));
    }
}
}

extern "C" {
void Trmp2Spinner_InitAll(int hitCount) {
    int i = 0;
    int n = sTrmp2SpinnerCount;
    unsigned char *base;
    unsigned char *p;
    if (n <= 0) {
        return;
    }
    base = sTrmp2Spinners;
    p = base;
    do {
        if (((Trmp2Spinner *)p)->mLifeTimer == 0) {
            Trmp2Spinner_Init(base + (i * 0x78), hitCount);
            return;
        }
        i++;
        p += 0x78;
    } while (i < sTrmp2SpinnerCount);
}
}

extern "C" {
void Trmp2Spinner_HideAll(void) {
    int i = 0;
    if (sTrmp2SpinnerCount > 0) {
        int off = 0;
        do {
            ((Trmp2Spinner *)(sTrmp2Spinners + off))->mLifeTimer = 0;
            i++;
            off += 0x78;
        } while (i < sTrmp2SpinnerCount);
    }
}
}

extern "C" {
void Trmp2Spinner_UpdateAll(void) {
    int i;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        char *obj = (char *)sTrmp2Spinners + i * 0x78;
        if (((Trmp2Spinner *)obj)->mLifeTimer != 0) {
            Trmp2Spinner_Update(obj);
        }
    }
}
}

extern "C" {
void Trmp2Spinner_UpdateMatricesAll(void) {
    int i;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        char *obj = (char *)sTrmp2Spinners + i * 0x78;
        if (((Trmp2Spinner *)obj)->mLifeTimer != 0) {
            Trmp2Spinner_UpdateMatrix(obj);
        }
    }
}
}

void Trmp2Spinner_SetModel(void *raw) {
    BMD_File *file = dExtSimpleModel_c::LoadFile(sTrmp2StarModelFile);
    ((dExtModel_c *)raw)->SetFile(file, 1, -1);
}

extern "C" {
extern struct Tbl sTrmp2SpinnerSpeeds;
extern const u16 sTrmp2SpinnerColors[];
}
void func_020169f4(char *self, int kind);

void Trmp2Spinner_Init(void *r0, int value) {
    Trmp2Spinner *spinner = (Trmp2Spinner *)r0;
    struct Tbl t = sTrmp2SpinnerSpeeds;
    unsigned int rnd = ((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13;
    int idx;
    if ((int)rnd < t.a) {
        idx = 0;
    } else if ((int)rnd < t.b) {
        idx = 1;
    } else if ((int)rnd < t.c) {
        idx = 2;
    } else {
        idx = 3;
    }
    func_020169f4((char *)spinner, sTrmp2SpinnerColors[idx]);
    int scale = value * 0xA00;
    spinner->mLifeTimer = 0xF0;
    if (scale > 0x4000) {
        scale = 0x4000;
    }
    int size = scale + 0x4000;
    spinner->mScale.x = size;
    spinner->mScale.y = size;
    spinner->mScale.z = size;
    spinner->mPos.x = 0x60000;
    spinner->mPos.y = 0x100000;
    spinner->mPos.z = 0x18000;
    int drop = -0x1C00;
    spinner->mVel.x = drop;
    spinner->mVel.y = drop;
    spinner->mVel.z = 0;
    unsigned int rnd2 = RandomIntInternal(&data_0209e650.value);
    int *p1 = &spinner->mPos.x;
    int v1 = *p1 + ((((rnd2 & 0x7fffffff) >> 0x13) - 0x800) * 0x40);
    *p1 = v1;
    int rnd3 = RandomIntInternal(&data_0209e650.value);
    int *p2 = &spinner->mVel.x;
    int adj3 = (((unsigned int)rnd3 & 0x7fffffff) >> 0x13);
    *p2 -= adj3 >> 1;
    int rnd4 = RandomIntInternal(&data_0209e650.value);
    int *p3 = &spinner->mVel.y;
    int adj4 = (((unsigned int)rnd4 & 0x7fffffff) >> 0x13);
    *p3 -= adj4 >> 1;
    func_02016a14((char *)spinner, 0);
    func_02016a04((char *)spinner, 0);
}

void Trmp2Spinner_Update(char *raw) {
    ApproachLinear2(SPINNER(raw)->mLifeTimer, 0, 1);
    short *p = &SPINNER(raw)->mAngleZ;
    *p = *p + 0x400;
    VEC_Add(&SPINNER(raw)->mPos, &SPINNER(raw)->mVel, &SPINNER(raw)->mPos);
}

extern "C" void Trmp2Spinner_UpdateMatrix(char *raw) {
    Matrix4x3_FromTranslation(&data_020a0e68, SPINNER(raw)->mPos.x, SPINNER(raw)->mPos.y,
                              SPINNER(raw)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, SPINNER(raw)->mAngleZ);
    SPINNER(raw)->mModel.mat4x3 = data_020a0e68;
    SPINNER(raw)->mModel.Render(&SPINNER(raw)->mScale);
}

/* The body is empty; mUnusedVec, mTexAnim and the models are destroyed
   after it, in reverse declaration order. */
TrmpItem::~TrmpItem() {}

/* D1 and D0 come from the inline destructor in dScMgTrampoline2_c.h. */

/* sTrmp2StarModelFile is the model SharedFilePtr this Init LoadFile's,
 * defined here at the end of the file (constructed as file ID 537) so its
 * constructor does not enter .text. */
Trampoline2ModelFilePtr sTrmp2StarModelFile(ASSET_MINI_STAR_BMD);

/* Scripted wave constants and random-wave positions. The array lengths follow
 * the observed consumers; original declaration grouping is not recovered.
 * The pinned linker's ALIGNALL(4) separates odd-length short arrays naturally.
 * Palette and spinner colors use the native 16-bit DS palette format. */
extern const short sTrmp2Wave3MotionTypes[2] = {2, 2};
extern const short sTrmp2Wave3Angles[2] = {0x4000, 0x4000};
extern const short sTrmp2Wave3ItemTypes[2] = {2, 2};
extern const short sTrmp2Wave4Angles[3] = {0, 0, 0};
extern const short sTrmp2Wave4MotionTypes[3] = {3, 3, 3};
extern const short sTrmp2Wave0Angles[3] = {0x4000, 0x4000, 0x4000};
extern const short sTrmp2Wave0ItemTypes[3] = {2, 1, 2};
extern const short sTrmp2Wave1ItemTypes[3] = {2, 1, 2};
extern const short sTrmp2Wave4ItemTypes[3] = {1, 1, 1};
extern const short sTrmp2Wave0MotionTypes[3] = {0, 0, 0};
extern const short sTrmp2Wave1MotionTypes[3] = {0, 0, 0};
extern const short sTrmp2Wave1Angles[3] = {0x4000, 0x4000, 0x4000};
extern const u16 sTrmp2SpinnerColors[4] = {0x7fff, 0x03ff, 0x001f, 0x7c00};
extern const short sTrmp2Wave2Angles[5] = {0, 0, 0, 0, 0};
extern const short sTrmp2Wave5Angles[5] = {-0x2000, -0x2000, -0x2000, -0x2000, -0x2000};
extern const short sTrmp2Wave2ItemTypes[5] = {2, 1, 2, 1, 0};
extern const short sTrmp2Wave5ItemTypes[5] = {2, 2, 2, 2, 2};
extern const short sTrmp2Wave2MotionTypes[5] = {1, 1, 1, 1, 1};
extern const short sTrmp2Wave5MotionTypes[5] = {0, 0, 0, 0, 0};
extern const VecFx32 sTrmp2Wave3Positions[2] = {{0x54000, 0x70000, 0x0}, {-0x54000, 0x70000, 0x0}};
extern const u16 sTrmp2BrushPltt[16] = {0x0000, 0x0000, 0x7fff, 0x0000, 0x0000, 0x0000,
                                        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                                        0x0000, 0x0000, 0x0000, 0x0000};
extern const VecFx32 sTrmp2Wave0Positions[3] = {
    {0x40000, 0x48000, 0x0}, {0x40000, 0xb0000, 0x0}, {0x40000, -0x30000, 0x0}};
extern const VecFx32 sTrmp2Wave1Positions[3] = {
    {-0x40000, 0x48000, 0x0}, {-0x40000, 0xb0000, 0x0}, {-0x40000, -0x30000, 0x0}};
extern const VecFx32 sTrmp2Wave4Positions[3] = {
    {-0x40000, 0x50000, 0x0}, {0x0, 0x88000, 0x0}, {0x40000, 0x50000, 0x0}};
extern const VecFx32 sTrmp2Wave2Positions[5] = {{-0x38000, 0x30000, 0x0},
                                                {0x40000, 0x48000, 0x0},
                                                {-0x38000, 0x60000, 0x0},
                                                {0x40000, 0x78000, 0x0},
                                                {-0x48000, 0x90000, 0x0}};
extern const VecFx32 sTrmp2Wave5Positions[5] = {{-0x3c000, 0x38000, 0x0},
                                                {-0x1e000, 0x56000, 0x0},
                                                {0x0, 0x74000, 0x0},
                                                {0x1e000, 0x92000, 0x0},
                                                {0x3c000, 0xb0000, 0x0}};
extern const VecFx32 sTrmp2RandomSpawnPositions[11] = {
    {0x0, 0xc0000, 0x0},      {0x0, 0x9a000, 0x0},       {0x0, 0x74000, 0x0},
    {0x0, 0x4e000, 0x0},      {0x0, 0x28000, 0x0},       {-0x50000, 0x98000, 0x0},
    {-0x50000, 0x3c000, 0x0}, {-0x50000, -0x20000, 0x0}, {0x50000, 0x98000, 0x0},
    {0x50000, 0x3c000, 0x0},  {0x50000, -0x20000, 0x0}};
extern const Trampoline2WaveDef sTrmp2SpawnDefs[6] = {
    {3, sTrmp2Wave0Positions, sTrmp2Wave0Angles, sTrmp2Wave0ItemTypes, sTrmp2Wave0MotionTypes},
    {3, sTrmp2Wave1Positions, sTrmp2Wave1Angles, sTrmp2Wave1ItemTypes, sTrmp2Wave1MotionTypes},
    {5, sTrmp2Wave2Positions, sTrmp2Wave2Angles, sTrmp2Wave2ItemTypes, sTrmp2Wave2MotionTypes},
    {2, sTrmp2Wave3Positions, sTrmp2Wave3Angles, sTrmp2Wave3ItemTypes, sTrmp2Wave3MotionTypes},
    {3, sTrmp2Wave4Positions, sTrmp2Wave4Angles, sTrmp2Wave4ItemTypes, sTrmp2Wave4MotionTypes},
    {5, sTrmp2Wave5Positions, sTrmp2Wave5Angles, sTrmp2Wave5ItemTypes, sTrmp2Wave5MotionTypes}};
