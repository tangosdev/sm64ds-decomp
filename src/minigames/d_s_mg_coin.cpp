//cpp
/**
 * Coincentration. Twenty-four coins fall into the blocks. A coin
 * keeps bouncing until it has bounced as many times as it is worth,
 * then sits and pops that value. Sparkles burst off it. The moneybag
 * hops down six stages and drops off the bottom. Phase 5 is the
 * result: a countdown, then this score against the other player's.
 *
 * This file is the whole ov006 unit 0x020dbe40..0x020de988: 63
 * functions in ROM order, from the destructor through InitResources to
 * the registry factory dScMgCoin_c_classInit. The destructor is the key
 * function, so the vtable and RTTI are emitted here too.
 *
 * The functions from Coin_StateWaitTouch up, and the three below
 * Bag_StateBounceDown, came from one-function files. Their bodies are
 * kept as they matched there. Where two of them spelled a type or a
 * declaration differently, the tag is renamed per function and the
 * call casts. Block_StateWander and SpawnBlocks called
 * slot 35 through a stand-in vtable; they now call IsAltVariant.
 *
 * The five state tables (the coin handlers at sCoinCoinStateTable, the
 * moneybag's at 021417c8, the caption's at 021417e8, the phases' at
 * 02141810 and the blocks' at 02141840) are {pmf, delta} records on
 * dScMgCoin_c. They are defined here, and the TU's static initialiser
 * fills them from the pointer-to-member constants at data_ov006_0213be**.
 *
 */

// Every helper reads the scene through plain members. The caption and the
// moneybag are one-element arrays: their state handlers take an index like
// the coin and block handlers do, and Caption_Update and Bag_Update pass 0.
// ResetRound also clears 0x4d00, 0x4d04, 0x51d0 and 0x51dd, which nothing in
// this unit reads; those stores stay raw. RenderOamMainScreen and
// Mg_RenderSpriteBothMtx are redeclared extern "C" at the call -- the ROM
// symbols are unmangled and the sites disagree on the types. DrawScoreRival
// reads the other score from gMgScene->mCreditsShown. GetGameLanguage is
// called twice in DrawScorePlayer and again in DrawScoreRival (hoisting
// misses).

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgCoin_c_AssetId {
    ASSET_D_2D_MG_BG_WRECKING_CREW_DOWN_NSC_BIN = 0x00a8,
    ASSET_D_2D_MG_BG_WRECKING_CREW_NCG_BIN = 0x00a9,
    ASSET_D_2D_MG_BG_WRECKING_CREW_NCL_BIN = 0x00aa,
    ASSET_D_2D_MG_BG_WRECKING_CREW_UP_NSC_BIN = 0x00ab,
    ASSET_D_2D_MG_WRECKING_CREW_NCG_BIN = 0x010a,
    ASSET_D_2D_MG_WRECKING_CREW_NCL_BIN = 0x010b
};

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "Sound.h"
#include "dScMgCoin_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/gx_load.h"

#pragma defer_codegen off /* ROM-ascending emission; dropping this inverts it */

enum {
    kCoinCount = 24,
    kSparkleCount = 32,
    kPopupCount = 24,
    kResultPhase = 5,
    kBouncerDone = 6,
    kCoinSettled = 2,
    kGravity = 0x400,
    kFallStep = 0x100,
    kHopVy = -0x3000,
    kKickVy = -0x4000,
    kParkedY = -0xf0000,
    kParkRow = -0xf0,
    kOffBottom = 0xd0,
    kSpinStep = 0x400,
    kSparkleFall = 0x180,
    kSparkleDrag = 0x180,
    kSparkleCap = 0x300,
    kCaptionHold = 0x3c,
    kCountInDelay = 0xb4,
    kStartDelay = 0x28,
    kChimeAt = 0x18,
    kBagRestX = 0x8e000,
    kBagRestY = -0x43000,
    kCaptionX = 0x70000,
    kCaptionY = 0x98000,
    kSndBounce = 0xef,
    kSndReady = 0xf0,
    kSndChime = 0xf1,
    kTileRows = 3,
    kTileCols = 0x10,
    kTileStep = 0x20
};

extern "C" {

extern const int sCoinBagRowTopY[];
extern const int sCoinBagRowHeight[];
extern const int sCoinBagHopVX[];
extern OamAttr *sCoinCaptionSprites[];
extern const unsigned char sCoinCaptionLoopTimes[];
extern const unsigned char sCoinCaptionLoopSprites[];
extern const unsigned char sCoinCaptionBlinkTimes[];
extern const unsigned char sCoinCaptionBlinkSprites[];
extern const u16 sCoinCaptionCountInTimes[];
extern const u8 sCoinCaptionCountInSprites[];
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern const int sCoinSparkleSpawnOffsets[];
extern const u16 sCoinSparkleAngles[];
extern int sCoinPopupSprites;
extern const int sCoinPopupY[];
extern OamAttr **sCoinScorePlayerSprites[];
extern int GetGameLanguage(void);
extern void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3);
struct OamAttr;
struct Matrix2x2;
extern void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern void *sCoinValuesSprites[];
extern void *sCoinSparkleSprites[];
extern void *sCoinBagSprites[];
extern void Mg_RenderSpriteBothPalMtx(void *obj, int x, int y, int w, int *vec);
}

extern void func_020127a4(int a, int b, int c, int d);
extern "C" {

extern const int sCoinRowY[];
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void RenderOamMainScreen(OamAttr *a, int b, int c, int d, int e);
extern const u16 sCoinBlockFrameSprites[];
extern void *sCoinBlocksSprites[];
extern const int sCoinBlockFallRowY[];
extern const int sCoinBlockDropRowY[];
extern const int sCoinBlockWanderRowY[];
extern const int sCoinBlockWanderRowHeight[];
extern const int sCoinBlockHopRowY[];
extern const int sCoinBlockHopRowHeight[];
extern const int sCoinBlockHopVX[];
extern const u16 sCoinBlockFrameTimes[];
extern void FreeGfxSlotsById(int n);
extern void Mg_PlayJingle(void *c);
extern int Mg_GetTopRecord(void);
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);

extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern int data_0208ee44;
extern void func_0203b9b4(int *p, int v);
}


/* The per-state handler tables: {pmf, delta} records indexed by the
   record's own state byte. Static initialisation fills each one from the
   pointer-to-member constants at 0x0213be44..0x0213bf0c in .data, which
   stay outside this TU's claim, so they are reached by extern. The
   definitions are in the order the initialiser fills them. */
