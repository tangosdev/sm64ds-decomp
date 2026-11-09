//cpp
#include "dFdBrightness_c.h"
#include "nitro/hw/registers.h"
#include "math/Approach.h"
/* dScMgTrampoline_c -- Trampoline Time. Draw a line on the touch screen
 * (up to three at once) and bounce Mario to the lit door. Three misses
 * end the round.
 *
 * Functions are written in reverse of ROM order; do not reorder.
 * `opt_loop_invariants off` above PaintSquare has no matching `on`, so it
 * applies to the whole file. Measured 2026-11-06: PaintSquare is the only
 * function in the TU whose bytes move when invariants are hoisted, and the
 * setting still cannot be narrowed to it -- this TU leaves codegen deferred,
 * so mwccarm reads the state at the end of the file and an `on` placed after
 * PaintSquare (or after the function following it) puts hoisting back for
 * PaintSquare as well. TUBUILD CONFLICT comments are paired with the
 * manifest; leave them.
 *
 * leftovers:
 * - The score pool (TrmpFx_Unlink..TrmpFx_InitPool) stays free:
 *   it works on the file-scope live-list globals, not one object, and
 *   TrmpFx_Spawn is the entry dMgTrmpln2Mario_c calls.
 * - ov004 helpers and most data symbols are unnamed.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_trampoline_AssetId {
    ASSET_D_2D_MG_BG_TRAMPOLINE_NCG_BIN = 0x00a5,
    ASSET_D_2D_MG_BG_TRAMPOLINE_NCL_BIN = 0x00a6,
    ASSET_D_2D_MG_BG_TRAMPOLINE_NSC_BIN = 0x00a7,
    ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN = 0x0108
};

#include "dScMgTrampoline_c.h"
#include "TrmpScorePopup.h"
#include "SpawnInfo.h"
#include "types.h"
#include "G2x.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "LevelRng.h"
#include "decl_common.h"
/* local: UpdateTouchInput only reproduces through a 4-byte-record view of
   each lane; the flat `gTouchHeld[slot * 4]` and TouchInfo forms DIFF there
   (measured 2026-11-08). */
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))

namespace cstd {
int fdiv(int numerator, int denominator);
}
namespace G2 {
void *GetBG0ScrPtr();
void *GetBG1ScrPtr();
void *GetBG2ScrPtr();
void *GetBG3ScrPtr();
void *GetBG2CharPtr();
}
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace G3X {
void SetFog(int enable, int blend, int slope, int offset);
}

typedef struct Vec2s {
    s16 x, y;
} Vec2s;

/* Eight-byte state descriptors. A pointer-to-member initializer
   (`TrampState pmf = &dScMgTrampoline_c::StateIntro` and the same for
   StatePlay, StateResults, StateWaitExit) was measured on this TU: the four
   symbols move from .data to .bss (0x20), __sinit_d_s_mg_trampoline.cpp
   is emitted at size 0x94 with a .ctor word, and the (code, 0) words survive only as
   anonymous .data temps that __sinit copies. .text stayed 0x1e10. The ROM
   keeps these four in .data, so the mangled externs stay. StateDone is
   empty, so it is static and its address is an ordinary function pointer. */
struct P2 {
    int a, b;
};

// Preserve the original front end's blind two-word aggregate copy in C++ mode.
struct P2Words {
    int words[2];
};

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_021218c4, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_02121cf4, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_02121f04, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied:
struct Obj {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int a);
};
*/

