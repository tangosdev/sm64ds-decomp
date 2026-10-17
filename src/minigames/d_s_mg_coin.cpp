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

// leftovers: DrawBlocks and Block_Animate still use local
// layout structs instead of the class members. The address-named helpers
// keep ROM names (no symbol table: SpawnBlocks, KickBlockUnderCoin
// and the func_ov006_020dc* family). RenderOamMainScreen and
// Mg_RenderSpriteBothMtx are redeclared extern "C" at the call -- the ROM symbols
// are unmangled and the sites disagree on the types. DrawScoreRival
// reads the other score at gMgScene + 0xac (not a dScMgBase_c
// field). Caption_StateDelay casts the scene through unsigned long long
// before clearing the caption frameTime (five words without it).
// Popup_UpdateAll/_020dc960/_020dcb1c stay on byte offsets (popup and
// sparkle structs miss: timer address reused, 0x18 kept as stride and timer,
// literal pool swapped). GetGameLanguage is called twice in
// DrawScorePlayer and again in DrawScoreRival (hoisting misses).

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
#include "decl_common.h"
#include "Sound.h"
#include "dScMgCoin_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

#pragma defer_codegen off /* ROM-ascending emission; dropping this inverts it */

/* Indexed from the object base. A pointer to the element misses. */
struct BagView {
    u8 pad[0x51a8];
    dScMgCoin_Bouncer bag[1];
};
struct CapView {
    u8 pad[0x5194];
    dScMgCoin_Caption cap[1];
};
struct CoinView {
    u8 pad[0x4ac0];
    dScMgCoin_Coin coin[1];
};
struct SparkView {
    u8 pad[0x4d14];
    dScMgCoin_Sparkle spark[1];
};

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
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int sCoinBagRowTopY[];
extern int sCoinBagRowHeight[];
extern int sCoinBagHopVX[];
extern int sCoinCaptionSprites[];
extern unsigned char sCoinCaptionLoopTimes[];
extern unsigned char sCoinCaptionLoopSprites[];
extern unsigned char sCoinCaptionBlinkTimes[];
extern unsigned char sCoinCaptionBlinkSprites[];
extern u16 sCoinCaptionCountInTimes[];
extern u8 sCoinCaptionCountInSprites[];
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern int data_ov006_0212e430[];
extern u16 sCoinSparkleAngles[];
extern s16 data_02082214[];
extern int sCoinPopupSprites;
extern int sCoinPopupY[];
extern int *sCoinScorePlayerSprites[];
extern int GetGameLanguage(void);
extern void Mg_DrawCreditsIcon(int r0, int r1, int r2, int r3);
extern void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern void *sCoinValuesSprites[];
extern void *sCoinSparkleSprites[];
extern void *sCoinBagSprites[];
extern void Mg_RenderSpriteBothPalMtx(void *obj, int x, int y, int w, int *vec);
extern void func_020127a4(int a, int b, int c, int d);
extern int sCoinRowY[];
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void RenderOamMainScreen(int a, int b, int c, int d, int e);
extern u16 sCoinBlockFrameSprites[];
extern void *sCoinBlocksSprites[];
extern int sCoinBlockFallRowY[];
extern int sCoinBlockDropRowY[];
extern int sCoinBlockWanderRowY[];
extern int sCoinBlockWanderRowHeight[];
extern int sCoinBlockHopRowY[];
extern int sCoinBlockHopRowHeight[];
extern int sCoinBlockHopVX[];
extern u16 sCoinBlockFrameTimes[];
extern void FreeGfxSlotsById(int n);
extern void Mg_PlayJingle(void *c);
extern int Mg_GetTopRecord(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
extern void func_0203b9b4(int *p, int v);
}

/* The per-state handler tables: {pmf, delta} records indexed by the
   record's own state byte. Static initialisation fills each one from the
   pointer-to-member constants at 0x0213be44..0x0213bf0c in .data, which
   stay outside this TU's claim, so they are reached by extern. The
   definitions are in the order the initialiser fills them. */
typedef void (dScMgCoin_c::*CoinPmf)(int);
struct CoinPmfEntry { CoinPmf pmf; };
typedef void (dScMgCoin_c::*PMF_Behavior)();
struct Entry_Behavior { PMF_Behavior pmf; };
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

// @symbol _ZN11dScMgCoin_cD1Ev
// @symbol _ZN11dScMgCoin_cD0Ev
/* Nothing to destroy by hand: D1 stores the vptr and calls dScMgBase_c's
   D2, and D0 adds the delete. */
dScMgCoin_c::~dScMgCoin_c()
{
}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgCoin_c() above,
 * so this spells out what the deleting destructor does in terms of it: the
 * D1 body, called qualified so it is a direct call, then the class-specific
 * operator delete. mwccarm never defines _MSC_VER, so nothing here reaches
 * the cartridge object. */
extern "C" dScMgCoin_c *_ZN11dScMgCoin_cD0Ev(dScMgCoin_c *thiz)
{
    thiz->dScMgCoin_c::~dScMgCoin_c();
    dScMgCoin_c::operator delete(thiz);
    return thiz;
}
#endif


// @symbol _ZN11dScMgCoin_c8Bag_DrawEv
/* The hand cursor while the stylus is down: the sine table entry for the
   scene's angle becomes a unit-scale 2x2 rotation matrix [cos, sin; -sin,
   cos] for the shared sprite call. The four matrix words are written
   straight into vec[], in slot order, after the icon index is read. A named
   rx/ry pair leaves the two 64-bit products in the wrong order or the sine
   base in the wrong register, and reading the icon index any later costs
   fifteen words. The pragma is part of that match. */
#pragma push
#pragma opt_propagation off
void dScMgCoin_c::Bag_Draw()
{

    if (mBouncer.shown == 0)
        return;
    {
        u16 idx_h = mBouncer.spin;
        s32 xr = mBouncer.x;
        s32 yr = mBouncer.y;
        int i = (idx_h >> 4) * 2;
        int vec[4];
        u8 idx_l = mBouncer.sprite;

        vec[0] = FX_MUL(data_02082214[i + 1], 0x1000);
        vec[1] = FX_MUL(data_02082214[i], 0x1000);
        vec[2] = -FX_MUL(data_02082214[i], 0x1000);
        vec[3] = FX_MUL(data_02082214[i + 1], 0x1000);
        Mg_RenderSpriteBothPalMtx(sCoinBagSprites[idx_l], xr >> 12, yr >> 12, -1, vec);
    }
}
#pragma pop