typedef void (dScMgCoin_c::*CoinPmf)(int);
struct CoinPmfEntry {
    CoinPmf pmf;
};
typedef void (dScMgCoin_c::*PMF_Behavior)();
struct Entry_Behavior {
    PMF_Behavior pmf;
};
extern "C" {

extern const Entry_Behavior pmf_dScMgCoin_c_Phase_StateIdle, pmf_dScMgCoin_c_Phase_StateIntro,
    pmf_dScMgCoin_c_Phase_StatePlay, pmf_dScMgCoin_c_Phase_StateSettle,
    pmf_dScMgCoin_c_Phase_StateScoreDelay, pmf_dScMgCoin_c_Phase_StateResult;
extern const CoinPmfEntry pmf_dScMgCoin_c_Block_StateWaiting, pmf_dScMgCoin_c_Block_StateHopDown,
    pmf_dScMgCoin_c_Block_StateWander, pmf_dScMgCoin_c_Block_StateDropOnCoin,
    pmf_dScMgCoin_c_Block_StateResting, pmf_dScMgCoin_c_Block_StateFallIn;
extern const CoinPmfEntry pmf_dScMgCoin_c_Coin_StateWaitTouch, pmf_dScMgCoin_c_Coin_StateHop,
    pmf_dScMgCoin_c_Coin_StateSettled;
extern const CoinPmfEntry pmf_dScMgCoin_c_Caption_StateDelay, pmf_dScMgCoin_c_Caption_StateCountIn,
    pmf_dScMgCoin_c_Caption_StateBlink, pmf_dScMgCoin_c_Caption_StateHidden,
    pmf_dScMgCoin_c_Caption_StateLoop;
extern const CoinPmfEntry pmf_dScMgCoin_c_Bag_StateIdle, pmf_dScMgCoin_c_Bag_StateRise,
    pmf_dScMgCoin_c_Bag_StateParked, pmf_dScMgCoin_c_Bag_StateBounceDown;

/* the phases (Behavior) */
Entry_Behavior sCoinPhaseStateTable[6] = {
    {pmf_dScMgCoin_c_Phase_StateIdle},       {pmf_dScMgCoin_c_Phase_StateIntro},
    {pmf_dScMgCoin_c_Phase_StatePlay},       {pmf_dScMgCoin_c_Phase_StateSettle},
    {pmf_dScMgCoin_c_Phase_StateScoreDelay}, {pmf_dScMgCoin_c_Phase_StateResult}};
/* the blocks */
CoinPmfEntry sCoinBlockStateTable[6] = {
    {pmf_dScMgCoin_c_Block_StateWaiting}, {pmf_dScMgCoin_c_Block_StateHopDown},
    {pmf_dScMgCoin_c_Block_StateWander},  {pmf_dScMgCoin_c_Block_StateDropOnCoin},
    {pmf_dScMgCoin_c_Block_StateResting}, {pmf_dScMgCoin_c_Block_StateFallIn}};
/* the coins */
CoinPmfEntry sCoinCoinStateTable[3] = {{pmf_dScMgCoin_c_Coin_StateWaitTouch},
                                       {pmf_dScMgCoin_c_Coin_StateHop},
                                       {pmf_dScMgCoin_c_Coin_StateSettled}};
/* the caption */
CoinPmfEntry sCoinCaptionStateTable[5] = {{pmf_dScMgCoin_c_Caption_StateDelay},
                                          {pmf_dScMgCoin_c_Caption_StateCountIn},
                                          {pmf_dScMgCoin_c_Caption_StateBlink},
                                          {pmf_dScMgCoin_c_Caption_StateHidden},
                                          {pmf_dScMgCoin_c_Caption_StateLoop}};
/* the moneybag */
CoinPmfEntry sCoinBagStateTable[4] = {{pmf_dScMgCoin_c_Bag_StateIdle},
                                      {pmf_dScMgCoin_c_Bag_StateRise},
                                      {pmf_dScMgCoin_c_Bag_StateParked},
                                      {pmf_dScMgCoin_c_Bag_StateBounceDown}};
}


/* Nothing to destroy by hand: D1 stores the vptr and calls dScMgBase_c's
   D2, and D0 adds the delete. */
dScMgCoin_c::~dScMgCoin_c() {}

/* The hand cursor while the stylus is down: the sine table entry for the
   scene's angle becomes a unit-scale 2x2 rotation matrix [cos, sin; -sin,
   cos] for the shared sprite call. The four matrix words are written
   straight into vec[], in slot order, after the icon index is read. A named
   rx/ry pair leaves the two 64-bit products in the wrong order or the sine
   base in the wrong register, and reading the icon index any later costs
   fifteen words. The pragma is part of that match. The table stays
   hand-indexed here: through FX_CosIdx / FX_SinIdx the two products take
   swapped registers (21 DIFF in every declaration order of vec, the icon
   index and fx16 or int locals, with or without the pragma). */
#pragma push
#pragma opt_propagation off
void dScMgCoin_c::Bag_Draw() {

    if (mBouncer[0].shown == 0)
        return;
    {
        u16 idx_h = mBouncer[0].spin;
        s32 xr = mBouncer[0].x;
        s32 yr = mBouncer[0].y;
        /* The table reads stay hand-indexed: through FX_SinIdx/FX_CosIdx
           (nitro/fx/fx_trig.h), with or without an index local, the
           function compiles to a different body. */
        int i = (idx_h >> 4) * 2;
        int vec[4];
        u8 idx_l = mBouncer[0].sprite;

        vec[0] = FX_MUL(FX_SinCosTable_[i + 1], 0x1000);
        vec[1] = FX_MUL(FX_SinCosTable_[i], 0x1000);
        vec[2] = -FX_MUL(FX_SinCosTable_[i], 0x1000);
        vec[3] = FX_MUL(FX_SinCosTable_[i + 1], 0x1000);
        Mg_RenderSpriteBothPalMtx(sCoinBagSprites[idx_l], xr >> 12, yr >> 12, -1, vec);
    }
}
#pragma pop

void dScMgCoin_c::Bag_StateBounceDown(int i) {
    int height;
    int stageIndex;

    mBouncer[i].x += mBouncer[i].vx;
    mBouncer[i].y += mBouncer[i].vy;
    mBouncer[i].vy += kGravity;
    if (mBouncer[i].vx > 0)
        mBouncer[i].spin += kSpinStep;
    else
        mBouncer[i].spin -= kSpinStep;

    height = mBouncer[i].y >> 12;
    stageIndex = mBouncer[i].stage;

    if (stageIndex >= kBouncerDone) {
        if (height >= kOffBottom) {
            mBouncer[i].shown = 0;
            mBouncer[i].active = 0;
        }
        return;
    }

    if (mBouncer[i].bouncesLeft != 0) {
        if (height < (sCoinBagRowTopY[stageIndex] - sCoinBagRowHeight[stageIndex]))
            return;
        mBouncer[i].vx = sCoinBagHopVX[MG_RAND(2)];
        mBouncer[i].vy = kHopVy;
        mBouncer[i].bouncesLeft--;
        func_02012718(kSndBounce, mBouncer[i].x);
        if (mBouncer[i].bouncesLeft == 0)
            mBouncer[i].stage++;
    } else {
        mBouncer[i].bouncesLeft = 1;
    }
}