extern "C" {
extern void Mg_RenderSpriteBoth(void *a, int b, int c, int d, int e);
extern int sTrmpRowScreenY[];
extern int sTrmpRowMarkerSprite;
extern void Mg_DrawBigNumber(int, int, int, int, int, int, int);
extern void TrmpFx_Link(TrmpScorePopup *entry);
extern TrmpScorePopup *sTrmpFxList;
extern int sTrmpFxCount;
extern TrmpScorePopup *sTrmpFxPool;

extern int sTrmpBrushWord[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern const unsigned char sTrmpBrushPltt[];
extern dFdBrightness_c *gSceneFader;
extern int sTrmpTouchBlocked;
int TrmpLine_Create(s16 *a, s16 *b);

void SetBg2Offset(int a, int b);
int Mg_GetTopScreenYOffset(void);
extern unsigned char gMainScreenLayers;
extern void Mg_CommitRecords(void);
int GetGameLanguage(void);
struct OamAttr;
struct Matrix2x2;
void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);
void RenderOamMainScreen(OamAttr *a0, int a1, int a2, int a3, int a4);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern short sTrmpGoalRow;

extern void Trmp3DMario_UpdateAll(void);
extern int Trmp3DMario_CountFinished(void);
extern void Mg_SetSharedState(int);
extern void Trmp3DMario_StartAll(void);
extern int sTrmpComboCount;
extern int sTrmp3DLives;
extern int sTrmpMariosInPlay;
extern int sTrmpAnyOnLedge;
extern void TrmpLine_UpdateAll(void);
extern int Trmp3DMario_IsOffstage(int *r0);
extern void Trmp3DMario_EnterWalkIn(char *c);
extern int RandomIntInternal(int *seed);
extern void Trmp3DMario_SetSpawnHeight(int n);
extern void Trmp3DMario_SetTuning(int a);
void TrmpLine_CreateStartLineT1(void);
extern s16 sTrmpIntroFromX;
extern s16 sTrmpStartTouchX;
extern s16 sTrmpStartTouchY;
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *ptr);
extern int sTrmpNcgByLanguage[];
extern void InitialiseVramGlobals(void);
extern s16 data_02082414;
extern int Trmp3DMario_InitPool(unsigned char *, int);
extern void TrmpFx_InitPool(void *, int);
extern void TrmpLine_LoadTextures(void);
extern void Mg_SetTopScreenYOffset(int);
extern void Trmp3DMario_ResetAll(unsigned int, int);
extern void TrmpFx_ClearAll(void);
extern int sTrmpPuffXByLanguage[];
extern void Puff_ClearAll(int, int);
extern void func_02012718(int, int);
extern void TrmpFx_UpdateAll(void);
extern void Mg_SubmitScore(int);
extern void TrmpFx_DrawAll(void);
extern void Mg_DrawTopRightCounter(int, int);
extern void *sTrmpDoorSignSprites[];
extern void *sTrmpArrowSprite;
extern void Trmp3DMario_DrawAll(void);
extern void TrmpLine_DrawAll(void);

dScMgTrampoline_c *dScMgTrampoline_c_classInit(void);
/* The four non-static state methods. A pointer to member does not convert
   to the int word a descriptor holds, and a pointer-to-member initializer
   is not static .data (see the note above the P2 definition). StateDone is
   static, so the descriptor below names it directly. */
extern void _ZN17dScMgTrampoline_c13StateWaitExitEv(void *); /* local extern: PMF -> int */
extern void _ZN17dScMgTrampoline_c12StateResultsEv(void *);  /* local extern: PMF -> int */
extern void _ZN17dScMgTrampoline_c9StatePlayEv(void *);      /* local extern: PMF -> int */
extern void _ZN17dScMgTrampoline_c10StateIntroEv(void *);    /* local extern: PMF -> int */
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

/* Data owned by the same original TU. */
P2 pmf_dScMgTrampoline_c_StateWaitExit = {(int)_ZN17dScMgTrampoline_c13StateWaitExitEv, 0};
P2 pmf_dScMgTrampoline_c_StateDone = {(int)&dScMgTrampoline_c::StateDone, 0};
P2 pmf_dScMgTrampoline_c_StateIntro = {(int)_ZN17dScMgTrampoline_c10StateIntroEv, 0};
BaseSpawnInfo<dScMgTrampoline_c> g_profile_MG_TRAMPOLINE = {
    dScMgTrampoline_c_classInit,
    0x180,
    0x180,
};
P2 pmf_dScMgTrampoline_c_StatePlay = {(int)_ZN17dScMgTrampoline_c9StatePlayEv, 0};
P2 pmf_dScMgTrampoline_c_StateResults = {(int)_ZN17dScMgTrampoline_c12StateResultsEv, 0};
int sTrmpNcgByLanguage[] = {0x107, 0x109, 0x104, 0x105, 0x106};
OamAttr **sTrmpTextSpritesByLanguage[] = {sMgTextSpritesEnglish, sMgTextSpritesSpanish,
                                          sMgTextSpritesFrench, sMgTextSpritesGerman,
                                          sMgTextSpritesItalian};