// @symbol _ZN11dScMgCoin_c19Bag_StateBounceDownEi
void dScMgCoin_c::Bag_StateBounceDown(int i) {
    BagView *scene = (BagView *)this;
    int height;
    int stageIndex;

    scene->bag[i].x += scene->bag[i].vx;
    scene->bag[i].y += scene->bag[i].vy;
    scene->bag[i].vy += kGravity;
    if (scene->bag[i].vx > 0)
        scene->bag[i].spin += kSpinStep;
    else
        scene->bag[i].spin -= kSpinStep;

    height = scene->bag[i].y >> 12;
    stageIndex = scene->bag[i].stage;

    if (stageIndex >= kBouncerDone) {
        if (height >= kOffBottom) {
            scene->bag[i].shown = 0;
            scene->bag[i].active = 0;
        }
        return;
    }

    if (scene->bag[i].bouncesLeft != 0) {
        unsigned int roll;
        if (height < (sCoinBagRowTopY[stageIndex] - sCoinBagRowHeight[stageIndex]))
            return;
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        scene->bag[i].vx = sCoinBagHopVX[((roll >> 16 & 0x7fff) << 1) >> 15];
        scene->bag[i].vy = kHopVy;
        scene->bag[i].bouncesLeft--;
        func_02012718(kSndBounce, scene->bag[i].x);
        if (scene->bag[i].bouncesLeft == 0)
            scene->bag[i].stage++;
    } else {
        scene->bag[i].bouncesLeft = 1;
    }
}

// @symbol _ZN11dScMgCoin_c15Bag_StateParkedEi
void dScMgCoin_c::Bag_StateParked(int index) {
    BagView *scene = (BagView *)this;
    if (scene->bag[index].countdown != 0) {
        scene->bag[index].countdown = scene->bag[index].countdown - 1;
        return;
    }
    scene->bag[index].state = 3;
    scene->bag[index].vy = 0;
    scene->bag[index].bouncesLeft = 1;
    scene->bag[index].stage = 0;
    this->SpawnBlocks();
}

// @symbol _ZN11dScMgCoin_c13Bag_StateRiseEi
void dScMgCoin_c::Bag_StateRise(int index) {
    BagView *scene = (BagView *)this;
    scene->bag[index].y = scene->bag[index].y + scene->bag[index].vy;
    scene->bag[index].vy = scene->bag[index].vy - kFallStep;
    scene->bag[index].spin = scene->bag[index].spin + kSpinStep;
    if ((scene->bag[index].y >> 12) > kParkRow)
        return;
    scene->bag[index].y = kParkedY;
    scene->bag[index].state = 2;
    scene->bag[index].countdown = 0x30;
    this->SpawnBlocks();
}

// @symbol _ZN11dScMgCoin_c11Bag_ReleaseEv
void dScMgCoin_c::Bag_Release()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.state = 1;
    scene->mBouncer.sprite = 1;
    scene->mBouncer.vx = 0;
    scene->mBouncer.vy = kKickVy;
}

// @symbol _ZN11dScMgCoin_c13Bag_StateIdleEv
void dScMgCoin_c::Bag_StateIdle() {}

// @symbol _ZN11dScMgCoin_c10Bag_UpdateEv
/* Dispatch the moneybag while mBouncer.active is set. Not a vtable:
   the records live in sCoinBagStateTable. */
void dScMgCoin_c::Bag_Update()
{
    if (this->mBouncer.active == 0)
        return;

    int state = this->mBouncer.state;
    (this->*sCoinBagStateTable[state].pmf)(0);
}

// @symbol _ZN11dScMgCoin_c8Bag_ShowEv
void dScMgCoin_c::Bag_Show()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.active = 1;
    scene->mBouncer.shown = 1;
    scene->mBouncer.sprite = 0;
    scene->mBouncer.state = 0;
    scene->mBouncer.spin = 0;
    scene->mBouncer.x = kBagRestX;
    scene->mBouncer.y = kBagRestY;
}

// @symbol _ZN11dScMgCoin_c8Bag_HideEv
void dScMgCoin_c::Bag_Hide()
{
    dScMgCoin_c *scene = this;
    scene->mBouncer.active = 0;
    scene->mBouncer.shown = 0;
}

// @symbol _ZN11dScMgCoin_c14Caption_FinishEv
void dScMgCoin_c::Caption_Finish()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.state = 4;
    scene->mCaption.frame = 0;
    scene->mCaption.frameTime = 0;
    scene->mCaptionLatch = 1;
}

// @symbol _ZN11dScMgCoin_c15Caption_TriggerEv
void dScMgCoin_c::Caption_Trigger()
{
    dScMgCoin_c *scene = this;
    if (scene->mCaptionLatch != 0)
        return;

    if (scene->mCaption.state == 2) {
        scene->mCaption.delay = kCaptionHold;
        return;
    }

    scene->mCaption.delay = kCaptionHold;
    scene->mCaption.state = 2;
    scene->mCaption.frame = 0;
    scene->mCaption.frameTime = 0;
}

// @symbol _ZN11dScMgCoin_c12Caption_DrawEv
void dScMgCoin_c::Caption_Draw()
{
    dScMgCoin_c *scene = this;

    if (scene->mCaption.visible == 0)
        return;

    RenderOamMainScreen(sCoinCaptionSprites[scene->mCaption.sprite], scene->mCaption.x >> 12,
                        scene->mCaption.y >> 12, -1, -1);
}

// @symbol _ZN11dScMgCoin_c17Caption_StateLoopEi
void dScMgCoin_c::Caption_StateLoop(int index) {
    CapView *scene = (CapView *)this;
    u16 *frameTime = &scene->cap[index].frameTime;
    u8 *frame = &scene->cap[index].frame;

    *frameTime = *frameTime + 1;
    if (*frameTime < sCoinCaptionLoopTimes[*frame])
        return;

    *frameTime = 0;
    *frame = *frame + 1;
    if (*frame >= 8)
        *frame = 1;
    else
        scene->cap[index].sprite = sCoinCaptionLoopSprites[*frame];
}

// @symbol _ZN11dScMgCoin_c19Caption_StateHiddenEi
void dScMgCoin_c::Caption_StateHidden(int index) {
    dScMgCoin_c *scene = this;
    (&scene->mCaption)[index].sprite = 0;
}

// @symbol _ZN11dScMgCoin_c18Caption_StateBlinkEi
void dScMgCoin_c::Caption_StateBlink(int index) {
    CapView *scene = (CapView *)this;
    u16 frameTime = scene->cap[index].frameTime;
    scene->cap[index].frameTime = frameTime + 1;
    u8 frame = scene->cap[index].frame;
    if (scene->cap[index].frameTime >= sCoinCaptionBlinkTimes[frame]) {
        scene->cap[index].frameTime = 0;
        scene->cap[index].frame = scene->cap[index].frame + 1;
        scene->cap[index].frame = scene->cap[index].frame & 1;
    }
    scene->cap[index].sprite = sCoinCaptionBlinkSprites[scene->cap[index].frame];
    if (this->mPhase == kResultPhase)
        return;
    u16 delay = scene->cap[index].delay;
    if (delay != 0) {
        scene->cap[index].delay = delay - 1;
        return;
    }
    scene->cap[index].state = 3;
    scene->cap[index].frame = 0;
    scene->cap[index].frameTime = 0;
    scene->cap[index].delay = 0;
}