void dScMgCoin_c::Bag_StateParked(int index) {
    if (mBouncer[index].countdown != 0) {
        mBouncer[index].countdown--;
        return;
    }
    mBouncer[index].state = 3;
    mBouncer[index].vy = 0;
    mBouncer[index].bouncesLeft = 1;
    mBouncer[index].stage = 0;
    this->SpawnBlocks();
}

void dScMgCoin_c::Bag_StateRise(int index) {
    mBouncer[index].y += mBouncer[index].vy;
    mBouncer[index].vy -= kFallStep;
    mBouncer[index].spin += kSpinStep;
    if ((mBouncer[index].y >> 12) > kParkRow)
        return;
    mBouncer[index].y = kParkedY;
    mBouncer[index].state = 2;
    mBouncer[index].countdown = 0x30;
    this->SpawnBlocks();
}

void dScMgCoin_c::Bag_Release() {
    mBouncer[0].state = 1;
    mBouncer[0].sprite = 1;
    mBouncer[0].vx = 0;
    mBouncer[0].vy = kKickVy;
}

void dScMgCoin_c::Bag_StateIdle() {}

/* Dispatch the moneybag while mBouncer[0].active is set. Not a vtable:
   the records live in sCoinBagStateTable. */
void dScMgCoin_c::Bag_Update() {
    if (this->mBouncer[0].active == 0)
        return;

    int state = this->mBouncer[0].state;
    (this->*sCoinBagStateTable[state].pmf)(0);
}

void dScMgCoin_c::Bag_Show() {
    mBouncer[0].active = 1;
    mBouncer[0].shown = 1;
    mBouncer[0].sprite = 0;
    mBouncer[0].state = 0;
    mBouncer[0].spin = 0;
    mBouncer[0].x = kBagRestX;
    mBouncer[0].y = kBagRestY;
}

void dScMgCoin_c::Bag_Hide() {
    mBouncer[0].active = 0;
    mBouncer[0].shown = 0;
}

void dScMgCoin_c::Caption_Finish() {
    mCaption[0].state = 4;
    mCaption[0].frame = 0;
    mCaption[0].frameTime = 0;
    mCaptionLatch = 1;
}

void dScMgCoin_c::Caption_Trigger() {
    if (mCaptionLatch != 0)
        return;

    if (mCaption[0].state == 2) {
        mCaption[0].delay = kCaptionHold;
        return;
    }

    mCaption[0].delay = kCaptionHold;
    mCaption[0].state = 2;
    mCaption[0].frame = 0;
    mCaption[0].frameTime = 0;
}

void dScMgCoin_c::Caption_Draw() {
    if (mCaption[0].visible == 0)
        return;

    RenderOamMainScreen(sCoinCaptionSprites[mCaption[0].sprite], mCaption[0].x >> 12,
                        mCaption[0].y >> 12, -1, -1);
}

void dScMgCoin_c::Caption_StateLoop(int index) {
    mCaption[index].frameTime++;
    if (mCaption[index].frameTime < sCoinCaptionLoopTimes[mCaption[index].frame])
        return;

    mCaption[index].frameTime = 0;
    mCaption[index].frame++;
    if (mCaption[index].frame >= 8)
        mCaption[index].frame = 1;
    else
        mCaption[index].sprite = sCoinCaptionLoopSprites[mCaption[index].frame];
}

void dScMgCoin_c::Caption_StateHidden(int index) {
    mCaption[index].sprite = 0;
}

void dScMgCoin_c::Caption_StateBlink(int index) {
    mCaption[index].frameTime++;
    if (mCaption[index].frameTime >= sCoinCaptionBlinkTimes[mCaption[index].frame]) {
        mCaption[index].frameTime = 0;
        mCaption[index].frame++;
        mCaption[index].frame &= 1;
    }
    mCaption[index].sprite = sCoinCaptionBlinkSprites[mCaption[index].frame];
    if (mPhase == kResultPhase)
        return;
    if (mCaption[index].delay != 0) {
        mCaption[index].delay--;
        return;
    }
    mCaption[index].state = 3;
    mCaption[index].frame = 0;
    mCaption[index].frameTime = 0;
    mCaption[index].delay = 0;
}

void dScMgCoin_c::Caption_StateCountIn(int index) {
    mCaption[index].frameTime++;
    if (mCaption[index].frameTime < sCoinCaptionCountInTimes[mCaption[index].frame])
        return;
    mCaption[index].frameTime = 0;
    mCaption[index].frame++;
    if (mCaption[index].frame == 2) {
        Bag_Release();
        Sound::PlayBank2_2D(kSndReady);
    }
    if (mCaption[index].frame >= 4) {
        mCaption[index].delay = kCountInDelay;
        mCaption[index].state = 2;
        mCaption[index].frame = 0;
        mCaption[index].frameTime = 0;
        mPhase++;
    } else {
        mCaption[index].sprite = sCoinCaptionCountInSprites[mCaption[index].frame];
    }
}

void dScMgCoin_c::Caption_StateDelay(int index) {
    if (mCaption[index].delay != 0) {
        mCaption[index].delay--;
        if (mCaption[index].delay == kChimeAt)
            Sound::PlayBank2_2D(kSndChime);
        return;
    }
    mCaption[index].state = 1;
    mCaption[index].frameTime = 0;
}

/* Same dispatch as the moneybag, for the caption. */
void dScMgCoin_c::Caption_Update() {
    if (this->mCaption[0].active == 0)
        return;

    int state = this->mCaption[0].state;
    (this->*sCoinCaptionStateTable[state].pmf)(0);
}

void dScMgCoin_c::Caption_Show() {
    mCaption[0].active = 1;
    mCaption[0].state = 0;
    mCaption[0].visible = 1;
    mCaption[0].delay = kStartDelay;
    mCaption[0].frameTime = 0;
    mCaption[0].sprite = 3;
    mCaption[0].frame = 0;
    mCaption[0].x = kCaptionX;
    mCaption[0].y = kCaptionY;
}

void dScMgCoin_c::Caption_Reset() {
    mCaption[0].active = 0;
    mCaption[0].visible = 1;
}