int sTrmpPuffXByLanguage[] = {(int)sTrmpPuffEnglish, (int)sTrmpPuffSpanish, (int)sTrmpPuffFrench,
                              (int)sTrmpPuffGerman, (int)sTrmpPuffItalian};
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_Link, NOT applied: extern void *sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_DrawAll, NOT applied: extern void *sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxPool, from the legacy file for TrmpFx_ClearAll, NOT applied: extern char* sTrmpFxPool; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_ClearAll, NOT applied: extern int sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_02120f18, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054efc, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054efc(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054ea8, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054ea8(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for _ZN17dScMgTrampoline_c16OnGraphCallback0Ev, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN17dScMgTrampoline_c16OnGraphCallback0Ev, NOT applied: void MultiStore16(u16 val, void* dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of Mg_RenderSpriteBoth, from the legacy file for _ZN17dScMgTrampoline_c6RenderEv, NOT applied: void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121778, NOT applied: extern void MultiStore16(unsigned short a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of Trmp3DMario_UpdateAll, from the legacy file for func_ov006_021218fc, NOT applied: extern void Trmp3DMario_UpdateAll(...); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of _Z15ApproachLinear2Rsss, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z15ApproachLinear2Rsss(short *v, short step, short rate); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_021218fc, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int bank); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpComboCount, from the legacy file for func_ov006_02121bc8, NOT applied: extern "C" int sTrmpComboCount; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_02120a18, from the legacy file for func_ov006_02121cf4, NOT applied: extern void func_ov006_02120a18(char* p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_02121cf4, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n); */
/* TUBUILD CONFLICT -- alternate declaration of TrmpLine_UpdateAll, from the legacy file for func_ov006_02121d64, NOT applied: void TrmpLine_UpdateAll(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02121d64, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121d64, NOT applied: void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02121d64, NOT applied: void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov006_02121d64, NOT applied: int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of func_020126e8, from the legacy file for func_ov006_02121d64, NOT applied: int func_020126e8(int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02121d64, NOT applied: int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchX, from the legacy file for func_ov006_02121f04, NOT applied: extern short sTrmpStartTouchX; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchY, from the legacy file for func_ov006_02121f04, NOT applied: extern short sTrmpStartTouchY; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN17dScMgTrampoline_c11CanEndStateEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd4fdivEii, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern s32 _ZN4cstd4fdivEii(s32 a, s32 b); */
/* TUBUILD CONFLICT -- alternate declaration of gMainScreenLayers, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern u8 gMainScreenLayers; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_0212231c, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_0212231c, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_0212231c, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_0212231c, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */
}

TrmpScorePopup::TrmpScorePopup() {}

/* `new dScMgTrampoline_c` reproduces it: the inline dScMgD3DBase_c
   constructor, then this class's vtable, the three element arrays and the
   door mark's clear. */
extern "C" dScMgTrampoline_c *dScMgTrampoline_c_classInit(void) {
    return new dScMgTrampoline_c;
}

void dScMgTrampoline_c::InitDisplay() {
    void *bgChar, *bgScr, *bgPltt, *objChar, *objPltt;
    volatile u16 fill;

    bgChar = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NCG_BIN);
    bgScr = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NSC_BIN);
    bgPltt = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NCL_BIN);
    objChar = LoadFile(sTrmpNcgByLanguage[GetGameLanguage()]);
    objPltt = LoadFile(ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN);

    REG_BG2CNT = (REG_BG2CNT & ~3) | 2;
    REG_BG2CNT = (REG_BG2CNT & 0x43) | 0x9110;
    REG_BG2CNT &= ~0x40;

    {
        char *dst = (char *)G2::GetBG2CharPtr();
        fill = 0;
        MultiStore16(fill, dst, 0x4000);
    }
    DecompressLZ16(bgChar, (void *)((char *)G2::GetBG2CharPtr() + 0x4000));
    DecompressLZ16(bgScr, (void *)G2::GetBG2ScrPtr());
    GX::LoadBGPltt(bgPltt, 0x1a0, 0x60);

    REG_BG3CNT = (REG_BG3CNT & ~3) | 1;
    REG_BG3CNT = (REG_BG3CNT & 0x43) | 0x1318;
    REG_BG3CNT &= ~0x40;

    DecompressLZ16(objChar, (void *)0x6400000);
    GX::LoadOBJPltt(objPltt, 0, 0x60);

    Deallocate(bgChar);
    Deallocate(bgScr);
    Deallocate(bgPltt);
    Deallocate(objChar);
    Deallocate(objPltt);

    InitBrush(3);
}

/* Slot 0. Sets up the two cameras and the fog, then ends with a virtual
 * call to StartRound(-1), the scene's reset hook. */
s32 dScMgTrampoline_c::InitResources() {
    s32 fov;

    this->mScrollY = 0x20;
    this->mScrollTargetY = this->mScrollY;
    InitDisplay();
    gMainScreenLayers =
        DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ;
    G3X::SetFog(false, 0, 2, 0x1000);
    REG_DISP3DCNT = (REG_DISP3DCNT & ~0x3000) | 8;
    InitialiseVramGlobals();
    REG_BG0CNT = (REG_BG0CNT & ~3) | 1;
    fov = cstd::fdiv(0xc0000, (s32)data_02082414);
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
    if (Trmp3DMario_InitPool((unsigned char *)mMarios, 4) == 0)
        return 0;
    TrmpFx_InitPool(mScorePopups, 5);
    TrmpLine_LoadTextures();
    Mg_SetTopScreenYOffset(0x10);
    StartRound(-1);
    return 1;
}