// @symbol _ZN11dScMgCoin_c20Caption_StateCountInEi
void dScMgCoin_c::Caption_StateCountIn(int index) {
    CapView *scene = (CapView *)this;

    u16 *frameTime = &scene->cap[index].frameTime;
    u8 *frame = &scene->cap[index].frame;

    *frameTime = (u16)(*frameTime + 1);
    if (*frameTime < sCoinCaptionCountInTimes[*frame])
        return;
    *frameTime = 0;
    *frame = (u8)(*frame + 1);
    if (*frame == 2) {
        this->Bag_Release();
        Sound::PlayBank2_2D(kSndReady);
    }
    if (*frame >= 4) {
        scene->cap[index].delay = kCountInDelay;
        scene->cap[index].state = 2;
        {
            int *phase = &this->mPhase;
            *frame = 0;
            *frameTime = 0;
            *phase = *phase + 1;
        }
    } else {
        scene->cap[index].sprite = sCoinCaptionCountInSprites[*frame];
    }
}

// @symbol _ZN11dScMgCoin_c18Caption_StateDelayEi
void dScMgCoin_c::Caption_StateDelay(int index) {
    int raw = (int)this;
    CapView *scene = (CapView *)raw;

    if (scene->cap[index].delay != 0) {
        scene->cap[index].delay = scene->cap[index].delay - 1;
        if (scene->cap[index].delay == kChimeAt)
            Sound::PlayBank2_2D(kSndChime);
        return;
    }
    scene->cap[index].state = 1;
    ((CapView *)(unsigned long long)raw)->cap[index].frameTime = 0;
}

// @symbol _ZN11dScMgCoin_c14Caption_UpdateEv
/* Same dispatch as the moneybag, for the caption. */
void dScMgCoin_c::Caption_Update()
{
    if (this->mCaption.active == 0)
        return;

    int state = this->mCaption.state;
    (this->*sCoinCaptionStateTable[state].pmf)(0);
}

// @symbol _ZN11dScMgCoin_c12Caption_ShowEv
void dScMgCoin_c::Caption_Show()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.active = 1;
    scene->mCaption.state = 0;
    scene->mCaption.visible = 1;
    scene->mCaption.delay = kStartDelay;
    scene->mCaption.frameTime = 0;
    scene->mCaption.sprite = 3;
    scene->mCaption.frame = 0;
    scene->mCaption.x = kCaptionX;
    scene->mCaption.y = kCaptionY;
}

// @symbol _ZN11dScMgCoin_c13Caption_ResetEv
void dScMgCoin_c::Caption_Reset()
{
    dScMgCoin_c *scene = this;
    scene->mCaption.active = 0;
    scene->mCaption.visible = 1;
}

// @symbol _ZN11dScMgCoin_c16DrawResultPromptEv
void dScMgCoin_c::DrawResultPrompt()
{
    dScMgCoin_c *scene = this;
    if (scene->mPhase != kResultPhase)
        return;

    int left = scene->mCountdown;
    if (left > 0x80)
        return;
    if (left == 0)
        return;

    Mg_DrawBlinkSprite(scene, 0xe0, 0xa0);
}