void dScMgCoin_c::DrawResultPrompt() {
    if (mPhase != kResultPhase)
        return;
    if (mCountdown > 0x80)
        return;
    if (mCountdown == 0)
        return;

    Mg_DrawBlinkSprite(this, 0xe0, 0xa0);
}

void dScMgCoin_c::DrawResultNumbers() {
    int i;
    if (mPhase == kResultPhase) {
        if (mCountdown == 0)
            return;
    }
    for (i = 0; i < kPopupCount; i++) {
        if (mPopups[i].flag != 0) {
            Mg_DrawBigNumber(mPopups[i].x >> 12, mPopups[i].y >> 12, mPopups[i].value, 0, -1, 0, 0);
        }
    }
}

void dScMgCoin_c::Popup_UpdateAll() {
    int i;
    for (i = 0; i < kPopupCount; i++) {
        if (mPopups[i].live != 0 && mPopups[i].timer != 0) {
            mPopups[i].timer--;
            if ((s16)mPopups[i].timer <= 0)
                mPopups[i].flag = 1;
        }
    }
}

void dScMgCoin_c::Popup_Spawn(int index) {
    mPopups[index].x = mCoins[index].x;
    mPopups[index].y = mCoins[index].y;
    mPopups[index].value = mCoins[index].value;
    mPopups[index].live = 1;
    mPopups[index].timer = 0x18;
}

void dScMgCoin_c::Sparkle_DrawAll() {
    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (mSparkles[i].visible != 0) {
            int sprite = mSparkles[i].sprite;
            int x = mSparkles[i].x;
            int y = mSparkles[i].y;
            Mg_RenderSpriteBothMtx(sCoinSparkleSprites[sprite], x >> 12, y >> 12, 0);
        }
    }
}

void dScMgCoin_c::Sparkle_UpdateAll() {
    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (mSparkles[i].active != 0) {
            if (mSparkles[i].life != 0) {
                mSparkles[i].life -= 1;
            } else {
                mSparkles[i].active = 0;
                mSparkles[i].visible = 0;
                return;
            }
            mSparkles[i].x += mSparkles[i].vx;
            mSparkles[i].y += mSparkles[i].vy;
            mSparkles[i].vy += kSparkleFall;
            if (mSparkles[i].vx > kSparkleCap)
                mSparkles[i].vx -= kSparkleDrag;
            else if (mSparkles[i].vx < -kSparkleCap)
                mSparkles[i].vx += kSparkleDrag;
        }
    }
}

void dScMgCoin_c::Sparkle_Burst(int coinIndex) {
    int i;
    for (i = 0; i < 4; i++) {
        if (mSparkles[i].active == 0) {
            mSparkles[i].active = 1;
            mSparkles[i].visible = 1;
            mSparkles[i].x = mCoins[coinIndex].x + (sCoinSparkleSpawnOffsets[i * 2] << 12);
            mSparkles[i].y = mCoins[coinIndex].y + (sCoinSparkleSpawnOffsets[i * 2 + 1] << 12);
            {
                mSparkles[i].vx = FX_MUL(FX_CosIdx(sCoinSparkleAngles[i] >> 4), 0x2000);
                mSparkles[i].vy = FX_MUL(FX_SinIdx(sCoinSparkleAngles[i] >> 4), 0x2000);
            }
            mSparkles[i].life = 0x18;
        }
    }
}

void dScMgCoin_c::Popup_DrawAll() {
    int i;
    int x, y, j;
    for (i = 0; i < kTileRows; i++) {
        y = sCoinPopupY[i];
        x = 0x10;
        for (j = 0; j < kTileCols; j++) {
            Mg_RenderSpriteBothMtx((void *)sCoinPopupSprites, x, y, (void *)0);
            x += kTileStep;
        }
    }
}