extern "C" void TrmpLine_ClearAll(u32);
extern volatile s16 data_020a0dbc[];

void dScMgTrampoline_c::StartRound(int /* arg */) {
    volatile unsigned short fill;
    int heightMix;

    G2x::SetBlendAlpha(&REG_BLDCNT, 1, 0x3e, 0x10, 0x10);

    mTouchStartX = data_020a0dbc[0];
    mTouchStartY = data_020a0dbc[1];
    mTouchX = data_020a0dbc[0];
    mTouchY = data_020a0dbc[1];

    sTrmpAnyOnLedge = 0;
    mLevel = 0;
    if (mLevel > 0x270e)
        mLevel = 0x270e;
    /* mLevel is u32 on dScMgBase_c (dScMg3DEsp_c's evidence). This function
       divides and takes a remainder of it, where signedness IS codegen -- it
       reads the field SIGNED, which is what this header used to declare
       locally. Cast at the use site rather than retype the shared base. */
    heightMix = (((s32)mLevel % 5) << 12) / 4;
    mScrollY = FX_MUL(0x1000 - heightMix, 0x20) + FX_MUL(heightMix, 0x50);
    mScrollTargetY = mScrollY;
    mScrollHoldTimer = 0;
    Trmp3DMario_ResetAll(mLevel, mScrollY);

    TrmpLine_ClearAll(mLevel);
    TrmpFx_ClearAll();
    Puff_ClearAll(0, sTrmpPuffXByLanguage[GetGameLanguage()]);

    mDoorMark.Hide();

    mInputEnabled = 0;
    mRoundOver = 0;
    mDoorSwitchTimer =
        (((int)(((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 0x13) *
          0x2d0) >>
         12) +
        0x2d0;
    mDoorBlinkTimer = 0;
    mDoorBlinkPhase = 1;
    mArrow1X = 0;
    mArrow2X = 0;

    SetDoorSide(0);

    {
        char *dst = (char *)func_02054d88();
        fill = 0;
        MultiStore16(fill, dst, 0x6000);
    }

    BeginIntro();
}

/* dScMgTrampoline_c::CanEndState, from its vtable slot. */
int dScMgTrampoline_c::CanEndState(int /* mode */) {
    Puff_ClearAll(0, sTrmpPuffXByLanguage[GetGameLanguage()]);
    return 1;
}

void dScMgTrampoline_c::BeginIntro() {
    short startX;
    short startY;

    mTimer = 0x5a;
    startX = sTrmpStartTouchX;
    mTouchStartX = startX;
    startY = sTrmpStartTouchY;
    mTouchStartY = startY;
    mTouchX = startX;
    mTouchY = startY;
    mDragSoundHandle = 0;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateIntro;
}

void dScMgTrampoline_c::StateIntro() {
    int counter;

    TrmpLine_UpdateAll();
    mTimer -= 1;
    counter = mTimer;

    if (counter == 0) {
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }

        {
            int dst = func_02054d88();
            volatile u16 tmp = 0;
            MultiStore16(tmp, (char *)dst, 0x6000);
        }

        TrmpLine_CreateStartLineT1();
        func_02012718(0x1af, 0x8000);
        BeginPlay();
        return;
    }

    {
        int t = cstd::fdiv(counter << 12, 0x5a000);
        /* Kept volatile: the ROM spills the previous point to the stack
           across the random draw; plain locals DIFF (measured 2026-11-08). */
        volatile s16 prevX = mTouchX;
        volatile s16 prevY = mTouchY;
        int penX = sTrmpStartTouchX * t + sTrmpIntroFromX * (0x1000 - t);

        mTouchX = (s16)(penX >> 12);
        mTouchY = sTrmpStartTouchY;

        mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650.value) & ~0x80000000) >> 19) -
               0x800)
              << 2) >>
             12);

        Mg_DrawLine(prevX, prevY, mTouchX, mTouchY, 2, 0xc);

        mDragSoundHandle =
            func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0, func_020126e8(penX), 0, 0);
    }
}

void dScMgTrampoline_c::BeginPlay() {
    mTimer = 0x1e;
    mInputEnabled = 1;
    mDoorMark.Show(mDoorSide);
    mDragSoundHandle = 0;
    Sound::PlayBank2_2D(0x1b6);
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StatePlay;
}