// @symbol _ZN11dScMgCoin_c17DrawResultNumbersEv
void dScMgCoin_c::DrawResultNumbers()
{
    dScMgCoin_c *scene = this;
    int i;
    if (scene->mPhase == kResultPhase) {
        if (scene->mCountdown == 0)
            return;
    }
    for (i = 0; i < kPopupCount; i++) {
        if (scene->mPopups[i].flag != 0) {
            Mg_DrawBigNumber(scene->mPopups[i].x >> 12, scene->mPopups[i].y >> 12,
                scene->mPopups[i].value, 0, -1, 0, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c15Popup_UpdateAllEv
void dScMgCoin_c::Popup_UpdateAll()
{
    char *raw = (char *)this;
    /* Popup struct misses: the (int) cast is what makes the timer
       read-modify-write use a fresh address. */
    int i;
    for (i = 0; i < 0x18; i++) {
        char *popup = raw + (int)((long long)i) * 0x10;
        if (*(u8 *)(popup + 0x5020) != 0 && *(u16 *)(popup + 0x501c) != 0) {
            *(u16 *)((int)(popup + 0x501c)) -= 1;
            if (*(s16 *)((int)(popup + 0x501c)) <= 0)
                *(u8 *)(popup + 0x5021) = 1;
        }
    }
}

// @symbol _ZN11dScMgCoin_c11Popup_SpawnEi
void dScMgCoin_c::Popup_Spawn(int index)
{
    char *raw = (char *)this;
    /* Two strides, 0x18 and 0x10. The struct form keeps 0x18 alive
       past the coin and pushes a register. */
    char *coin = raw + index * 0x18;
    mPopups[index].x = mCoins[index].x;
    mPopups[index].y = mCoins[index].y;
    mPopups[index].value = mCoins[index].value;
    mPopups[index].live = 1;
    mPopups[index].timer = 0x18;
}

// @symbol _ZN11dScMgCoin_c15Sparkle_DrawAllEv
void dScMgCoin_c::Sparkle_DrawAll()
{
    SparkView *scene = (SparkView *)this;

    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (scene->spark[i].visible != 0) {
            int sprite = scene->spark[i].sprite;
            int x = scene->spark[i].x;
            int y = scene->spark[i].y;
            Mg_RenderSpriteBothMtx(sCoinSparkleSprites[sprite], x >> 12, y >> 12, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c17Sparkle_UpdateAllEv
void dScMgCoin_c::Sparkle_UpdateAll()
{
    SparkView *scene = (SparkView *)this;
    int i;
    for (i = 0; i < kSparkleCount; i++) {
        if (scene->spark[i].active != 0) {
            if (scene->spark[i].life != 0) {
                scene->spark[i].life -= 1;
            } else {
                scene->spark[i].active = 0;
                scene->spark[i].visible = 0;
                return;
            }
            scene->spark[i].x += scene->spark[i].vx;
            scene->spark[i].y += scene->spark[i].vy;
            scene->spark[i].vy += kSparkleFall;
            if (scene->spark[i].vx > kSparkleCap)
                scene->spark[i].vx -= kSparkleDrag;
            else if (scene->spark[i].vx < -kSparkleCap)
                scene->spark[i].vx += kSparkleDrag;
        }
    }
}

// @symbol _ZN11dScMgCoin_c13Sparkle_BurstEi
void dScMgCoin_c::Sparkle_Burst(int coinIndex)
{
    char *raw = (char *)this;
    /* Sparkle struct reorders the literal pool (0212e430 before 0212e334). */
    int i, j, k;
    char *coin = raw + coinIndex * 0x18;
    int *coinX = (int *)(coin + 0x4ac0);
    int *coinY = (int *)(coin + 0x4ac4);
    for (i = 0, j = 0, k = 1; i < 4; i++, raw += 0x18, j += 2, k += 2) {
        if (*(u8 *)(raw + 0x4d28) == 0) {
            *(u8 *)(raw + 0x4d28) = 1;
            *(u8 *)(raw + 0x4d29) = 1;
            *(int *)(raw + 0x4d14) = *coinX + (data_ov006_0212e430[j] << 12);
            *(int *)(raw + 0x4d18) = *coinY + (data_ov006_0212e430[k] << 12);
            {
                u16 ang = sCoinSparkleAngles[i];
                int sine = (ang >> 4) * 2;
                *(int *)(raw + 0x4d1c) = FX_MUL(data_02082214[sine + 1], 0x2000);
                *(int *)(raw + 0x4d20) = FX_MUL(data_02082214[sine], 0x2000);
            }
            *(u16 *)(raw + 0x4d24) = 0x18;
        }
    }
}

// @symbol _ZN11dScMgCoin_c13Popup_DrawAllEv
void dScMgCoin_c::Popup_DrawAll()
{
    dScMgCoin_c *scene = this;

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

// @symbol _ZN11dScMgCoin_c15DrawScorePlayerEv
void dScMgCoin_c::DrawScorePlayer()
{
    dScMgCoin_c *scene = this;

    int lang;
    if (scene->mPhase < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(sCoinScorePlayerSprites[lang][8], 0x80, 0x18, -1, -1);
    Mg_DrawCreditsIcon(0x68, 0x28, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)sCoinScorePlayerSprites[lang][1], (void *)0x7a, 0x28, (void *)0);
    Mg_DrawBigNumber(0x8c, 0x28, scene->mPlayerScore, 1, -1, 2, 0);
}

// @symbol _ZN11dScMgCoin_c14DrawScoreRivalEv
void dScMgCoin_c::DrawScoreRival()
{
    dScMgCoin_c *scene = this;

    int lang;
    int count;
    if (scene->mPhase < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(sCoinScorePlayerSprites[lang][4], 0x80, 0x50, -1, -1);
    Mg_DrawCreditsIcon(0x6c, 0x60, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)sCoinScorePlayerSprites[lang][1], (void *)0x7e, 0x60, (void *)0);
    if (gMgScene != 0)
        count = gMgScene->mCreditsShown;
    else
        count = 0;
    Mg_DrawBigNumber(0x90, 0x60, count, 1, -1, 2, 0);
}

// @symbol _ZN11dScMgCoin_c12Score_UpdateEv
void dScMgCoin_c::Score_Update()
{
    dScMgCoin_c *scene = this;
    if (scene->mScore.running == 0)
        return;
    if (scene->mScore.total == scene->mScore.shown)
        return;
    {
        u16 *tick = &scene->mScore.tick;
        *tick = *tick + 1;
    }
    if (scene->mScore.tick < 8)
        return;
    scene->mScore.tick = 0;
    {
        u16 *shown = &scene->mScore.shown;
        *shown = *shown + 1;
    }
}

// @symbol _ZN11dScMgCoin_c14DrawCoinValuesEv
void dScMgCoin_c::DrawCoinValues()
{
    dScMgCoin_c *scene = this;
    int i;
    int v1, v2;
    if (scene->mPhase == kResultPhase && scene->mCountdown == 0)
        return;

    v1 = 0;
    v2 = 0;
    for (i = 0; i < kCoinCount; i++) {
        int st = scene->mCoins[i].state;
        int sprite = (st == kCoinSettled) ? 1 : v1;
        int x = scene->mCoins[i].x >> 12;
        int y = scene->mCoins[i].y >> 12;
        int highlight = (scene->mPhase == kResultPhase) ? 1 : v2;
        if (st == kCoinSettled)
            highlight = 1;
        if (scene->mPhase != kResultPhase) {
            if (scene->mCoins[i].visible != 0)
                Mg_RenderSpriteBoth(sCoinValuesSprites[sprite], x, y, -1, highlight);
        } else {
            if (sprite != 0 && scene->mCoins[i].value != 0)
                Mg_RenderSpriteBoth(sCoinValuesSprites[sprite], x, y, -1, highlight);
            if (scene->mCoins[i].collected == 0 && scene->mCoins[i].value != 0)
                Mg_DrawBigNumber(x, y, scene->mCoins[i].value, 0, -1, 0, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c17Coin_StateSettledEv
void dScMgCoin_c::Coin_StateSettled() {}

// @symbol _ZN11dScMgCoin_c13Coin_StateHopEi
void dScMgCoin_c::Coin_StateHop(int index) {
    CoinView *scene = (CoinView *)this;

    scene->coin[index].y = scene->coin[index].y + scene->coin[index].vy;
    scene->coin[index].vy = scene->coin[index].vy + kGravity;
    if (scene->coin[index].y < scene->coin[index].landY)
        return;
    scene->coin[index].y = scene->coin[index].landY;
    scene->coin[index].bounces = scene->coin[index].bounces + 1;
    this->Caption_Trigger();
    if (scene->coin[index].value == scene->coin[index].bounces) {
        scene->coin[index].state = kCoinSettled;
        this->Popup_Spawn(index);
        return;
    }
    scene->coin[index].vy = kHopVy;
    this->KickBlockUnderCoin(index);
}

// @symbol _ZN11dScMgCoin_c19Coin_StateWaitTouchEi
/* The touch test for one coin. In phase 2, with the stylus down and newly
   pressed, a touch within 0x10 of the coin takes it: a coin still worth
   something starts its hop, otherwise the round ends (phase 3, a 0x40 timer,
   the panned reveal jingle, and the win flag from the other score).
   Four spellings carry the match: p1 is assigned before p0; the touch point
   is re-read through gActivePlayerSlot rather than the i already in hand; each
   of the five coin stores spells its own self + n + 0x4000 + ... with no
   hoisted base; and the hop target re-reads its address instead of *p1. */
void dScMgCoin_c::Coin_StateWaitTouch(int idx) {
    char *self = (char *)this;
    int i;
    int ok;
    int n;
    int v;
    int w;
    int *p0;
    int *p1;
    int ax;
    int ay;
    int q0;
    int q1;
    int ang;
    int stars;
    int need;

    if (mPhase != 2)
        return;

    i = gActivePlayerSlot;
    ok = 0;
    if (gTouchHeld[i * 4] != 0 && gTouchEdge[i * 4] != 0)
        ok = 1;
    if (ok == 0)
        return;

    n = idx * 0x18;
    p1 = (int *)(self + 0x4ac4 + n);
    p0 = (int *)(self + 0x4ac0 + n);
    q1 = *p1;
    ax = gTouchX[gActivePlayerSlot * 4];
    ay = gTouchY[gActivePlayerSlot * 4];
    q0 = *p0;
    v = ax - (q0 >> 12);
    w = ay - (q1 >> 12);

    if (v <= -0x10)
        return;
    if (v >= 0x10)
        return;
    if (w <= -0x10)
        return;
    if (w >= 0x10)
        return;

    *(u8 *)(self + n + 0x4000 + 0xad5) = 1;
    if (*(u8 *)(self + n + 0x4000 + 0xad3) != 0) {
        *(u8 *)(self + n + 0x4000 + 0xad0) = 1;
        *(int *)(self + n + 0x4000 + 0xac8) = *(int *)(self + 0x4ac4 + n);
        *(int *)(self + n + 0x4000 + 0xacc) = -0x3000;
        *(u8 *)(self + n + 0x4000 + 0xad6) = 0;
        this->KickBlockUnderCoin(idx);
        return;
    }

    this->Sparkle_Burst(idx);
    *(u8 *)(self + idx * 0x18 + 0x4000 + 0xad2) = 0;
    mPhase = 3;
    mCountdown = 0x40;

    ang = (*p0 >> 12) - 0x80;
    ang >>= 1;
    if (ang >= 0x3c)
        ang = 0x3c;
    if (ang <= -0x3c)
        ang = -0x3c;
    func_020127a4(2, 0xee, 0xffff, ang);
    Sound::PlayBank2_2D(0xf2);

    *(u8 *)(self + 0x4000 + idx * 0x18 + 0xad0) = 2;
    stars = (gMgScene != 0) ? gMgScene->mCredits : 0;
    need = mPlayerScore;
    if (stars > need)
        mWonLast = 1;
    else
        mWonLast = 0;
}

// @symbol _ZN11dScMgCoin_c14Coin_UpdateAllEv
/* Runs each coin's state handler from sCoinCoinStateTable. */
void dScMgCoin_c::Coin_UpdateAll()
{
    int i;
    for (i = 0; i < 0x18; i++) {
        unsigned char k = this->mCoins[i].state;
        (this->*sCoinCoinStateTable[k].pmf)(i);
    }
}

// @symbol _ZN11dScMgCoin_c11LayOutCoinsEv
/* Lays the 24 coins out in three rows of eight and marks two coins in each
   row, picked at random, at +0x14. */
void dScMgCoin_c::LayOutCoins()
{
    char *c = (char *)this;
    int a[3];
    int b[3];
    int k;
    int col;
    int row;
    int i;

    for (k = 0; k < 3; k++) {
        a[k] = (int)((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 3) >> 15);
    }

    for (k = 0; k < 3; k++) {
        unsigned int w = ((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 6;
        b[k] = (a[k] + (int)(w >> 15) + 1) & 7;
    }

    row = 0;
    col = 0;
    for (i = 0; i < 0x18; i++) {
        mCoins[i].x = (((col << 5) + 0x10) << 12);
        mCoins[i].y = sCoinRowY[row] << 12;
        col++;
        mCoins[i].pad11 = 1;
        mCoins[i].visible = 1;
        if (col >= 8) { col = 0; row++; }
    }

    *(unsigned char *)(c + a[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + b[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[2] + 0x10) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[2] + 0x10) * 0x18 + 0x4ad4) = 1;
}

// @symbol _ZN11dScMgCoin_c18KickBlockUnderCoinEi
/* Finds the first resting block (state 4) that belongs to coin id, kicks it
   up from under the coin (state 5) and plays the panned hop sound. */
void dScMgCoin_c::KickBlockUnderCoin(int id)
{
    char *base = (char *)this;
    int i;
    char *p = base;
    for (i = 0; i < 0x28; i++) {
        if (*(unsigned char *)(p + 0x4677) != 0 &&
            *(unsigned char *)(p + 0x4675) == 4 &&
            id == *(unsigned char *)(p + 0x467b)) {
            int v = mCoins[id].y;
            mBlocks[i].y = v - 0x20000;
            mBlocks[i].unk16 = 1;
            mBlocks[i].state = 5;
            mBlocks[i].vy = -0x4800;
            mBlocks[i].delay = 0;
            int w = (mCoins[id].x >> 12) - 0x80;
            int r = w >> 1;
            if (r >= 0x3c) r = 0x3c;
            if (r <= -0x3c) r = -0x3c;
            func_020127a4(2, 0xef, 0xffff, r);
            return;
        }
        p += 0x1c;
    }
}

// @symbol _ZN11dScMgCoin_c10DrawBlocksEv
/* Draws the 40 blocks on both screens. */
typedef struct {
    int f0;
    int f4;
    char pad8[0xd];
    unsigned char b15;
    unsigned char b16;
    char pad17;
    unsigned char b18;
    char pad19[3];
} E_dd594;

typedef struct {
    char pad0[0x4660];
    E_dd594 e[40];
    char pad1[0x708];
    int f51c8;
    int f51cc;
} Self_dd594;

void dScMgCoin_c::DrawBlocks()
{
    Self_dd594 *self = (Self_dd594 *)this;
    int i;
    if (self->f51c8 == 5 && self->f51cc == 0) return;
    for (i = 0; i < 0x28; i++) {
        if (self->e[i].b16 == 0) continue;
        {
            unsigned int flag = self->e[i].b15;
            int v0 = self->e[i].f0;
            int v4 = self->e[i].f4;
            int a1, a2, a4;
            unsigned short idx;
            a1 = v0 >> 12;
            a2 = v4 >> 12;
            a4 = (flag >= 3) ? 1 : 0;
            idx = sCoinBlockFrameSprites[self->e[i].b18];
            RenderOamBothScreens(sCoinBlocksSprites[idx], a1, a2, -1, a4, 0);
        }
    }
}

// @symbol _ZN11dScMgCoin_c17Block_StateFallInEi
/* A block falling into place. The panned landing sound plays on the frame
   vy turns from rising to falling; once it passes its row it rests (state
   4), bumps the running total and bursts an effect. */
struct Pair_dd658 { int a; int b; };
extern "C" void Mg_SpawnFx(int a, int b, struct Pair_dd658 *p2, struct Pair_dd658 *p3,
                                    struct Pair_dd658 *p4);

void dScMgCoin_c::Block_StateFallIn(int i) {
    int n = i * 0x1c;
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
        mBlocks[i].unk16 = 0;
        mBlocks[i].unk17 = 0;
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

// @symbol _ZN11dScMgCoin_c18Block_StateRestingEv
void dScMgCoin_c::Block_StateResting() {}

// @symbol _ZN11dScMgCoin_c21Block_StateDropOnCoinEi
/* A block dropping onto its coin's row. It stops at the row height from
   sCoinBlockDropRowY, takes the coin's x and rests (state 4). */
void dScMgCoin_c::Block_StateDropOnCoin(int index) {
    char *thiz = (char *)this;
    int off = index * 0x1c;
    int new_var;
    int *pa = (int *)((thiz + 0x466c) + off);
    int *pb = (int *)((thiz + 0x4664) + off);
    *pb = (*pb) + (*((int *)((thiz + 0x466c) + off)));
    *((int *)((thiz + 0x466c) + off)) = (*pa) + 0x400;
    new_var = 0x674;
    int look = sCoinBlockDropRowY[*((unsigned char *)(((thiz + off) - -0x4000) + new_var))];
    if (((*pb) >> 12) <= look) {
        return;
    }
    new_var = (*((unsigned char *)(((thiz + off) + 0x4000) + 0x67b))) * 0x18;
    char *g = (thiz + new_var) + 0x4000;
    *((int *)(((thiz + off) + 0x4000) + 0x660)) = *((int *)(g + 0xac0));
    *pb = look << 12;
    *pa = 0;
    *((int *)(((thiz + off) + 0x4000) + 0x668)) = 0;
    *((unsigned char *)(((thiz + off) + 0x4000) + 0x675)) = 4;
    pa += 0;
    *((unsigned char *)(((thiz + off) + 0x4000) + 0x676)) = 0;
}

// @symbol _ZN11dScMgCoin_c17Block_StateWanderEi
/* A block wandering along its row. When it reaches the row it picks a
   random direction, and over a coin of that row still worth at most two
   (five when IsAltVariant says so) it claims the coin, adds one to its value
   and starts to rise (state 3). */
#define LB(a) (*(unsigned char *)(((long long)(int)(a))))

void dScMgCoin_c::Block_StateWander(int i) {
    char *c = (char *)this;
    int n = i * 0x1c;
    int idxb;
    int diff;
    int base_j;
    int fp;
    int j;
    int *pD;

    *(int *)(c + 0x4660 + n) += *(int *)(c + 0x4668 + n);
    *(int *)(c + 0x4664 + n) += *(int *)(c + 0x466c + n);
    *(int *)(c + 0x466c + n) += 0x400;
    this->Block_ClampToWalls(i);

    pD = (int *)(c + 0x466c + n);
    idxb = *(u8 *)(c + n + 0x4674);
    diff = sCoinBlockWanderRowY[idxb] - sCoinBlockWanderRowHeight[idxb];
    if ((*(int *)(c + 0x4664 + n) >> 12) > diff) {
        *(int *)(c + 0x4664 + n) = diff * 0x1000;
        *pD = 0;
        if (*(int *)(c + 0x4668 + n) == 0) {
            unsigned int rv = (unsigned int)RandomIntInternal(&data_0209d4b8);
            if (((((rv >> 16) & 0x7fff) << 1) >> 15))
                *(int *)(c + 0x4668 + n) = 0xc00;
            else
                *(int *)(c + 0x4668 + n) = -0xc00;
        }
    }
    {
        int bval = *(int *)(c + 0x4664 + n);
        if (bval != (diff << 12))
            return;
    }

    base_j = (idxb - 3) * 8;
    fp = *(int *)(c + 0x4660 + n) >> 12;
    for (j = 0; j < 8; j++) {
        int idx = base_j + j;
        int d = (*(int *)((c + idx * 0x18) + 0x4ac0) >> 12) - fp;
        int limit = 2;
        char *e = c + idx * 0x18;
        int res = this->IsAltVariant();
        if (res != 0) limit = 5;
        if (d <= -6 || d >= 6) continue;
        if (*(u8 *)(e + 0x4ad4) != 0) continue;
        if (LB(e + 0x4ad3) > limit) continue;
        LB(e + 0x4ad3) += 1;
        *(u8 *)(c + n + 0x4675) = 3;
        *pD = 0x200;
        *(u8 *)(c + n + 0x467b) = (u8)idx;
        return;
    }
}

#undef LB

// @symbol _ZN11dScMgCoin_c18Block_StateHopDownEi
/* A block hopping down the stages. Each landing picks a new random vx and
   plays the panned hop; when its hops for a stage run out it moves down a
   stage, and past its own row it switches to state 2. */
void dScMgCoin_c::Block_StateHopDown(int i) {
    char *self = (char *)this;
    int n = i * 0x1c;
    char *fp;

    mBlocks[i].x += mBlocks[i].vx;
    mBlocks[i].y += mBlocks[i].vy;
    mBlocks[i].vy += 0x400;

    this->Block_ClampToWalls(i);

    {
        u8 g = mBlocks[i].hops;
        int y = mBlocks[i].y >> 12;
        u8 h = mBlocks[i].stage;

        if (g != 0) {
            int rnd, idx, a, v;
            u8 g3;

            fp = (char *)(sCoinBlockHopRowY[h] - sCoinBlockHopRowHeight[h]);
            if (y < (int)fp) return;

            rnd = RandomIntInternal(&data_0209d4b8);
            idx = ((((unsigned)rnd >> 16) & 0x7fff) * 3) >> 15;
            mBlocks[i].vx = sCoinBlockHopVX[idx];
            mBlocks[i].vy = -0x2e00;
            mBlocks[i].y = (int)fp << 12;
            mBlocks[i].hops -= 1;

            a = (mBlocks[i].x >> 12) - 0x80;
            v = a >> 1;
            if (v >= 0x3c) v = 0x3c;
            if (v <= -0x3c) v = -0x3c;
            func_020127a4(2, 0x197, 0xffff, v);

            g3 = mBlocks[i].hops;
            if (g3 == 0) {
                mBlocks[i].stage += 1;
            }
            {
                u8 hh = mBlocks[i].stage;
                u8 e = mBlocks[i].row;
                if (hh > e) mBlocks[i].state = 2;
            }
        } else {
            unsigned r;
            mBlocks[i].hops = 1;
            r = (unsigned)RandomIntInternal(&data_0209d4b8);
            r = (((r >> 16) & 0x7fff) << 3) >> 15;
            if (r == 2 || r == 5) {
                mBlocks[i].hops += 1;
            }
            return;
        }
    }
}

// @symbol _ZN11dScMgCoin_c18Block_StateWaitingEi
/* A block waiting to drop: counts its delay down, then starts it (state 1). */
void dScMgCoin_c::Block_StateWaiting(int i) {
    char *c = (char *)this;
    int idx = i * 0x1c;
    char *r2 = c + 0x4670;
    unsigned short *p = (unsigned short *)(r2 + idx);
    if (*p != 0) {
        *p = *p - 1;
        if (*(short *)p < 0) *(short *)p = 0;
        return;
    }
    char *r0 = c + idx;
    *(char *)(r0 + 0x4000 + 0x675) = 1;
    *(char *)(r0 + 0x4000 + 0x679) = 1;
}

// @symbol _ZN11dScMgCoin_c18Block_ClampToWallsEi
/* Keeps a block between x 8 and 0xf8, reversing vx at either wall. */
void dScMgCoin_c::Block_ClampToWalls(int idx)
{
    int *f60 = &mBlocks[idx].x;
    int v = *f60 >> 12;
    if (v < 8) {
        *f60 = 0x8000;
        mBlocks[idx].vx = -mBlocks[idx].vx;
        return;
    }
    if (v > 0xf8) {
        *f60 = 0xf8000;
        mBlocks[idx].vx = -mBlocks[idx].vx;
    }
}

// @symbol _ZN11dScMgCoin_c15Block_UpdateAllEv
/* Runs every live block's state handler and animation. When none is left
   moving the scene goes to phase 2, the score starts running and the start
   sound plays. */
void dScMgCoin_c::Block_UpdateAll()
{
    char *thiz = (char *)this;
    int n = 0;
    int i = 0;
    char *p = thiz;
    for (; i < 0x28; i++) {
        if (*(unsigned char *)(p + 0x4000 + 0x677) != 0) {
            (this->*sCoinBlockStateTable[*(unsigned char *)(p + 0x4000 + 0x675)].pmf)(i);
            if (*(unsigned char *)(p + 0x4000 + 0x675) != 4)
                n++;
            this->Block_Animate(i);
        }
        p += 0x1c;
    }
    if (n != 0)
        return;
    mPhase = 2;
    mScore.running = 1;
    Sound::PlayBank2_2D(0x151);
}

// @symbol _ZN11dScMgCoin_c13Block_AnimateEi
/* Steps one block's four-frame animation; sCoinBlockFrameTimes holds each
   frame's length. */
typedef struct {
    char _pad0[0x12];
    u16 timer;   /* +0x12 */
    char _pad1[4];
    u8 frame;    /* +0x18 */
    char _pad2[3];
} Entry_dde28; /* 0x1c */

typedef struct {
    char _pad0[0x4660];
    Entry_dde28 entries[16];
} Work_dde28;

void dScMgCoin_c::Block_Animate(int index)
{
    char *c = (char *)this;
    Work_dde28 *w = (Work_dde28 *)c;
    w->entries[index].timer++;
    if (w->entries[index].timer < sCoinBlockFrameTimes[w->entries[index].frame])
        return;
    w->entries[index].frame++;
    w->entries[index].frame &= 3;
    w->entries[index].timer = 0;
}

// @symbol _ZN11dScMgCoin_c11SpawnBlocksEv
/* Parks the blocks above the top of the screen at random x with staggered
   delays, cycling their rows 3, 4, 5. Sixteen when IsAltVariant says so,
   otherwise forty. */
void dScMgCoin_c::SpawnBlocks()
{
    int n;
    unsigned int row = 0;
    int i;
    n = this->IsAltVariant() != 0 ? 0x10 : 0x28;
    for (i = 0; i < n; i++) {
        unsigned int v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        mBlocks[i].x = ((((v << 3) >> 0xf) << 5) + 0x10) << 0xc;
        mBlocks[i].y = -0xf0000;
        mBlocks[i].unk17 = 1;
        mBlocks[i].unk16 = 1;
        {
            unsigned int w = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mBlocks[i].delay = (short)(((i & 0xf) << 3) + (((w << 4) >> 0xf) << 4));
        }
        mBlocks[i].row = (unsigned char)(row + 3);
        row = (row + 1) & 0xff;
        if (row >= 3) row = 0;
    }
}

// @symbol _ZN11dScMgCoin_c10ResetRoundEv
/* Clears the blocks, coins, score, sparkles, popups and phase, then resets
   the caption and the moneybag. */
void dScMgCoin_c::ResetRound()
{
    char *c = (char *)this;
    int i, k, j, i2, i3, i4;
    char *p = c;
    k = 0;
    for (; k < 0x28; k++) {
        i = 0;
        *((u32 *)(p + 0x4660)) = i;
        *((u32 *)(p + 0x4664)) = i;
        *((u32 *)(p + 0x4668)) = i;
        *((u32 *)(p + 0x466c)) = i;
        *((u16 *)(p + 0x4670)) = i;
        *((u16 *)(p + 0x4672)) = i;
        *((u8 *)(p + 0x4674)) = i;
        *((u8 *)(p + 0x4675)) = i;
        *((u8 *)(p + 0x4676)) = i;
        *((u8 *)(p + 0x4677)) = i;
        *((u8 *)(p + 0x4678)) = i;
        *((u8 *)(p + 0x4679)) = i;
        *((u8 *)(p + 0x467a)) = i;
        *((u8 *)(p + 0x467b)) = i;
        p += 0x1c;
    }

    p = c;
    for (i2 = 0; i2 < 0x18; i2++) {
        j = 0;
        *((u32 *)(p + 0x4ac0)) = j;
        *((u32 *)(p + 0x4ac4)) = j;
        *((u8 *)(p + 0x4ad0)) = j;
        *((u8 *)(p + 0x4ad1)) = j;
        *((u8 *)(p + 0x4ad2)) = j;
        *((u8 *)(p + 0x4ad3)) = j;
        *((u8 *)(p + 0x4ad4)) = j;
        *((u8 *)(p + 0x4ad5)) = j;
        p += 0x18;
    }

    *((u32 *)(c + 0x4d00)) = 0;
    *((u32 *)(c + 0x4d04)) = 0;
    *(&mScore.total) = 0;
    *(&mScore.shown) = 0;
    *(&mScore.tick) = 0;
    *((u8 *)(c + 0x4d13)) = 0;
    p = c;
    for (i3 = 0; i3 < 0x20; i3++) {
        *((u8 *)(p + 0x4d28)) = 0;
        *((u8 *)(p + 0x4d29)) = 0;
        p += 0x18;
    }

    for (i4 = 0; i4 < 0x18; i4++) {
        char *e = c + ((i4 & 0xFFFFFFFFu) << 4);
        *((u8 *)(e + 0x5020)) = 0;
        *((u8 *)(e + 0x5021)) = 0;
    }

    *((u32 *)(c + 0x51c8)) = 0;
    *((u32 *)(c + 0x51cc)) = 0;
    mWonLast = 0;
    *((u8 *)(c + 0x51dd)) = 0;
    *((u32 *)(c + 0x51d0)) = 0;
    mCaptionLatch = 0;
    this->Caption_Reset();
    this->Bag_Hide();
    *((u8 *)(c + 0x51df)) = 0;
}

// @symbol _ZN11dScMgCoin_c17Phase_StateResultEv
/* The result countdown. While the stylus is held it speeds to zero with a
   sound; otherwise it holds at 0x80. At zero the scene frees its graphics
   if it loaded them and starts the fade out. */
void dScMgCoin_c::Phase_StateResult() {
    char *self = (char *)this;
    if (mCountdown == 0) return;
    mCountdown -= 1;
    unsigned int idx = gActivePlayerSlot;
    int flag = 0;
    if (gTouchHeld[idx * 4] != 0) {
        if (gTouchEdge[idx * 4] != 0) flag = 1;
    }
    if (flag != 0 && mCountdown <= 0x80) {
        mCountdown = 0;
        Sound::PlayBank2_2D(0x62);
    } else {
        mCountdown = 0x80;
    }
    if (mCountdown > 0) return;
    mCountdown = 0;
    if (*(unsigned char *)(self + 0x51df) != 0) {
        FreeGfxSlotsById(6);
        Mg_RestoreMusic();
    }
    Mg_SetSharedState(MG_STATE_RESULTS);
    mPromptEnabled = 0;
}

// @symbol _ZN11dScMgCoin_c21Phase_StateScoreDelayEv
/* Counts the countdown at 0x51cc down; at zero, re-arms the caption if the
   other player scored, then enters the result phase with 0xc0 frames. */
void dScMgCoin_c::Phase_StateScoreDelay() {
    char *c = (char *)this;
    if (*(int *)((c + 0x5000) + 0x1cc) != 0) {
        int *p = (int *)(int)(((long long)(int)(c + 0x51cc)));
        *p = *p - 1;
        if (*(int *)((c + 0x5000) + 0x1cc) < 0)
            *(int *)((c + 0x5000) + 0x1cc) = 0;
        return;
    }
    {
        dScMgBase_c *o = gMgScene;
        int v = o != 0 ? o->mCredits : 0;
        if (v != 0) {
            mCaptionLatch = 0;
            this->Caption_Trigger();
        }
    }
    mCountdown = 0xc0;
    mPhase = 5;
}

// @symbol _ZN11dScMgCoin_c17Phase_StateSettleEv
/* Phase 4 on the way to the result: runs the blocks, coins and sparkles,
   and once nothing moves counts 0x51cc down, bringing in the caption or
   the other screen at 0x20, then hands the score over and sets phase 4. */
void dScMgCoin_c::Phase_StateSettle() {
    char *self = (char *)this;
    int count;
    int i;
    int j;
    dScMgBase_c *g;

    count = 0;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].unk17) {
            int idx = mBlocks[i].state;
            (this->*sCoinBlockStateTable[idx].pmf)(i);
            if (mBlocks[i].state != 4) count++;
            if (mBlocks[i].unk16 != 0) this->Block_Animate(i);
        }
    }
    for (j = 0; j < 0x18; j++) {
        if (mCoins[j].state == 1 && mCoins[j].pad11 != 0)
            count++;
    }
    this->Coin_UpdateAll();
    this->Sparkle_UpdateAll();
    if (count != 0) return;

    if ((&mSparkles[31].y)[0x73] != 0) {
        *(int *)(((long long)(int)(self + 0x51cc))) -= 1;
        if ((&mSparkles[31].y)[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] == 0)
            this->Caption_Finish();
        if ((&mSparkles[31].y)[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] != 0) {
            Mg_ShowBanner(6, 0x80, -0x80, -1, -1, 0xd);
            Mg_PlayJingle(0);
        }
        if ((&mSparkles[31].y)[0x73] <= 0) (&mSparkles[31].y)[0x73] = 0;
        return;
    }
    g = gMgScene;
    Mg_SubmitScore(g != 0 ? g->mCredits : 0);
    (&mSparkles[31].y)[0x73] = 0x70;
    (&mSparkles[31].y)[0x72] = 4;
}

// @symbol _ZN11dScMgCoin_c15Phase_StatePlayEv
/* Phase 2, play: runs the blocks, coins and sparkles. Once every coin worth
   something has been touched the round ends (phase 3, 0x40 frames) and the
   win flag is set from the other score. */
void dScMgCoin_c::Phase_StatePlay() {
    char *c = (char *)this;
    int i;
    for (i = 0; i < 0x28; i++) {
        if (mBlocks[i].unk17 != 0) {
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
    *(u8 *)(c + 0x51df) = 1;
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

// @symbol _ZN11dScMgCoin_c16Phase_StateIntroEv
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

// @symbol _ZN11dScMgCoin_c15Phase_StateIdleEv
void dScMgCoin_c::Phase_StateIdle() {}

// @symbol _ZN11dScMgCoin_c10StartRoundEi
/* Slot 18. Starts the next round: counts a win if the last one was won,
   clears the scene, frees its graphics and lays the coins out again. It is
   void; the mov r0,#0 at the end is the source of the mPhase store. */
void dScMgCoin_c::StartRound(int arg)
{
    dScMgCoin_c *self = this;

    if (self->mWonLast != 0) {
        self->mWins += 1;
    } else {
        self->mWins = 0;
    }
    self->mCredits = 0;
    self->mCreditsShown = self->mCredits;
    this->ResetRound();
    FreeGfxSlotsById(0x1d);
    this->LayOutCoins();
    this->Caption_Show();
    this->Bag_Show();
    self->mPlayerScore = Mg_GetTopRecord();
    self->mPhase = 0;
}

// @symbol _ZN11dScMgCoin_c6RenderEv
int dScMgCoin_c::Render()
{
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

// @symbol _ZN11dScMgCoin_c8BehaviorEv
/* Runs this phase's handler from sCoinPhaseStateTable, then the caption,
   the moneybag, the popups and the score. */

int dScMgCoin_c::Behavior()
{
    int idx = mPhase;
    (this->*sCoinPhaseStateTable[idx].pmf)();
    this->Caption_Update();
    this->Bag_Update();
    this->Popup_UpdateAll();
    this->Score_Update();
    return 1;
}

// @symbol _ZN11dScMgCoin_c13InitResourcesEv
/* Loads and decompresses both screens' backgrounds, palettes and sprites,
   then sets up the first round. */
int dScMgCoin_c::InitResources()
{
    dScMgCoin_c *self = this;
    char *c = (char *)self;
    int a, b, d;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e &= ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCG_BIN);
    DecompressLZ16(a, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)a);

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCL_BIN);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)a, 0x1e0, 0x20);
    Deallocate((void *)a);

    a = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_UP_NSC_BIN);
    func_02056314((void *)a, 0, 0x800);
    Deallocate((void *)a);

    b = LoadFile(ASSET_D_2D_MG_WRECKING_CREW_NCG_BIN);
    a = LoadFile(ASSET_D_2D_MG_WRECKING_CREW_NCL_BIN);
    DecompressLZ16(b, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)a, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1210;

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCG_BIN);
    DecompressLZ16(d, (void *)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
    Deallocate((void *)d);

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_NCL_BIN);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)d, 0x1e0, 0x20);
    Deallocate((void *)d);

    d = LoadFile(ASSET_D_2D_MG_BG_WRECKING_CREW_DOWN_NSC_BIN);
    func_020562b4((const void *)d, 0, 0x800);
    Deallocate((void *)d);

    DecompressLZ16(b, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)a, 0, 0x100);
    Deallocate((void *)b);
    Deallocate((void *)a);

    self->mCredits = 0;
    self->mCreditsShown = self->mCredits;
    data_0208ee44 = 1;
    this->ResetRound();
    this->LayOutCoins();
    this->Caption_Show();
    this->Bag_Show();
    self->mWins = 0;
    self->unk_51dc = 2;
    Mg_SetTopScreenYOffset(0x20);
    self->mPlayerScore = Mg_GetTopRecord();
    self->mRunDuringFade = 1;
    return 1;
}

/* The base constructor and the vptr store are the compiler's; the body is
   the one member that needs construction. */
inline dScMgCoin_c::dScMgCoin_c()
{
    func_0203b9b4(&mSeed, 1);
}

/* Reconstructed source-style name: SM64DS proves dScMgCoin_c through RTTI,
 * allocation size, vtable identity, and the MG_COIN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgCoincentration_Spawn. */
// @symbol dScMgCoin_c_classInit
extern "C" void *dScMgCoin_c_classInit()
{
    return new dScMgCoin_c;
}