void dScMgCoin_c::DrawScorePlayer() {
    int lang;
    if (mPhase < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(sCoinScorePlayerSprites[lang][8], 0x80, 0x18, -1, -1);
    Mg_DrawCreditsIcon(0x68, 0x28, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite(sCoinScorePlayerSprites[lang][1], 0x7a, 0x28, 0);
    Mg_DrawBigNumber(0x8c, 0x28, mPlayerScore, 1, -1, 2, 0);
}

void dScMgCoin_c::DrawScoreRival() {
    int lang;
    int count;
    if (mPhase < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(sCoinScorePlayerSprites[lang][4], 0x80, 0x50, -1, -1);
    Mg_DrawCreditsIcon(0x6c, 0x60, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite(sCoinScorePlayerSprites[lang][1], 0x7e, 0x60, 0);
    if (gMgScene != 0)
        count = gMgScene->mCreditsShown;
    else
        count = 0;
    Mg_DrawBigNumber(0x90, 0x60, count, 1, -1, 2, 0);
}

void dScMgCoin_c::Score_Update() {
    if (mScore.running == 0)
        return;
    if (mScore.total == mScore.shown)
        return;
    mScore.tick++;
    if (mScore.tick < 8)
        return;
    mScore.tick = 0;
    mScore.shown++;
}

void dScMgCoin_c::DrawCoinValues() {
    int i;
    int v1, v2;
    if (mPhase == kResultPhase && mCountdown == 0)
        return;

    v1 = 0;
    v2 = 0;
    for (i = 0; i < kCoinCount; i++) {
        int st = mCoins[i].state;
        int sprite = (st == kCoinSettled) ? 1 : v1;
        int x = mCoins[i].x >> 12;
        int y = mCoins[i].y >> 12;
        int highlight = (mPhase == kResultPhase) ? 1 : v2;
        if (st == kCoinSettled)
            highlight = 1;
        if (mPhase != kResultPhase) {
            if (mCoins[i].visible != 0)
                Mg_RenderSpriteBoth(sCoinValuesSprites[sprite], x, y, -1, highlight);
        } else {
            if (sprite != 0 && mCoins[i].value != 0)
                Mg_RenderSpriteBoth(sCoinValuesSprites[sprite], x, y, -1, highlight);
            if (mCoins[i].collected == 0 && mCoins[i].value != 0)
                Mg_DrawBigNumber(x, y, mCoins[i].value, 0, -1, 0, 0);
        }
    }
}

void dScMgCoin_c::Coin_StateSettled() {}

void dScMgCoin_c::Coin_StateHop(int index) {
    mCoins[index].y += mCoins[index].vy;
    mCoins[index].vy += kGravity;
    if (mCoins[index].y < mCoins[index].landY)
        return;
    mCoins[index].y = mCoins[index].landY;
    mCoins[index].bounces = mCoins[index].bounces + 1;
    this->Caption_Trigger();
    if (mCoins[index].value == mCoins[index].bounces) {
        mCoins[index].state = kCoinSettled;
        this->Popup_Spawn(index);
        return;
    }
    mCoins[index].vy = kHopVy;
    this->KickBlockUnderCoin(index);
}

/* The touch test for one coin. In phase 2, with the stylus down and newly
   pressed, a touch within 0x10 of the coin takes it: a coin still worth
   something starts its hop, otherwise the round ends (phase 3, a 0x40 timer,
   the panned reveal jingle, and the win flag from the other score). */
void dScMgCoin_c::Coin_StateWaitTouch(int idx) {
    int touched;
    int dx, dy;
    int pan;

    if (mPhase != 2)
        return;

    int slot = gActivePlayerSlot;
    touched = 0;
    if (gTouchHeld[slot * 4] != 0 && gTouchEdge[slot * 4] != 0)
        touched = 1;
    if (touched == 0)
        return;

    dx = gTouchX[gActivePlayerSlot * 4] - (mCoins[idx].x >> 12);
    dy = gTouchY[gActivePlayerSlot * 4] - (mCoins[idx].y >> 12);
    if (dx <= -0x10 || dx >= 0x10 || dy <= -0x10 || dy >= 0x10)
        return;

    mCoins[idx].collected = 1;
    if (mCoins[idx].value != 0) {
        mCoins[idx].state = 1;
        mCoins[idx].landY = mCoins[idx].y;
        mCoins[idx].vy = kHopVy;
        mCoins[idx].bounces = 0;
        KickBlockUnderCoin(idx);
        return;
    }

    Sparkle_Burst(idx);
    mCoins[idx].visible = 0;
    mPhase = 3;
    mCountdown = 0x40;

    pan = ((mCoins[idx].x >> 12) - 0x80) >> 1;
    if (pan >= 0x3c)
        pan = 0x3c;
    if (pan <= -0x3c)
        pan = -0x3c;
    func_020127a4(2, 0xee, 0xffff, pan);
    Sound::PlayBank2_2D(0xf2);

    mCoins[idx].state = 2;
    if (((gMgScene != 0) ? gMgScene->mCredits : 0) > mPlayerScore)
        mWonLast = 1;
    else
        mWonLast = 0;
}

/* Runs each coin's state handler from sCoinCoinStateTable. */
void dScMgCoin_c::Coin_UpdateAll() {
    int i;
    for (i = 0; i < 0x18; i++) {
        unsigned char k = this->mCoins[i].state;
        (this->*sCoinCoinStateTable[k].pmf)(i);
    }
}

/* Lays the 24 coins out in three rows of eight and marks two coins in each
   row, picked at random, as keepEmpty. */
void dScMgCoin_c::LayOutCoins() {
    int a[3];
    int b[3];
    int k;
    int col;
    int row;
    int i;

    for (k = 0; k < 3; k++) {
        a[k] = MG_RAND(8);
    }

    for (k = 0; k < 3; k++) {
        b[k] = (a[k] + MG_RAND(6) + 1) & 7;
    }

    row = 0;
    col = 0;
    for (i = 0; i < 0x18; i++) {
        mCoins[i].x = (((col << 5) + 0x10) << 12);
        mCoins[i].y = sCoinRowY[row] << 12;
        col++;
        mCoins[i].placed = 1;
        mCoins[i].visible = 1;
        if (col >= 8) {
            col = 0;
            row++;
        }
    }

    mCoins[a[0]].keepEmpty = 1;
    mCoins[b[0]].keepEmpty = 1;
    mCoins[a[1] + 8].keepEmpty = 1;
    mCoins[b[1] + 8].keepEmpty = 1;
    mCoins[a[2] + 0x10].keepEmpty = 1;
    mCoins[b[2] + 0x10].keepEmpty = 1;
}

/* Finds the first resting block (state 4) that belongs to coin id, kicks it
   up from under the coin (state 5) and plays the panned hop sound. */
void dScMgCoin_c::KickBlockUnderCoin(int id) {
    int i;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].active != 0 && mBlocks[i].state == 4 && id == mBlocks[i].coin) {
            mBlocks[i].y = mCoins[id].y - 0x20000;
            mBlocks[i].visible = 1;
            mBlocks[i].state = 5;
            mBlocks[i].vy = -0x4800;
            mBlocks[i].delay = 0;
            int pan = ((mCoins[id].x >> 12) - 0x80) >> 1;
            if (pan >= 0x3c)
                pan = 0x3c;
            if (pan <= -0x3c)
                pan = -0x3c;
            func_020127a4(2, 0xef, 0xffff, pan);
            return;
        }
    }
}

/* Draws the 40 blocks on both screens. */
void dScMgCoin_c::DrawBlocks() {
    int i;
    if (mPhase == 5 && mCountdown == 0)
        return;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].visible == 0)
            continue;
        {
            unsigned int state = mBlocks[i].state;
            int posX = mBlocks[i].x;
            int posY = mBlocks[i].y;
            int screenX, screenY, priority;
            unsigned short spriteIndex;
            screenX = posX >> 12;
            screenY = posY >> 12;
            priority = (state >= 3) ? 1 : 0;
            spriteIndex = sCoinBlockFrameSprites[mBlocks[i].animFrame];
            RenderOamBothScreens(sCoinBlocksSprites[spriteIndex], screenX, screenY, -1, priority,
                                 0);
        }
    }
}

/* A block falling into place. The panned landing sound plays on the frame
   vy turns from rising to falling; once it passes its row it rests (state
   4), bumps the running total and bursts an effect. */
struct Pair_dd658 {
    int a;
    int b;
};
extern "C" void Mg_SpawnFx(int a, int b, struct Pair_dd658 *p2, struct Pair_dd658 *p3,
                           struct Pair_dd658 *p4);

void dScMgCoin_c::Block_StateFallIn(int i) {
    int oldVy;
    struct Pair_dd658 pa;
    struct Pair_dd658 pb;

    oldVy = mBlocks[i].vy;
    mBlocks[i].y += oldVy;
    mBlocks[i].vy += 0x600;

    if (oldVy <= 0 && mBlocks[i].vy >= 0) {
        int v = ((mBlocks[i].x >> 12) - 0x80) >> 1;
        if (v >= 0x3c)
            v = 0x3c;
        if (v <= -0x3c)
            v = -0x3c;
        func_020127a4(2, 0xec, 0xffff, v);
    }

    {
        int y = mBlocks[i].y >> 12;
        u8 idx = mBlocks[i].row;
        if (mBlocks[i].vy < 0)
            return;
        if (y <= sCoinBlockFallRowY[idx] - 0x20)
            return;

        mBlocks[i].state = 4;
        mBlocks[i].visible = 0;
        mBlocks[i].active = 0;
        mScore.total++;

        {
            int v1 = mBlocks[i].y;
            int v0 = mBlocks[i].x;
            pb.a = 0x6c000;
            pb.b = -0x80000;
            pa.a = v0;
            pa.b = v1;
        }
        Mg_SpawnFx(1, 0x20, &pa, &pa, &pb);
    }
}