void dScMgTrampoline_c::UpdateScroll() {
    int level = sTrmpComboCount / 10;
    int oldLevel = mLevel;
    Trmp3DMario_SetSpawnHeight(mScrollY);
    Trmp3DMario_SetTuning((level << 8) + 0x1000);
    if (oldLevel != level) {
        mLevel = level;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
        if (mScrollTargetY < 0x50) {
            mScrollHoldTimer = 0x78;
            Sound::PlayBank2_2D(0x1b2);
        }
    }
    if (ApproachLinear(mScrollHoldTimer, 0, 1) != 0) {
        if (mScrollY == mScrollTargetY) {
            int target;
            mScrollTargetY = (level << 3) + 0x20;
            target = mScrollTargetY;
            if (target > 0x50)
                target = 0x50;
            mScrollTargetY = target;
        }
        ApproachLinear(mScrollY, mScrollTargetY, 2);
        mScrollOffsetY = 0;
    } else {
        mScrollOffsetY = (mScrollHoldTimer & 2) >> 1;
    }
}

void dScMgTrampoline_c::StatePlay() {
    int oldCombo = sTrmpComboCount;
    TrmpLine_UpdateAll();
    Trmp3DMario_UpdateAll();
    if (sTrmpComboCount > oldCombo) {
        if (sTrmpComboCount == 3)
            mPromptEnabled = 0;
    }
    UpdateScroll();

    if (sTrmp3DLives == 0) {
        mInputEnabled = 0;
        BeginResults();
    } else {
        if (ApproachLinear(mTimer, 0, 1)) {
            if (sTrmpComboCount >= 5 || sTrmpMariosInPlay < 3) {
                int i;
                for (i = 0; i < 4; i++) {
                    if (Trmp3DMario_IsOffstage((int *)&mMarios[i])) {
                        Trmp3DMario_EnterWalkIn((char *)&mMarios[i]);
                        break;
                    }
                }
            }
            mTimer = 0x168;
            {
                int speedUp = sTrmpComboCount * 2;
                if (speedUp > 0xb4)
                    speedUp = 0xb4;
                mTimer -= speedUp;
            }
        }
    }

    if (sTrmpAnyOnLedge == 0) {
        if (ApproachLinear2(mDoorSwitchTimer, 0, 1)) {
            if (GetDoorSide()) {
                SetDoorSide(0);
                mDoorMark.Show(mDoorSide);
            } else {
                SetDoorSide(1);
                mDoorMark.Show(mDoorSide);
            }
            Sound::PlayBank2_2D(0x1b6);
            {
                unsigned int rnd =
                    (unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff;
                int scaled = (rnd >> 19) * 0x2d0;
                mDoorSwitchTimer = (s16)((scaled >> 12) + 0x2d0);
                mDoorBlinkTimer = 0;
                mDoorBlinkPhase = 1;
            }
        }
    }

    if (mDoorSwitchTimer <= 0x3c) {
        if (ApproachLinear2(mDoorBlinkTimer, 0, 1)) {
            mDoorBlinkTimer = 0xa;
            if (mDoorBlinkPhase != 0)
                mDoorBlinkPhase = 0;
            else
                mDoorBlinkPhase = 1;
        }
    }

    if (mDoorSide != 0) {
        ApproachLinear(mArrow1X, 0, 1);
        ApproachLinear(mArrow2X, 0x20, 1);
    } else {
        ApproachLinear(mArrow1X, 0x20, 1);
        ApproachLinear(mArrow2X, 0, 1);
    }
}

void dScMgTrampoline_c::BeginResults() {
    Trmp3DMario_StartAll();
    mTimer = 0x5a;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateResults;
}

void dScMgTrampoline_c::StateResults() {
    Trmp3DMario_UpdateAll();
    if (ApproachLinear(mTimer, 0, 1) == 0)
        return;
    if (Trmp3DMario_CountFinished() == 0)
        return;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    mTimer = 0xb4;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateWaitExit;
}

void dScMgTrampoline_c::StateWaitExit() {
    int slot, tapped;
    mTimer -= 1;
    if (mTimer != 0) {
        slot = gActivePlayerSlot;
        tapped = 0;
        if (gTouchHeld[slot * 4] != 0 && gTouchEdge[slot * 4] != 0)
            tapped = 1;
        if (tapped == 0)
            return;
    }
    {
        int dst;
        volatile unsigned short fill;
        dst = func_02054d88();
        fill = 0;
        MultiStore16(fill, (char *)dst, 0x6000);
    }
    mRoundOver = 1;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateDone;
}

void dScMgTrampoline_c::StateDone() {}

s16 dScMgTrampoline_c::GetDoorSide() {
    return mDoorSide;
}

void dScMgTrampoline_c::SetDoorSide(short value) {
    sTrmpGoalRow = value;
    mDoorSide = value;
}

void dScMgTrampoline_c::UpdateTouchInput() {
    int slot;
    int edge;

    if (mInputEnabled == 0) {
        mTouching = 0;
        return;
    }

    slot = gActivePlayerSlot;
    edge = (TOUCH_REC(gTouchHeld)[slot][0] != 0 && TOUCH_REC(gTouchEdge)[slot][0] != 0);
    if (edge) {
        mTouchX = TOUCH_REC(gTouchX)[slot][0];
        mTouchStartX = mTouchX;
        mTouchY = TOUCH_REC(gTouchY)[slot][0];
        mTouchStartY = mTouchY;
        mTouching = 1;
        mDragSoundHandle = 0;
    }

    if (mTouching != 1)
        return;

    slot = gActivePlayerSlot;
    if (TOUCH_REC(gTouchHeld)[slot][0] != 0) {
        Mg_DrawLine(mTouchX, mTouchY, TOUCH_REC(gTouchX)[slot][0], TOUCH_REC(gTouchY)[slot][0], 2,
                    4);
        slot = gActivePlayerSlot;
        mTouchX = TOUCH_REC(gTouchX)[slot][0];
        mTouchY = TOUCH_REC(gTouchY)[slot][0];
        mDragSoundHandle =
            func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0, func_020126e8(mTouchX << 12), 0, 0);
    } else {
        edge = (TOUCH_REC(gTouchHeld)[slot][0] == 0 && TOUCH_REC(gTouchEdge)[slot][0] != 0);
        if (edge != 0) {
            mTouchReleased = 1;
        }
    }
}

/* Slot 6. The field at 0x5004 is a pointer to member function: the current
 * state, one of the five State functions. The header keeps it as two raw
 * words, so it is cast to dScMgTrampoline_c::State here to dispatch. */
s32 dScMgTrampoline_c::Behavior() {
    int saved = sTrmpComboCount;
    TrmpFx_UpdateAll();
    (this->*(*(State *)mState))();
    UpdateTouchInput();
    mDoorMark.Update();
    if (saved != sTrmpComboCount)
        Mg_SubmitScore(sTrmpComboCount);
    return 1;
}

/* Draws the three miss icons, the two door arrows, and the flashing mark.
   The arrows slide in mArrow1X / mArrow2X; mDoorBlinkPhase picks the frame
   on the lit side. */
s32 dScMgTrampoline_c::Render() {
    int lives;
    int iconX;
    int i;
    int side;
    int door0Frame, door1Frame;
    int rowY;

    mDoorMark.Render(mScrollY);
    TrmpFx_DrawAll();

    if (mDrawSubScreen == 1) {
        lives = sTrmp3DLives;
        iconX = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= lives) {
                int language = GetGameLanguage();
                DrawOamSprite(sTrmpTextSpritesByLanguage[language][1], iconX, 0xc, 0);
            } else {
                RenderOamMainScreen(sTrmpLifeIconSprite, iconX, 0xc, -1, -1);
            }
            iconX += 0x12;
        }
    }

    Mg_DrawTopRightCounter(sTrmpComboCount, 4);

    side = mDoorSide;
    door0Frame = 1;
    door1Frame = 1;
    if (side == 0) {
        if (mDoorBlinkPhase != 0)
            door0Frame = 0;
    }
    if (side != 0) {
        if (mDoorBlinkPhase != 0)
            door1Frame = 0;
    }

    rowY = sTrmpRowScreenY[0] - (mScrollY + mScrollOffsetY);
    Mg_RenderSpriteBoth(sTrmpArrowSprite, mArrow1X + 0xf0, rowY, -1, 2);
    Mg_RenderSpriteBoth(sTrmpDoorSignSprites[door0Frame], 0xf0, rowY - 0x20, -1, 2);

    rowY = sTrmpRowScreenY[1] - (mScrollY + mScrollOffsetY);
    Mg_RenderSpriteBoth(sTrmpArrowSprite, mArrow2X + 0xf0, rowY, -1, 2);
    Mg_RenderSpriteBoth(sTrmpDoorSignSprites[door1Frame], 0xf0, rowY - 0x20, -1, 2);

    if (mRoundOver == 0) {
        Trmp3DMario_DrawAll();
        TrmpLine_DrawAll();
    }
    return 1;
}

/* Slot 3. */
s32 dScMgTrampoline_c::CleanupResources() {
    Mg_CommitRecords();
    return 1;
}