void dScMgCoin_c::Block_StateResting() {}

/* A block dropping onto its coin's row. It stops at the row height from
   sCoinBlockDropRowY, takes the coin's x and rests (state 4). */
void dScMgCoin_c::Block_StateDropOnCoin(int index) {
    mBlocks[index].y += mBlocks[index].vy;
    mBlocks[index].vy += 0x400;
    int rowY = sCoinBlockDropRowY[mBlocks[index].row];
    if ((mBlocks[index].y >> 12) <= rowY)
        return;
    mBlocks[index].x = mCoins[mBlocks[index].coin].x;
    mBlocks[index].y = rowY << 12;
    mBlocks[index].vy = 0;
    mBlocks[index].vx = 0;
    mBlocks[index].state = 4;
    mBlocks[index].visible = 0;
}

/* A block wandering along its row. When it reaches the row it picks a
   random direction, and over a coin of that row still worth at most two
   (five when IsAltVariant says so) it claims the coin, adds one to its value
   and starts to rise (state 3). */
void dScMgCoin_c::Block_StateWander(int i) {
    int row;
    int rowY;
    int first;
    int x;
    int j;

    mBlocks[i].x += mBlocks[i].vx;
    mBlocks[i].y += mBlocks[i].vy;
    mBlocks[i].vy += 0x400;
    Block_ClampToWalls(i);

    row = mBlocks[i].row;
    rowY = sCoinBlockWanderRowY[row] - sCoinBlockWanderRowHeight[row];
    if ((mBlocks[i].y >> 12) > rowY) {
        mBlocks[i].y = rowY * 0x1000;
        mBlocks[i].vy = 0;
        if (mBlocks[i].vx == 0) {
            if (MG_RAND(2))
                mBlocks[i].vx = 0xc00;
            else
                mBlocks[i].vx = -0xc00;
        }
    }
    {
        int y = mBlocks[i].y;
        if (y != (rowY << 12))
            return;
    }

    first = (row - 3) * 8;
    x = mBlocks[i].x >> 12;
    for (j = 0; j < 8; j++) {
        int d = (mCoins[first + j].x >> 12) - x;
        int limit = 2;
        if (IsAltVariant() != 0)
            limit = 5;
        if (d <= -6 || d >= 6)
            continue;
        if (mCoins[first + j].keepEmpty != 0)
            continue;
        if (mCoins[first + j].value > limit)
            continue;
        mCoins[first + j].value++;
        mBlocks[i].state = 3;
        mBlocks[i].vy = 0x200;
        mBlocks[i].coin = first + j;
        return;
    }
}

/* A block hopping down the stages. Each landing picks a new random vx and
   plays the panned hop; when its hops for a stage run out it moves down a
   stage, and past its own row it switches to state 2. */
void dScMgCoin_c::Block_StateHopDown(int i) {
    u8 hops;
    int y;
    u8 stage;

    mBlocks[i].x += mBlocks[i].vx;
    mBlocks[i].y += mBlocks[i].vy;
    mBlocks[i].vy += 0x400;
    Block_ClampToWalls(i);

    hops = mBlocks[i].hops;
    y = mBlocks[i].y >> 12;
    stage = mBlocks[i].stage;
    if (hops != 0) {
        int pan;

        if (y < sCoinBlockHopRowY[stage] - sCoinBlockHopRowHeight[stage])
            return;

        mBlocks[i].vx = sCoinBlockHopVX[MG_RAND(3)];
        mBlocks[i].vy = -0x2e00;
        mBlocks[i].y = (sCoinBlockHopRowY[stage] - sCoinBlockHopRowHeight[stage]) << 12;
        mBlocks[i].hops--;

        pan = ((mBlocks[i].x >> 12) - 0x80) >> 1;
        if (pan >= 0x3c)
            pan = 0x3c;
        if (pan <= -0x3c)
            pan = -0x3c;
        func_020127a4(2, 0x197, 0xffff, pan);

        if (mBlocks[i].hops == 0)
            mBlocks[i].stage++;
        if (mBlocks[i].stage > mBlocks[i].row)
            mBlocks[i].state = 2;
    } else {
        unsigned int r;
        mBlocks[i].hops = 1;
        r = MG_RAND(8);
        if (r == 2 || r == 5)
            mBlocks[i].hops++;
    }
}

/* A block waiting to drop: counts its delay down, then starts it (state 1). */
void dScMgCoin_c::Block_StateWaiting(int i) {
    if (mBlocks[i].delay != 0) {
        mBlocks[i].delay--;
        if ((s16)mBlocks[i].delay < 0)
            mBlocks[i].delay = 0;
        return;
    }
    mBlocks[i].state = 1;
    mBlocks[i].hops = 1;
}

/* Keeps a block between x 8 and 0xf8, reversing vx at either wall. */
void dScMgCoin_c::Block_ClampToWalls(int idx) {
    if ((mBlocks[idx].x >> 12) < 8) {
        mBlocks[idx].x = 0x8000;
        mBlocks[idx].vx = -mBlocks[idx].vx;
        return;
    }
    if ((mBlocks[idx].x >> 12) > 0xf8) {
        mBlocks[idx].x = 0xf8000;
        mBlocks[idx].vx = -mBlocks[idx].vx;
    }
}

/* Runs every live block's state handler and animation. When none is left
   moving the scene goes to phase 2, the score starts running and the start
   sound plays. */
void dScMgCoin_c::Block_UpdateAll() {
    int n = 0;
    int i;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].active != 0) {
            (this->*sCoinBlockStateTable[mBlocks[i].state].pmf)(i);
            if (mBlocks[i].state != 4)
                n++;
            Block_Animate(i);
        }
    }
    if (n != 0)
        return;
    mPhase = 2;
    mScore.running = 1;
    Sound::PlayBank2_2D(0x151);
}

/* Steps one block's four-frame animation; sCoinBlockFrameTimes holds each
   frame's length. */
void dScMgCoin_c::Block_Animate(int index) {
    mBlocks[index].animTimer++;
    if (mBlocks[index].animTimer < sCoinBlockFrameTimes[mBlocks[index].animFrame])
        return;
    mBlocks[index].animFrame++;
    mBlocks[index].animFrame &= 3;
    mBlocks[index].animTimer = 0;
}

/* Parks the blocks above the top of the screen at random x with staggered
   delays, cycling their rows 3, 4, 5. Sixteen when IsAltVariant says so,
   otherwise forty. */