/* dScMgTrampoline_c::OnGraphCallback2, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback2() {
    if (!dScMgD3DBase_c::OnGraphCallback2())
        return 0;
    if (mMenuOpen == 0) {
        if (mDrawSubScreen == 0) {
            gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
            SetBg2Offset(0, mScrollY + mScrollOffsetY);
            REG_BG2OFS = (mScrollY + mScrollOffsetY) << 16 & 0x1ff0000;
        } else {
            gMainScreenLayers |= DISPLAY_LAYER_BG3;
            SetBg2Offset(0, mScrollY + 0xc0 + mScrollOffsetY + Mg_GetTopScreenYOffset());
            REG_BG2OFS =
                (mScrollY + 0xc0 + mScrollOffsetY + Mg_GetTopScreenYOffset()) << 16 & 0x1ff0000;
        }
    }
    return 1;
}

/* dScMgTrampoline_c::OnGraphCallback3, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback3() {
    return dScMgD3DBase_c::OnGraphCallback3() != 0;
}

/* dScMgTrampoline_c::OnGraphCallback0, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback0() {
    volatile u16 fill;
    Vec2s from;
    Vec2s to;
    s16 startX;
    int dst;

    if (!dScMgBase_c::OnGraphCallback0())
        return 0;

    if (gSceneFader->IsAtStart()) {
        sTrmpTouchBlocked = 0;
    } else if (gSceneFader->IsBetweenStartAndEnd()) {
        sTrmpTouchBlocked = 1;
    }

    if (sTrmpTouchBlocked != 0)
        return 0;

    if (mMenuOpen != 0)
        return 0;

    if (mTouchReleased != 0) {
        startX = mTouchStartX;
        if ((startX < 0x24 && mTouchX < 0x24) || (startX > 0xd4 && mTouchX > 0xd4)) {
            func_02012790(0xe);
        } else {
            /* Kept volatile: a plain read of mTouchStartX DIFFs here
               (measured 2026-11-08). */
            from.x = *(volatile s16 *)&mTouchStartX;
            from.y = mTouchStartY;
            to.x = mTouchX;
            to.y = mTouchY;
            if (TrmpLine_Create(&from.x, &to.x)) {
                func_02012718(0x1af, (mTouchStartX + mTouchX) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        dst = func_02054d88();
        fill = 0;
        MultiStore16(fill, (char *)dst, 0x6000);
        mTouching = 0;
        mTouchReleased = 0;
    }
    return 1;
}

void dScMgTrampoline_c::InitBrush(int layer) {
    volatile unsigned short t0;
    volatile unsigned short t1;
    int tile = 0x7000;
    char *scrPtr;
    char *charPtr;
    int i;

    mBrushOnMain = 1;
    mBrushLayer = layer;

    switch (mBrushLayer) {
    case 0:
        scrPtr = (char *)G2::GetBG0ScrPtr();
        charPtr = (char *)func_02054efc();
        break;
    case 1:
        scrPtr = (char *)G2::GetBG1ScrPtr();
        charPtr = (char *)func_02054ea8();
        break;
    case 2:
        scrPtr = (char *)G2::GetBG2ScrPtr();
        charPtr = (char *)G2::GetBG2CharPtr();
        break;
    case 3:
        scrPtr = (char *)G2::GetBG3ScrPtr();
        charPtr = (char *)func_02054d88();
        break;
    default:
        return;
    }

    for (i = 0; i < 0x300; i++) {
        t0 = tile;
        MultiStore16(t0, scrPtr, 2);
        tile = (unsigned short)(tile + 1);
        scrPtr += 2;
    }

    t1 = 0;
    MultiStore16(t1, charPtr, 0x6000);
    GX::LoadBGPltt(sTrmpBrushPltt, 0xe0, 0x20);
}

/* Kept for PaintSquare: see the file comment above for why it reaches the
   whole TU instead of just that function. */
#pragma opt_loop_invariants off

/* Slot 34, the brush stamp. dScMgD3DBase_c does not override this slot,
   so each trampoline class has its own; they differ only in the shape
   table (sTrmpBrushWord here, _02142f78 in dScMgTrampoline2_c).
   bgLayer (+0x6c, in the base padding) picks the character base. No
   wrapped-region branch: a 3D minigame owns the top screen. */