void dScMgCoin_c::SpawnBlocks() {
    int n;
    u8 row = 0;
    int i;
    n = IsAltVariant() != 0 ? 0x10 : 0x28;
    for (i = 0; i < n; i++) {
        mBlocks[i].x = ((MG_RAND(8) << 5) + 0x10) << 12;
        mBlocks[i].y = -0xf0000;
        mBlocks[i].active = 1;
        mBlocks[i].visible = 1;
        mBlocks[i].delay = ((i & 0xf) << 3) + (MG_RAND(16) << 4);
        mBlocks[i].row = row + 3;
        row++;
        if (row >= 3)
            row = 0;
    }
}

/* Clears the blocks, coins, score, sparkles, popups and phase, then resets
   the caption and the moneybag. */
void dScMgCoin_c::ResetRound() {
    int i, j, k, l;
    for (i = 0; i < 0x28; i++) {
        mBlocks[i].x = 0;
        mBlocks[i].y = 0;
        mBlocks[i].vx = 0;
        mBlocks[i].vy = 0;
        mBlocks[i].delay = 0;
        mBlocks[i].animTimer = 0;
        mBlocks[i].row = 0;
        mBlocks[i].state = 0;
        mBlocks[i].visible = 0;
        mBlocks[i].active = 0;
        mBlocks[i].animFrame = 0;
        mBlocks[i].hops = 0;
        mBlocks[i].stage = 0;
        mBlocks[i].coin = 0;
    }

    for (j = 0; j < 0x18; j++) {
        mCoins[j].x = 0;
        mCoins[j].y = 0;
        mCoins[j].state = 0;
        mCoins[j].placed = 0;
        mCoins[j].visible = 0;
        mCoins[j].value = 0;
        mCoins[j].keepEmpty = 0;
        mCoins[j].collected = 0;
    }

    *(u32 *)&pad_4d00[0] = 0;
    *(u32 *)&pad_4d00[4] = 0;
    mScore.total = 0;
    mScore.shown = 0;
    mScore.tick = 0;
    mScore.running = 0;
    for (k = 0; k < 0x20; k++) {
        mSparkles[k].active = 0;
        mSparkles[k].visible = 0;
    }

    for (l = 0; l < 0x18; l++) {
        mPopups[l].live = 0;
        mPopups[l].flag = 0;
    }

    mPhase = 0;
    mCountdown = 0;
    mWonLast = 0;
    pad_51dd = 0;
    *(u32 *)pad_51d0 = 0;
    mCaptionLatch = 0;
    Caption_Reset();
    Bag_Hide();
    mClearedAll = 0;
}

/* The result countdown. While the stylus is held it speeds to zero with a
   sound; otherwise it holds at 0x80. At zero the scene frees its graphics
   if it loaded them and starts the fade out. */
void dScMgCoin_c::Phase_StateResult() {
    if (mCountdown == 0)
        return;
    mCountdown--;
    int slot = gActivePlayerSlot;
    int touched = 0;
    if (gTouchHeld[slot * 4] != 0 && gTouchEdge[slot * 4] != 0)
        touched = 1;
    if (touched != 0 && mCountdown <= 0x80) {
        mCountdown = 0;
        Sound::PlayBank2_2D(0x62);
    } else {
        mCountdown = 0x80;
    }
    if (mCountdown > 0)
        return;
    mCountdown = 0;
    if (mClearedAll != 0) {
        FreeGfxSlotsById(6);
        Mg_RestoreMusic();
    }
    Mg_SetSharedState(MG_STATE_RESULTS);
    mPromptEnabled = 0;
}

/* Counts mCountdown down; at zero, re-arms the caption if the
   other player scored, then enters the result phase with 0xc0 frames. */
void dScMgCoin_c::Phase_StateScoreDelay() {
    if (mCountdown != 0) {
        mCountdown--;
        if (mCountdown < 0)
            mCountdown = 0;
        return;
    }
    if ((gMgScene != 0 ? gMgScene->mCredits : 0) != 0) {
        mCaptionLatch = 0;
        Caption_Trigger();
    }
    mCountdown = 0xc0;
    mPhase = 5;
}

/* Phase 4 on the way to the result: runs the blocks, coins and sparkles,
   and once nothing moves counts mCountdown down, bringing in the caption or
   the other screen at 0x20, then hands the score over and sets phase 4. */
void dScMgCoin_c::Phase_StateSettle() {
    int count;
    int i;
    int j;
    dScMgBase_c *g;

    count = 0;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].active) {
            int idx = mBlocks[i].state;
            (this->*sCoinBlockStateTable[idx].pmf)(i);
            if (mBlocks[i].state != 4)
                count++;
            if (mBlocks[i].visible != 0)
                this->Block_Animate(i);
        }
    }
    for (j = 0; j < 0x18; j++) {
        if (mCoins[j].state == 1 && mCoins[j].placed != 0)
            count++;
    }
    this->Coin_UpdateAll();
    this->Sparkle_UpdateAll();
    if (count != 0)
        return;

    if (mCountdown != 0) {
        mCountdown -= 1;
        if (mCountdown == 0x20 && mClearedAll == 0)
            this->Caption_Finish();
        if (mCountdown == 0x20 && mClearedAll != 0) {
            Mg_ShowBanner(6, 0x80, -0x80, -1, -1, 0xd);
            Mg_PlayJingle(0);
        }
        if (mCountdown <= 0)
            mCountdown = 0;
        return;
    }
    g = gMgScene;
    Mg_SubmitScore(g != 0 ? g->mCredits : 0);
    mCountdown = 0x70;
    mPhase = 4;
}

/* Phase 2, play: runs the blocks, coins and sparkles. Once every coin worth
   something has been touched the round ends (phase 3, 0x40 frames) and the
   win flag is set from the other score. */
void dScMgCoin_c::Phase_StatePlay() {
    int i;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].active != 0) {
            (this->*sCoinBlockStateTable[mBlocks[i].state].pmf)(i);
            this->Block_Animate(i);
        }
    }
    this->Coin_UpdateAll();
    this->Sparkle_UpdateAll();
    if (mPhase == 3) {
        return;
    }
    {
        int found = 0;
        int j;
        for (j = 0; j < 0x18; j++) {
            if (mCoins[j].value != 0) {
                if (mCoins[j].collected == 0) {
                    found++;
                    break;
                }
            }
        }
        if (found != 0) {
            return;
        }
    }
    mPhase = 3;
    mCountdown = 0x40;
    mClearedAll = 1;
    {
        int t;
        dScMgBase_c *g = gMgScene;
        if (g != 0) {
            t = g->mCredits;
        } else {
            t = 0;
        }
        if (t > mPlayerScore) {
            mWonLast = 1;
        } else {
            mWonLast = 0;
        }
    }
}

/* Phase 1: on the first frame, flags the scene and clears its timer, then
   runs the blocks. */
void dScMgCoin_c::Phase_StateIntro() {
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    Block_UpdateAll();
}

void dScMgCoin_c::Phase_StateIdle() {}

/* Slot 18. Starts the next round: counts a win if the last one was won,
   clears the scene, frees its graphics and lays the coins out again. It is
   void; the mov r0,#0 at the end is the source of the mPhase store. */
void dScMgCoin_c::StartRound(int arg) {
    if (mWonLast != 0) {
        mWins += 1;
    } else {
        mWins = 0;
    }
    mCredits = 0;
    mCreditsShown = mCredits;
    this->ResetRound();
    FreeGfxSlotsById(0x1d);
    this->LayOutCoins();
    this->Caption_Show();
    this->Bag_Show();
    mPlayerScore = Mg_GetTopRecord();
    mPhase = 0;
}

int dScMgCoin_c::Render() {
    this->DrawScorePlayer();
    this->DrawScoreRival();
    this->DrawResultPrompt();
    this->DrawBlocks();
    this->Caption_Draw();
    this->Bag_Draw();
    this->Sparkle_DrawAll();
    this->DrawResultNumbers();
    this->DrawCoinValues();
    this->Popup_DrawAll();
    return 1;
}

/* Runs this phase's handler from sCoinPhaseStateTable, then the caption,
   the moneybag, the popups and the score. */

int dScMgCoin_c::Behavior() {
    int idx = mPhase;
    (this->*sCoinPhaseStateTable[idx].pmf)();
    this->Caption_Update();
    this->Bag_Update();
    this->Popup_UpdateAll();
    this->Score_Update();
    return 1;
}

/* Loads and decompresses both screens' backgrounds, palettes and sprites,
   then sets up the first round. */
int dScMgCoin_c::InitResources() {
    void *a, *b, *d;

    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCG_BIN);
    DecompressLZ16(a, (void *)((int)G2_GetBG3CharPtr() + 0x4000));
    Deallocate(a);

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCL_BIN);
    GX_LoadBGPltt(a, 0x1e0, 0x20);
    Deallocate(a);

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_UP_NSC_BIN);
    GX_LoadBG3Scr(a, 0, 0x800);
    Deallocate(a);

    b = LoadFile(ASSET_D_2D_MG_WRECKING_CREW_NCG_BIN);
    a = LoadFile(ASSET_D_2D_MG_WRECKING_CREW_NCL_BIN);
    DecompressLZ16(b, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(a, 0, 0x100);

    gSubScreenLayers |= 8;
    G2S_SetBG3Priority(3);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                          GX_BG_CHARBASE_0x10000);

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCG_BIN);
    DecompressLZ16(d, (void *)((char *)G2S_GetBG3CharPtr() + 0x4000));
    Deallocate(d);

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCL_BIN);
    GXS_LoadBGPltt(d, 0x1e0, 0x20);
    Deallocate(d);

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_DOWN_NSC_BIN);
    GXS_LoadBG3Scr(d, 0, 0x800);
    Deallocate(d);

    DecompressLZ16(b, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(a, 0, 0x100);
    Deallocate(b);
    Deallocate(a);

    mCredits = 0;
    mCreditsShown = mCredits;
    data_0208ee44 = 1;
    this->ResetRound();
    this->LayOutCoins();
    this->Caption_Show();
    this->Bag_Show();
    mWins = 0;
    unk_51dc = 2;
    Mg_SetTopScreenYOffset(0x20);
    mPlayerScore = Mg_GetTopRecord();
    mRunDuringFade = 1;
    return 1;
}

/* The base constructor and the vptr store are the compiler's; the body is
   the one member that needs construction. */
inline dScMgCoin_c::dScMgCoin_c() {
    func_0203b9b4(&mSeed, 1);
}

/* Reconstructed source-style name: SM64DS proves dScMgCoin_c through RTTI,
 * allocation size, vtable identity, and the MG_COIN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgCoincentration_Spawn. */
extern "C" void *dScMgCoin_c_classInit() {
    return new dScMgCoin_c;
}

/* Scalar animation and movement tables. Blink has two frames; the other
 * caption sequences have four and eight. The two blink arrays each acquire
 * two bytes of linker alignment before the next table, not extra entries.
 * Row positions and heights are pixels; hop velocities are Q12.
 * The final flat table holds four XY pixel offsets for Sparkle_Burst.
 * Names describe the existing consumers, not recovered original identifiers. */
extern "C" {

extern const unsigned char sCoinCaptionBlinkTimes[2] = {7, 9};
extern const unsigned char sCoinCaptionBlinkSprites[2] = {2, 1};
extern const u8 sCoinCaptionCountInSprites[4] = {3, 4, 5, 6};
extern const u16 sCoinBlockFrameSprites[4] = {0, 1, 2, 1};
extern const unsigned char sCoinCaptionLoopSprites[8] = {7, 8, 9, 8, 9, 8, 9, 8};
extern const unsigned char sCoinCaptionLoopTimes[8] = {10, 60, 7, 45, 6, 7, 6, 3};
extern const u16 sCoinBlockFrameTimes[4] = {10, 8, 6, 8};
extern const u16 sCoinSparkleAngles[4] = {0xb000, 0xd000, 0x9000, 0xf000};
extern const u16 sCoinCaptionCountInTimes[4] = {30, 3, 10, 60};
extern const int sCoinBagHopVX[2] = {0xc00, -0xc00};
extern const int sCoinBlockHopVX[3] = {0, 0xc00, -0xc00};
extern const int sCoinRowY[3] = {48, 112, 176};
extern const int sCoinPopupY[3] = {-168, -108, -48};
extern const int sCoinBagRowTopY[6] = {-168, -108, -48, 48, 112, 176};
extern const int sCoinBagRowHeight[6] = {18, 18, 18, 26, 26, 26};
extern const int sCoinBlockHopRowY[6] = {-168, -108, -48, 48, 112, 176};
extern const int sCoinBlockHopRowHeight[6] = {16, 16, 16, 24, 24, 24};
extern const int sCoinBlockWanderRowY[6] = {-168, -108, -48, 48, 112, 176};
extern const int sCoinBlockWanderRowHeight[6] = {16, 16, 16, 24, 24, 24};
extern const int sCoinBlockDropRowY[6] = {-168, -108, -48, 48, 112, 176};
extern const int sCoinBlockFallRowY[6] = {-168, -108, -48, 48, 112, 176};
extern const int sCoinSparkleSpawnOffsets[8] = {-4, -4, 4, -4, -4, 4, 4, 4};
}