void dScMgTrampoline_c::PaintSquare(int cx, int cy, int colour, int size) {
    int half;
    int left;
    int j;
    int x;
    int *addr;
    int i;
    int y;

    half = size / 2;
    j = 0;
    if (j < size) {
        y = cy - half;
        left = cx - half;
        do {
            for (i = 0; i < size; i++) {
                x = i + left;
                if (x >= 0 && x < 0x100) {
                    if (y >= 0 && y < 0xc0) {
                        void *charBase;
                        switch (mBrushLayer) {
                        case 0:
                            charBase = func_02054efc();
                            break;
                        case 1:
                            charBase = func_02054ea8();
                            break;
                        case 2:
                            charBase = G2::GetBG2CharPtr();
                            break;
                        case 3:
                            charBase = (void *)func_02054d88();
                            break;
                        default:
                            return;
                        }

                        addr =
                            (int *)((char *)charBase + ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);

                        MultiCopy_Int(addr, sTrmpBrushWord, 4);
                        /* Kept as `-1 ^`: the `~` spelling DIFFs here (measured 2026-11-08). */
                        sTrmpBrushWord[0] = (sTrmpBrushWord[0] & (-1 ^ (0xf << ((x & 7) * 4)))) |
                                            (colour << ((x & 7) * 4));
                        MultiCopy_Int(sTrmpBrushWord, addr, 4);
                    }
                }
            }
            y++;
            j++;
        } while (j < size);
    }
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_InitPool(void *records, int count) {
    sTrmpFxPool = (TrmpScorePopup *)records;
    sTrmpFxCount = count;
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_Spawn(int x, int y) {
    int i;
    for (i = sTrmpFxCount - 1; i >= 0; i--) {
        if (sTrmpFxPool[i].active == 0) {
            sTrmpFxPool[i].Spawn(x, y);
            return;
        }
    }
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_ClearAll(void) {
    int i;
    for (i = 0; i < sTrmpFxCount; i++) {
        sTrmpFxPool[i].active = 0;
    }
    sTrmpFxList = 0;
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_UpdateAll(void) {
    int i;
    for (i = 0; i < sTrmpFxCount; i++) {
        sTrmpFxPool[i].Update();
    }
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_DrawAll(void) {
    TrmpScorePopup *node = sTrmpFxList;
    if (node == 0)
        return;
    do {
        node->Render();
        node = node->next;
    } while (node != 0);
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_Link(TrmpScorePopup *entry) {
    TrmpScorePopup *node;
    entry->next = sTrmpFxList;
    sTrmpFxList = entry;
    node = entry->next;
    int targetY = entry->y - 0x10000;
    while (node) {
        node->targetY = targetY;
        node = node->next;
        targetY -= 0x10000;
    }
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void TrmpFx_Unlink(TrmpScorePopup *node) {
    TrmpScorePopup *prev = sTrmpFxList;
    if (prev == node) {
        sTrmpFxList = node->next;
        return;
    }
    TrmpScorePopup *cur = prev->next;
    while (cur) {
        if (cur == node) {
            prev->next = node->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
}

/* Fills a slot with fixed parameters and tail-calls TrmpFx_Link. */
void TrmpScorePopup::Spawn(int y, int number) {
    x = 0x110000;
    this->y = y;
    speedX = -0x4000;
    speedY = 0;
    targetX = 0xc0000;
    targetY = y;
    life = 0x80;
    active = 1;
    this->number = number;
    TrmpFx_Link(this);
}

extern "C" void func_0203d630(int *p, int m);
void TrmpScorePopup::Update() {
    if (active == 0) {
        return;
    }
    if (ApproachLinear2(life, 0, 1)) {
        TrmpFx_Unlink(this);
        active = 0;
    }
    x += speedX;
    ApproachLinear(y, targetY, 0x1800);
    func_0203d630(&speedX, 0xf00);
}

void TrmpScorePopup::Render() {
    if (active == 0)
        return;
    Mg_DrawBigNumber(x >> 12, y >> 12, number, -1, -1, 0, 0);
}

void dScMgTrampoline_DoorMark::Init() {
    active = 0;
    shown = 0;
}

void dScMgTrampoline_DoorMark::Hide() {
    active = 0;
    shown = 0;
}

void dScMgTrampoline_DoorMark::Show(int side) {
    row = (s16)side;
    timer = 0x3c;
    shown = 1;
    active = 1;
    Sound::PlayBank2_2D(0x1B7);
}

void dScMgTrampoline_DoorMark::Update() {
    if (active == 0)
        return;
    if (ApproachLinear2(timer, 0, 1) != 0) {
        active = 0;
    }
    if (((timer / 10) & 1) != 0) {
        shown = 0;
    } else {
        shown = 1;
    }
}

void dScMgTrampoline_DoorMark::Render(int scrollY) {
    if (active == 0)
        return;
    if (shown == 0)
        return;
    Mg_RenderSpriteBoth((void *)sTrmpRowMarkerSprite, 0xd0, sTrmpRowScreenY[row] - scrollY - 8, -1,
                        -1);
}

TrmpScorePopup::~TrmpScorePopup() {}
