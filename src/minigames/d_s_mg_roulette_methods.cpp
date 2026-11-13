//cpp
/* dScMgRoulette_c -- Mushroom Roulette. Racers are dealt onto a spinning
 * board, the countdown runs, and each racer is paid by the tile it stops on.
 *
 * _ZTS15dScMgRoulette_c 0x0213e30c. _ZTI15dScMgRoulette_c 0x0213e300.
 * _ZTV15dScMgRoulette_c 0x0213e39c. The destructor is inline in the
 * header: D1 at 0x0210788c, D0 at 0x02107920, no D2. InitResources is the
 * key function.
 * 40 functions, .text 0x0210788c..0x0210a400.
 *
 * Functions are in reverse ROM order; do not reorder.
 *
 * common.h comes first so its flat Matrix4x3 wins the MATRIX4X3_DEFINED
 * guard; RouletteBoard_Update copies a whole matrix and needs that spelling.
 */

// leftovers:
// - The RouletteBoard_, RouletteRacer_ and RouletteStrip_ helpers are
//   still free functions over their record. The ones decl_common.h declares
//   (RouletteBoard_LoadModels, _StartSpin, _Draw, RouletteRacer_Draw,
//   RouletteStrip_LinkAll) keep its char* parameter. mMeter stays bytes in
//   the header, reached through the local RouletteStrip view. Most func_
//   and data_ helpers are unnamed in symbols.txt.
// - unk_53e0..unk_53f4 are only written in this unit (StartRound and phase 3), so
//   they stay unnamed.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgRoulette_c_AssetId {
    ASSET_D_2D_MG_BG_ROULETTE_NCG_BIN = 0x006f,
    ASSET_D_2D_MG_BG_ROULETTE_NCL_BIN = 0x0070,
    ASSET_D_2D_MG_BG_ROULETTE_NSC_BIN = 0x0071,
    ASSET_D_2D_MG_ROULETTE_NCG_BIN = 0x00e9,
    ASSET_D_2D_MG_ROULETTE_NCL_BIN = 0x00ea,
    ASSET_D_2D_YAKU_NSC_BIN = 0x01e4
};

#include "common.h"
#include "nitro/hw/registers.h"
#include "types.h"
#include "Vector2.h"
#include "dScMgRoulette_c.h"
#include "dExtSimpleModel_c.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "LevelRng.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/fx/fx_cp.h"
namespace s20b2220 { void Mg_DrawNumberRotated(int, int, int, int, int, int, u16); }
using s20b2220::Mg_DrawNumberRotated;
namespace s20b1bc8 { void Mg_DrawCredits(char *a0, int, int, int); }
using s20b1bc8::Mg_DrawCredits;
namespace s20b1b08 { void Mg_LoseCredits(int); }
using s20b1b08::Mg_LoseCredits;
namespace s20b0aa0 { void FreeGfxSlotsById(int); }
using s20b0aa0::FreeGfxSlotsById;
namespace s20b0104 { void RenderOamBothScreens(void *attr, int, int, int, int, void *mtx); }
using s20b0104::RenderOamBothScreens;
namespace s20af68c { void Hud_RenderSprite(void *attr, int, int, int, int); }
using s20af68c::Hud_RenderSprite;
namespace s20b1b40 { void Mg_GainCredits(int); }
using s20b1b40::Mg_GainCredits;
namespace s20adc68 { void *Ov004_LoadFile(int); }
using s20adc68::Ov004_LoadFile;
namespace s20adc5c { void Ov004_Deallocate(void *ptr); }
using s20adc5c::Ov004_Deallocate;
namespace s20ad674 { s32 GetGameLanguage(void); }
using s20ad674::GetGameLanguage;
void MgCoins_Reset(void);
void MgCoins_Update(void);
void MgCoins_StartPayout(int);

/* Local views of the records the helpers walk. */

/* One row of the 12-entry angle table at sRouletteSlowStops, copied whole. */
typedef struct {
    short v[12];
} ShortTbl12;

/* Two Fix12 words copied as a unit out of pmf_RouletteBoard_StateSlow and
   pmf_RouletteBoard_StateSpin. Both keep the array member: a whole-struct copy of
   `{int a, b;}` is split into load, store, load, store, where the ROM does
   both loads first. */
struct Pair {
    int w[2];
};
struct Pair2W {
    int w[2];
};

/* One payout strip, 0x18 bytes. The scene's mMeter holds four, linked into
   sRouletteStrips; each collects the racers paid at one multiplier. */
struct RouletteStrip {
    RouletteRacer *racers[5]; /* 0x00, filled in order; 0 ends the list */
    s16 payoutIndex;  /* 0x14, sRoulettePayoutMultipliers index; ResetAll
                         gives strip i index i */
    s16 count;        /* 0x16, racers added this round */
};
typedef char RouletteStrip_size_must_be_0x18[sizeof(RouletteStrip) == 0x18 ? 1 : -1];

/* The active minigame scene's mCredits, or 0 with no scene. */
#define SCENE_BONUS ((gMgScene != 0) \
                        ? gMgScene->mCredits : 0)

/* Thirty-seven tile centres. mwcc emits __sinit_d_s_mg_roulette_methods.cpp
   from this array: each Vector2(x, y) is two stores and a ~Vector2()
   registration, in this order. The centres are 20.12 positions. */
Vector2 sRouletteTiles[37] = {
    Vector2(0x70000, 0x24000), Vector2(0xa0000, 0x24000), Vector2(0xd0000, 0x24000),
    Vector2(0x70000, 0x4c000), Vector2(0xa0000, 0x4c000), Vector2(0xd0000, 0x4c000),
    Vector2(0x70000, 0x74000), Vector2(0xa0000, 0x74000), Vector2(0xd0000, 0x74000),
    Vector2(0x70000, 0x9c000), Vector2(0xa0000, 0x9c000), Vector2(0xd0000, 0x9c000),
    Vector2(0x88000, 0x24000), Vector2(0xb8000, 0x24000), Vector2(0x88000, 0x4c000),
    Vector2(0xb8000, 0x4c000), Vector2(0x88000, 0x74000), Vector2(0xb8000, 0x74000),
    Vector2(0x88000, 0x9c000), Vector2(0xb8000, 0x9c000), Vector2(0x70000, 0x38000),
    Vector2(0xa0000, 0x38000), Vector2(0xd0000, 0x38000), Vector2(0x70000, 0x60000),
    Vector2(0xa0000, 0x60000), Vector2(0xd0000, 0x60000), Vector2(0x70000, 0x88000),
    Vector2(0xa0000, 0x88000), Vector2(0xd0000, 0x88000), Vector2(0x88000, 0x38000),
    Vector2(0xb8000, 0x38000), Vector2(0x88000, 0x60000), Vector2(0xb8000, 0x60000),
    Vector2(0x88000, 0x88000), Vector2(0xb8000, 0x88000), Vector2(0x38000, 0x38000),
    Vector2(0x38000, 0x88000),
};

extern "C" {





void Vec2_Sub(int *out, int *a, int *b);}

int Sound_PlayIfNotActive(int a, int b, int c, short d);extern "C" {

void *LoadFile(int id);

void DecompressLZ16(void *src, void *dst);




void func_0203d680(Vector2 *out, const Vector2 *in, int scale);






void MgCoins_StartPayout(int);

void MgCoins_Update(void);

void MgCoins_Reset(void);}
int Dealer_IsAnimDone(void *a);
int Dealer_IsIdle(Dealer *dealer);
extern "C" {

void Dealer_Draw(void *c);}
namespace s020c19d0 {
void Dealer_Update(Dealer *dealer);
}
using s020c19d0::Dealer_Update;
extern "C" {

int Dealer_Load(void *c);}


int sRouletteResultsTimer;

extern int sRouletteResultIndex;

RouletteStrip *sRouletteStrips[4];

void *sRouletteLandedRacer;

extern int data_020a0db0;

/* The first of the two blink frames in sRouletteStripSprites for each strip's
   marker: {0, 2, 6, 8} for strips 0..3. A horizontal pair adds 2. */
extern s32 sRouletteStripSpriteBase[];

extern void *sRouletteStripSprites[];

extern void **sRouletteTextSpritesByLanguage[];

extern int sRoulettePayoutMultipliers[];

extern void *sRouletteRacerSprites;

/* Bet tiles: 0..0xb are single numbers, 0xc..0x1c pairs of numbers and
 * 0x1d..0x22 groups of four (RouletteRacer_GetPayout checks 1, 2 and 4
 * table entries for them and pays 12, 6 and 3). */
static inline int Roulette_IsPairTile(int tile) {
    return (tile >= 0xc && tile <= 0x1c) ? 1 : 0;
}


static inline int Roulette_IsQuadTile(int tile) {
    return (tile >= 0x1d && tile <= 0x22) ? 1 : 0;
}


extern short sRouletteSlowStops[];

/* The single number (0..0xb) under each of the twelve stops in
   sRouletteSlowStops, in wheel order. */
extern short sRouletteStopTiles[12];

extern struct Pair pmf_RouletteBoard_StateSlow;

extern struct Pair2W pmf_RouletteBoard_StateSpin;

extern int sRouletteTileBounds[];

extern int sRouletteRowEdgesY[];

/* The numbers each pair tile (0xc..0x1c) and each group-of-four tile
   (0x1d..0x22) covers. */
extern short sRoulettePairNumbers[17][2];

extern short sRouletteQuadNumbers[6][4];

extern s32 sMgCoinRiseFrames;

extern u16 gMgCoinsStatus;

extern u8 gMainScreenLayers;

extern u8 gSubScreenLayers;
void RouletteResults_Draw(void);
void RouletteStrip_AddRacer(RouletteStrip *strip, RouletteRacer *racer);
void RouletteStrip_Reset(RouletteStrip *strip);
void RouletteBoard_Update(RouletteBoard *c);
void RouletteBoard_StateSlow(RouletteBoard *board);
void RouletteBoard_StartSlowdown(RouletteBoard *board);
void RouletteBoard_StateSpin(RouletteBoard *board);
void RouletteBoard_SetStateSpin(RouletteBoard *board);
int RouletteBoard_FreeModels(RouletteBoard *res);
short Roulette_TileAt(int x, int y);
int RouletteRacer_GetPayout(RouletteRacer *racer, int tile);
void RouletteRacer_CheckLandingB(RouletteRacer *racer);
int RouletteRacer_CheckLandingA(RouletteRacer *racer);
void RouletteRacer_Update(RouletteRacer *racer);
void RouletteRacer_Release(RouletteRacer *racer);
void RouletteRacer_Init(RouletteRacer *racer);
void RouletteRacer_MoveToTile(RouletteRacer *racer, int tile);
void RouletteRacer_SetTarget(RouletteRacer *racer, int *target);
void RouletteRacer_SetVelocityToward(RouletteRacer *racer, int *target, int scale);
void RouletteRacer_AimAt(RouletteRacer *racer, int *target);
void RouletteStrip_DrawRacers(RouletteStrip *strip);
void RouletteStrip_Draw(RouletteStrip *strip, int y);


/* Sets up both screens: the sub BG layers, the board tiles, map and
   palette, and the shared OBJ tiles and palettes for both engines. Then
   sets up the camera, the dealer and the board and starts the first round. */
s32 dScMgRoulette_c::InitResources() {
    void *bgChars, *bgMap, *file, *pltt;

    Mg_SetTopScreenYOffset(0x20);

    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x04000);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2800,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);

    bgChars = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NCG_BIN);
    bgMap = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NSC_BIN);
    file = LoadFile(ASSET_D_2D_YAKU_NSC_BIN);
    pltt = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NCL_BIN);
    DecompressLZ16(bgChars, G2S_GetBG2CharPtr());
    DecompressLZ16(bgMap, G2S_GetBG2ScrPtr());
    DecompressLZ16(file, G2S_GetBG0ScrPtr());
    GXS_LoadBGPltt(pltt, 0x1a0, 0x60);
    Deallocate(bgChars);
    Deallocate(bgMap);
    Deallocate(file);
    Deallocate(pltt);

    G2S_BG0Mosaic(0);
    G2S_BG2Mosaic(0);
    SetSubBg0Offset(0, 0);
    SetSubBg2Offset(0, 0);
    G2S_SetBG0Priority(2);
    G2S_SetBG2Priority(2);

    file = LoadFile(ASSET_D_2D_MG_ROULETTE_NCG_BIN);
    pltt = LoadFile(ASSET_D_2D_MG_ROULETTE_NCL_BIN);
    DecompressLZ16(file, G2_GetOBJCharPtr());
    DecompressLZ16(file, G2S_GetOBJCharPtr());
    GX_LoadOBJPltt(pltt, 0, 0x100);
    GXS_LoadOBJPltt(pltt, 0, 0x100);
    Deallocate(file);
    Deallocate(pltt);

    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    gSubScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_OBJ;
    G2_SetBG0Priority(1);

    Camera_InitDealerView(&mCamera);
    if (mDealer.Load() == 0)
        return 0;
    if (RouletteBoard_LoadModels((char *)&mBoard) == 0)
        return 0;

    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    Mg_PlaceBannersOnTopScreen();
    RouletteStrip_LinkAll((char *)mMeter);
    StartRound(-1);
    return 1;
}

/* One frame of the round, by mPhase: 1 deals the racers out one at a time,
   2 runs the countdown and scores the board, 3 pays out per landed tile and
   4 announces the result. The racer update loops run every frame. */
s32 dScMgRoulette_c::Behavior() {
    switch (mPhase) {
    case 0:
        break;
    case 1:
        if (mDealer.IsAnimDone() != 0) {
            if (mBoard.mRollSound == 0) {
                mBoard.mRollSound = 1;
            }
        }
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (mDealer.IsAnimDone() != 0) {
            mPhaseTimer -= 1;
            if (mPhaseTimer == 0) {
                mCameraPreset = 1;
                if (mDealIndex < mRacerCount) {
                    int dealPos[2];
                    int dealY;
                    /* The int cast keeps the negation out of the add that uses
                       it, as the ROM does; without it Behavior's size changes
                       (measured 2026-11-06). The long long round trip it used
                       to go through was doing nothing. */
                    dealY = (int)(0 - (mDealIndex << 16)) + 0x80000;
                    dealPos[0] = 0x20000;
                    dealPos[1] = dealY;
                    RouletteRacer_AimAt(&mRacers[mDealIndex], dealPos);
                    mDealIndex += 1;
                    mPhaseTimer = 8;
                    Mg_LoseCredits(1);
                    Sound::PlayBank2_2D(0x14d);
                } else {
                    for (int i = 0; i < mRacerCount; i++)
                        mRacers[i].state = 1;
                    mPhaseTimer = 0x258;
                    mDealer.PlayIdle();
                    mPhase += 1;
                    mDealIndex = 0;
                    Sound::PlayBank2_2D(0x15b);
                }
            }
        }
        break;
    case 2: {
        int timer = mPhaseTimer;
        if (timer % 60 == 0 && timer > 0 && timer <= 0xb4) {
            Sound::PlayBank2_2D(0xa6);
        }
        mPhaseTimer -= 1;
        if (mPhaseTimer == 0) {
            {
                short tileType;
                for (int i = 0; i < mRacerCount; i++) {
                    mRacers[i].tile =
                        Roulette_TileAt(mRacers[i].targetX >> 12, mRacers[i].targetY >> 12);
                    tileType = mRacers[i].tile;
                    if (Roulette_IsPairTile(tileType) || Roulette_IsQuadTile(tileType)) {
                        RouletteRacer_SetVelocityToward(&mRacers[i], &sRouletteTiles[tileType].x,
                                                        0x100);
                    } else {
                        RouletteRacer_MoveToTile(&mRacers[i], tileType);
                    }
                }
            }
            mDealIndex = 0;
            mCameraPreset += 1;
            mPhase += 1;
            mBoard.mBraking = 1;
            mPhaseTimer = 1;
            Sound::PlayBank2_2D(0x15a);
            {
                int sounded = 0;
                for (int j = 0; j < mRacerCount; j++) {
                    if (mRacers[j].tile == 0x25) {
                        RouletteRacer_Init(&mRacers[j]);
                        if (sounded == 0) {
                            Sound::PlayBank2_2D(0x15c);
                            sounded = 1;
                        }
                    } else {
                        mRacers[j].state = 3;
                        mTargetScore += 1;
                    }
                }
                Mg_SaveRecordPair(mRacerCount - mTargetScore + SCENE_BONUS, 0);
            }
        }
        break;
    }
    case 3: {
        int stopped = (mBoard.mSpinSpeed == 0) ? 1 : 0;
        if (stopped != 0) {
            mPromptEnabled = 0;
            short wait = mPhaseTimer;
            mPhaseTimer -= 1;
            if (wait == 0) {
                int tile = mBoard.mStopTile;
                mScore = 0;
                for (int j = 0; j < mRacerCount; j++) {
                    int payout = RouletteRacer_GetPayout(&mRacers[j], tile);
                    if (payout == 2) {
                        RouletteStrip_AddRacer((RouletteStrip *)mMeter[3], &mRacers[j]);
                    } else if (payout == 3) {
                        RouletteStrip_AddRacer((RouletteStrip *)mMeter[2], &mRacers[j]);
                    } else if (payout == 6) {
                        RouletteStrip_AddRacer((RouletteStrip *)mMeter[1], &mRacers[j]);
                    } else if (payout == 0xc) {
                        RouletteStrip_AddRacer((RouletteStrip *)mMeter[0], &mRacers[j]);
                    }
                    mScore += payout;
                }
                Mg_SaveRecordPair(mScore + SCENE_BONUS, 0);
                unk_53f4 = 0;
                mDealIndex = 0;
                mPhaseTimer = 0x5a;
                mPhase += 1;
            }
        }
        break;
    }
    case 4: {
        mPhaseTimer -= 1;
        if (mPhaseTimer == 0) {
            short target = mTargetScore;
            short score = mScore;
            if (score > target) {
                mDealer.PlayWin();
                Mg_SetSharedState(MG_STATE_WIN);
            } else if (score < target) {
                mDealer.PlayLose();
                Mg_SetSharedState(MG_STATE_LOSE);
            } else {
                Mg_SetSharedState(MG_STATE_TIE);
            }
            mCameraPreset = 0;
            mPhase += 1;
        }
        break;
    }
    }

    for (int n = 0; n < mRacerCount; n++)
        RouletteRacer_Update(&mRacers[n]);
    for (int n = 0; n < mRacerCount; n++)
        RouletteRacer_CheckLandingB(&mRacers[n]);
    RouletteBoard_Update(&mBoard);
    mDealer.Update();
    MgCoins_Update();
    return 1;
}

/* A tail-call veneer to RouletteResults_Draw. */

void Roulette_DrawResultsThunk(char *) {
    RouletteResults_Draw();
}


/* Draws the racers back to front, the cursor on the selected tile, the
   countdown (its volume ramps over the last 3 seconds), and moves the camera
   to the current preset. */
s32 dScMgRoulette_c::Render() {
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();

    {
        int i = mRacerCount - 1;
        for (; i >= 0; i--)
            RouletteRacer_Draw((char *)&mRacers[i]);
    }

    Roulette_DrawResultsThunk((char *)this);

    {
        int idle = (int)(mBoard.mSpinSpeed == 0);
        if (idle != 0) {
            if (mPhase < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = mBoard.mStopTile;
                    Hud_RenderSprite((void *)data_ov006_02138c18, sRouletteTiles[idx].x >> 12,
                                     sRouletteTiles[idx].y >> 12, -1, 1);
                }
            }
        }
    }

    if (mPhase == 2) {
        int timer = mPhaseTimer;
        if (timer <= 0xb4) {
            int frames = timer + 0x3b;
            s16 rem = frames % 60;
            int seconds = frames / 60;
            int vol = rem * 0x88 + 0x1000;
            if (vol > 0x2000)
                vol = 0x2000;
            Mg_DrawNumberRotated(0x80, 0x60, seconds, 1, -1, FX_Inv(vol), 0);
        }
    }

    {
        s16 preset = mCameraPreset;
        if (preset != 0) {
            int off = preset * 0xc;
            s16 angle = *(s16 *)(data_ov006_0213e2e0 + preset * 2);
            {
                int d1 = *(int *)(data_ov006_0213e354 + off);
                int b1 = *(int *)(data_ov006_0213e350 + off);
                int a1 = *(int *)(data_ov006_0213e34c + off);
                mCamera.mPos.x = a1;
                mCamera.mPos.y = b1;
                mCamera.mPos.z = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                mCamera.mLookAt.x = d2;
                mCamera.mLookAt.y = e2;
                mCamera.mLookAt.z = f2;
            }
            mCamera.mFovAngle = angle;
            Camera_UpdateMatrices(&mCamera);
        } else {
            Camera_InitDealerView(&mCamera);
        }
        if (preset != 0)
            RouletteBoard_Draw((char *)&mBoard);
        else
            mDealer.Draw();
    }

    return 1;
}

/* Frees the slider's two model files. */
s32 dScMgRoulette_c::CleanupResources() {
    RouletteBoard_FreeModels(&mBoard);
    return 1;
}

/* Runs the end of the round, phases 5 to 9. Returns 1 when it is done. */
int dScMgRoulette_c::CanEndState(int /* mode */) {

    switch (mPhase) {
    case 5:
        FreeGfxSlotsById(0x1d);
        if (RouletteResults_Tick() != 0)
            (mPhase)++;
        break;
    case 6:
        if (mDealer.IsIdle() != 0) {
            mPhaseTimer = 0x3c;
            mDealIndex = 0;
            (mPhase)++;
        }
        break;
    case 7:
        (mPhaseTimer)--;
        if (mPhaseTimer == 0) {
            if (mScore != 0)
                MgCoins_StartPayout(mScore);
            (mPhase)++;
        }
        break;
    case 8:
        (mPhase)++;
        /* fall through */
    case 9:
    default:
        if (gMgCoinsStatus <= 1)
            return 1;
        break;
    }
    return 0;
}

/* Resets the round: the dealer, the racers, the score and the board, with
   one racer per bonus the scene holds, up to five. InitResources calls it
   with -1. */
void dScMgRoulette_c::StartRound(int /* arg */) {

    int i;
    RouletteRacer *racer;
    int count;

    RouletteStrip_ResetAll();
    mDealer.StartSwap();

    mScore = 0;
    mPhaseTimer = 0xa;

    racer = mRacers;
    for (i = 0; i < 5; i++) {
        RouletteRacer_PlaceAtStart((int *)racer);
        racer++;
    }

    mDealIndex = 0;
    mTargetScore = 0;
    count = 0;
    if (gMgScene != 0)
        count = gMgScene->mCredits;
    if (count >= 5)
        count = 5;
    mRacerCount = count;

    RouletteBoard_StartSpin((char *)&mBoard);

    sRouletteLandedRacer = 0;
    mCameraPreset = 0;
    unk_53e0 = 0x100;
    unk_53ea = 0;
    unk_53ec = 0;
    unk_53ed = 0;
    unk_53ee = 0;
    unk_53ef = 0;
    unk_53f0 = 0;
    mPhase = 1;

    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
}


void RouletteRacer_AimAt(RouletteRacer *racer, int *target) {
    racer->homeX = target[0];
    racer->homeY = target[1];
    RouletteRacer_SetVelocityToward(racer, target, 0x80);
}



void RouletteRacer_SetVelocityToward(RouletteRacer *racer, int *target, int scale) {
    Vector2 delta;
    Vector2 step;
    racer->targetX = target[0];
    racer->targetY = target[1];
    Vec2_Sub(&delta.x, &racer->targetX, &racer->x);
    func_0203d680(&step, &delta, scale);
    racer->stepX = step.x;
    racer->stepY = step.y;
    if (racer->stepX < 0)
        racer->stepX = -racer->stepX;
    if (racer->stepY < 0)
        racer->stepY = -racer->stepY;
}



void RouletteRacer_SetTarget(RouletteRacer *racer, int *target) {
    Vector2 delta;
    racer->targetX = target[0];
    racer->targetY = target[1];
    Vec2_Sub(&delta.x, &racer->targetX, &racer->x);
    racer->stepX = delta.x;
    racer->stepY = delta.y;
    if (func_0203d434(&racer->stepX)) {
        func_0203d630(&racer->stepX, 0x18000);
    }
    if (racer->stepX < 0)
        racer->stepX = -racer->stepX;
    if (racer->stepY < 0)
        racer->stepY = -racer->stepY;
}


/* Clamps the racer's target into a box round the tile's centre and slides
   it there: tiles 0x23 and 0x24 get the box to their right, tiles up to 0xb
   one centred on them; any other tile leaves the racer alone. */

void RouletteRacer_MoveToTile(RouletteRacer *racer, int tile) {
    Vector2 goal;
    goal.x = racer->targetX;
    goal.y = racer->targetY;
    if ((unsigned short)(short)(tile - 0x23) <= 1) {
        Vector2 centre = sRouletteTiles[tile];
        int lo = centre.x + 0x8000;
        int hi = centre.x + 0x18000;
        if (goal.x >= lo) {
            if (goal.x <= hi)
                hi = goal.x;
            lo = hi;
        }
        goal.x = lo;
        lo = centre.y - 0x20000;
        hi = centre.y + 0x20000;
        if (goal.y >= lo) {
            if (goal.y <= hi)
                hi = goal.y;
            lo = hi;
        }
        goal.y = lo;
        RouletteRacer_SetVelocityToward(racer, &goal.x, 0x100);
        return;
    }
    /* Spelled as a 0/1 flag: the ROM materialises it (movle #1 / movgt #0,
       cmp #0); `if (racer->tile > 0xb) return;` and a bool local both
       build 0x10 bytes short. */
    {
        int early = (racer->tile <= 0xb) ? 1 : 0;
        if (early == 0)
            return;
    }
    Vector2 centre = sRouletteTiles[tile];
    int lo = centre.x - 0x10000;
    int hi = centre.x + 0x10000;
    if (goal.x >= lo) {
        if (goal.x <= hi)
            hi = goal.x;
        lo = hi;
    }
    goal.x = lo;
    lo = centre.y - 0xc000;
    hi = centre.y + 0xc000;
    if (goal.y >= lo) {
        if (goal.y <= hi)
            hi = goal.y;
        lo = hi;
    }
    goal.y = lo;
    RouletteRacer_SetVelocityToward(racer, &goal.x, 0x100);
}


/* First call builds the goal at (0xc000, 0xc000) and registers ~Vector2. */

void RouletteRacer_Init(RouletteRacer *racer) {
    static Vector2 sRouletteRacerGoal(0xc000, 0xc000);
    RouletteRacer_SetVelocityToward(racer, &sRouletteRacerGoal.x, 0x80);
    racer->state = 7;
}



void RouletteRacer_PlaceAtStart(int *racer) {
    RouletteRacer *t = (RouletteRacer *)racer;
    t->x = 0xc000;
    t->y = 0xc000;
    t->targetX = t->x;
    t->targetY = t->y;
    t->stepX = 0;
    t->stepY = 0;
    t->state = 6;
    t->tile = 0x25;
    t->payout = 0;
    t->priority = 1;
}



void RouletteRacer_Release(RouletteRacer *racer) {
    racer->tile = 0x25;
    racer->state = 1;
    RouletteRacer_SetVelocityToward(racer, &racer->homeX, 0x100);
}



void RouletteRacer_Update(RouletteRacer *racer) {
    u8 state;
    s16 type;

    state = racer->state;
    if (state == 0 || state == 3) {
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    }

    state = racer->state;
    if (state == 1) {
        RouletteRacer_CheckLandingA(racer);
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    } else if (state == 7) {
        s32 out[2];
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
        Vec2_Sub(out, &racer->targetX, &racer->x);
        if (out[0] != 0)
            return;
        if (out[1] != 0)
            return;
        racer->state = 6;
        Mg_GainCredits(1);
    } else if (state == 4) {
        s32 out[2];
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
        Vec2_Sub(out, &racer->targetX, &racer->x);
        if (out[0] != 0)
            return;
        if (out[1] == 0) {
            racer->hop = 0x14;
            racer->state = 5;
        }
    } else if (state == 5) {
        s32 d;
        s32 q;
        d = sMgCoinRiseFrames;
        q = -((racer->hop - (d >> 1)) << 12) / d - 0x600;
        racer->y += (s32)((((s64)q << 14) + 0x800) >> 12);
        racer->hop -= 1;
        if (racer->hop == 0)
            racer->state = 6;
    } else if (state == 2) {
        s32 vec[2];
        u8 idx = gActivePlayerSlot;
        if (gTouchHeld[idx * 4] != 0) {
            vec[0] = gTouchX[idx * 4] << 12;
            vec[1] = gTouchY[idx * 4] << 12;
            RouletteRacer_SetTarget(racer, vec);
        } else {
            type = Roulette_TileAt(racer->targetX >> 12, racer->targetY >> 12);
            racer->tile = type;
            if (type != 0x25) {
                type = racer->tile;
                if (Roulette_IsPairTile(type) || Roulette_IsQuadTile(type))
                    RouletteRacer_SetVelocityToward(racer, &sRouletteTiles[type].x, 0x100);
                else
                    RouletteRacer_MoveToTile(racer, type);
                Sound::PlayBank2_2D(0x15e);
            } else {
                Sound::PlayBank2_2D(0x15f);
                RouletteRacer_Release(racer);
            }
            sRouletteLandedRacer = 0;
            racer->state = 1;
        }
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    }
}



int RouletteRacer_CheckLandingA(RouletteRacer *racer) {
    int idx;
    int flag;
    int stride;
    int dx;
    int dy;
    stride = 4;
    if (sRouletteLandedRacer != 0) {
        return 0;
    }
    idx = gActivePlayerSlot;
    flag = 0;
    if (gTouchHeld[idx * 4]) {
        if (gTouchEdge[idx * 4] != 0) {
            flag = 1;
        }
    }
    if (flag == 0) {
        return 0;
    }
    idx = gActivePlayerSlot;
    dx = (racer->x >> 0xc) - gTouchX[idx * stride];
    dy = (racer->y >> 0xc) - gTouchY[idx * 4];
    if ((((dx >= (-8)) && (dx < 8)) && (dy >= (-8))) && (dy < 8)) {
        Sound::PlayBank2_2D(0x15d);
        sRouletteLandedRacer = (void *)racer;
        racer->state = 2;
        racer->grabX = dx << 0xc;
        racer->grabY = dy << 0xc;
        return 1;
    }
    return 0;
}



void RouletteRacer_CheckLandingB(RouletteRacer *racer) {
    int idx;
    int flag;
    int stride;
    int dx;
    int dy;
    int px, py, touchX, touchY;
    s16 tile;
    stride = 4;
    if (sRouletteLandedRacer != 0)
        return;
    idx = gActivePlayerSlot;
    flag = 0;
    if (gTouchHeld[idx * 4]) {
        if (gTouchEdge[idx * 4] != 0) {
            flag = 1;
        }
    }
    if (flag == 0)
        return;
    if (racer->state != 1)
        return;
    idx = gActivePlayerSlot;
    px = racer->x;
    py = racer->y;
    touchX = gTouchX[idx * stride];
    tile = racer->tile;
    px >>= 12;
    touchY = gTouchY[idx * 4];
    py >>= 12;
    dx = px - touchX;
    dy = py - touchY;
    if (tile != 0x25)
        return;
    if (Roulette_TileAt(touchX, touchY) == 0x25)
        return;
    Sound::PlayBank2_2D(0x15d);
    sRouletteLandedRacer = (void *)racer;
    racer->state = 2;
    racer->grabX = dx << 12;
    racer->grabY = dy << 12;
}



void RouletteRacer_Draw(char *racer) {
    RouletteRacer *t = (RouletteRacer *)racer;
    if (t->state == 6)
        return;
    RenderOamBothScreens(sRouletteRacerSprites, t->x >> 0xc, t->y >> 0xc, -1, t->priority,
                         (void *)0);
    t->priority = 1;
}


/* The three range tests are kept as 0 or 1 values held in a local and
   tested; the ROM materialises them. The 0x23 case tests `tile < 6`, the
   opposite of the case beside it, and the two share one `payout = 2`. */

int RouletteRacer_GetPayout(RouletteRacer *racer, int tile) {
    int landed = racer->tile;
    int payout = 0;
    int inRange;

    inRange = (landed <= 0xb) ? 1 : 0;
    if (inRange != 0) {
        if (landed == tile)
            payout = 0xc;
    } else {
        inRange = Roulette_IsPairTile(landed);
        if (inRange != 0) {
            if (tile == sRoulettePairNumbers[landed - 0xc][0] ||
                tile == sRoulettePairNumbers[landed - 0xc][1])
                payout = 6;
        } else {
            inRange = Roulette_IsQuadTile(landed);
            if (inRange != 0) {
                if (tile == sRouletteQuadNumbers[landed - 0x1d][0] ||
                    tile == sRouletteQuadNumbers[landed - 0x1d][1] ||
                    tile == sRouletteQuadNumbers[landed - 0x1d][2] ||
                    tile == sRouletteQuadNumbers[landed - 0x1d][3])
                    payout = 3;
            } else if ((landed == 0x23 && tile < 6) || (landed == 0x24 && tile >= 6)) {
                payout = 2;
            }
        }
    }
    racer->payout = payout;
    return payout;
}



short Roulette_TileAt(int x, int y) {
    short tile = 0x25;

    if (x > sRouletteTileBounds[0] && x <= sRouletteTileBounds[1]) {
        if (y > 0x10 && y <= 0x60) {
            tile = 0x23;
        } else if (y > 0x60 && y <= 0xb0) {
            tile = 0x24;
        }
    } else if (x > sRouletteTileBounds[1] && x <= sRouletteTileBounds[2]) {
        if (y > sRouletteRowEdgesY[0] && y <= sRouletteRowEdgesY[1]) {
            tile = 0;
        } else if (y > sRouletteRowEdgesY[1] && y <= sRouletteRowEdgesY[2]) {
            tile = 0x14;
        } else if (y > sRouletteRowEdgesY[2] && y <= sRouletteRowEdgesY[3]) {
            tile = 3;
        } else if (y > sRouletteRowEdgesY[3] && y <= sRouletteRowEdgesY[4]) {
            tile = 0x17;
        } else if (y > sRouletteRowEdgesY[4] && y <= sRouletteRowEdgesY[5]) {
            tile = 6;
        } else if (y > sRouletteRowEdgesY[5] && y <= sRouletteRowEdgesY[6]) {
            tile = 0x1a;
        } else if (y > sRouletteRowEdgesY[6] && y <= sRouletteRowEdgesY[7]) {
            tile = 9;
        }
    } else if (x > sRouletteTileBounds[2] && x <= sRouletteTileBounds[3]) {
        if (y > sRouletteRowEdgesY[0] && y <= sRouletteRowEdgesY[1]) {
            tile = 0xc;
        } else if (y > sRouletteRowEdgesY[1] && y <= sRouletteRowEdgesY[2]) {
            tile = 0x1d;
        } else if (y > sRouletteRowEdgesY[2] && y <= sRouletteRowEdgesY[3]) {
            tile = 0xe;
        } else if (y > sRouletteRowEdgesY[3] && y <= sRouletteRowEdgesY[4]) {
            tile = 0x1f;
        } else if (y > sRouletteRowEdgesY[4] && y <= sRouletteRowEdgesY[5]) {
            tile = 0x10;
        } else if (y > sRouletteRowEdgesY[5] && y <= sRouletteRowEdgesY[6]) {
            tile = 0x21;
        } else if (y > sRouletteRowEdgesY[6] && y <= sRouletteRowEdgesY[7]) {
            tile = 0x12;
        }
    } else if (x > sRouletteTileBounds[3] && x <= sRouletteTileBounds[4]) {
        if (y > sRouletteRowEdgesY[0] && y <= sRouletteRowEdgesY[1]) {
            tile = 1;
        } else if (y > sRouletteRowEdgesY[1] && y <= sRouletteRowEdgesY[2]) {
            tile = 0x15;
        } else if (y > sRouletteRowEdgesY[2] && y <= sRouletteRowEdgesY[3]) {
            tile = 4;
        } else if (y > sRouletteRowEdgesY[3] && y <= sRouletteRowEdgesY[4]) {
            tile = 0x18;
        } else if (y > sRouletteRowEdgesY[4] && y <= sRouletteRowEdgesY[5]) {
            tile = 7;
        } else if (y > sRouletteRowEdgesY[5] && y <= sRouletteRowEdgesY[6]) {
            tile = 0x1b;
        } else if (y > sRouletteRowEdgesY[6] && y <= sRouletteRowEdgesY[7]) {
            tile = 0xa;
        }
    } else if (x > sRouletteTileBounds[4] && x <= sRouletteTileBounds[5]) {
        if (y > sRouletteRowEdgesY[0] && y <= sRouletteRowEdgesY[1]) {
            tile = 0xd;
        } else if (y > sRouletteRowEdgesY[1] && y <= sRouletteRowEdgesY[2]) {
            tile = 0x1e;
        } else if (y > sRouletteRowEdgesY[2] && y <= sRouletteRowEdgesY[3]) {
            tile = 0xf;
        } else if (y > sRouletteRowEdgesY[3] && y <= sRouletteRowEdgesY[4]) {
            tile = 0x20;
        } else if (y > sRouletteRowEdgesY[4] && y <= sRouletteRowEdgesY[5]) {
            tile = 0x11;
        } else if (y > sRouletteRowEdgesY[5] && y <= sRouletteRowEdgesY[6]) {
            tile = 0x22;
        } else if (y > sRouletteRowEdgesY[6] && y <= sRouletteRowEdgesY[7]) {
            tile = 0x13;
        }
    } else if (x > sRouletteTileBounds[5] && x <= sRouletteTileBounds[6]) {
        if (y > sRouletteRowEdgesY[0] && y <= sRouletteRowEdgesY[1]) {
            tile = 2;
        } else if (y > sRouletteRowEdgesY[1] && y <= sRouletteRowEdgesY[2]) {
            tile = 0x16;
        } else if (y > sRouletteRowEdgesY[2] && y <= sRouletteRowEdgesY[3]) {
            tile = 5;
        } else if (y > sRouletteRowEdgesY[3] && y <= sRouletteRowEdgesY[4]) {
            tile = 0x19;
        } else if (y > sRouletteRowEdgesY[4] && y <= sRouletteRowEdgesY[5]) {
            tile = 8;
        } else if (y > sRouletteRowEdgesY[5] && y <= sRouletteRowEdgesY[6]) {
            tile = 0x1c;
        } else if (y > sRouletteRowEdgesY[6] && y <= sRouletteRowEdgesY[7]) {
            tile = 0xb;
        }
    }
    return tile;
}



int RouletteBoard_LoadModels(char *slider) {
    RouletteBoard *board = (RouletteBoard *)slider;
    board->mWheelFile = (BMD_File *)Ov004_LoadFile(0x21d);
    board->mBallFile = (BMD_File *)Ov004_LoadFile(0x21e);
    dExtSimpleModel_c::UpdateFileOffsets(*board->mWheelFile);
    dExtSimpleModel_c::UpdateFileOffsets(*board->mBallFile);
    if (board->mWheel.SetFile(board->mWheelFile, 1, -1) == 0)
        return 0;
    if (board->mBall.SetFile(board->mBallFile, 1, -1) == 0)
        return 0;
    board->mRollSound = 0;
    board->mRollSoundHandle = 0;
    return 1;
}



int RouletteBoard_FreeModels(RouletteBoard *res) {
    if (res->mWheelFile)
        Ov004_Deallocate(res->mWheelFile);
    if (res->mBallFile)
        Ov004_Deallocate(res->mBallFile);
    return 1;
}



void RouletteBoard_StartSpin(char *slider) {
    RouletteBoard *board = (RouletteBoard *)slider;
    unsigned rand;
    board->mBallRadius = 0x4200;
    board->mBallRadiusVel = 0;
    board->mOrbitAngle = 0;
    board->mBoardAngle = 0;
    board->mAngleStep = 0x400;
    board->mBoardSpin = -0x200;
    rand = (unsigned)RandomIntInternal(&data_0209e650.value);
    board->mSpinSpeed = ((rand & 0x7fffffff) >> 0x13) + 0x2000;
    board->mBraking = 0;
    RouletteBoard_SetStateSpin(board);
}



void RouletteBoard_SetStateSpin(RouletteBoard *board) {
    *(Pair2W *)&board->mState = pmf_RouletteBoard_StateSpin;
}



void RouletteBoard_StateSpin(RouletteBoard *board) {
    int spin = board->mSpinSpeed;
    int pull = FX_Div(FX_MUL(spin, spin), board->mBallRadius) - 0xc00;

    spin = board->mSpinSpeed;
    int scaled = FX_MUL(spin, 0x500);
    int step = FX_Div(scaled, board->mBallRadius);
    board->mAngleStep = (s16)step;

    board->mOrbitAngle += board->mAngleStep;

    board->mBallRadiusVel = (int)(((s64)board->mBallRadiusVel * 0xfe0 + 0x800) >> 12);

    board->mBallRadiusVel += pull;
    board->mBallRadius += board->mBallRadiusVel;

    int radius = board->mBallRadius;
    if (radius > 0x4200) {
        radius = 0x4200;
        board->mBallRadius = radius;
        board->mBallRadiusVel = 0;
    } else if (radius < 0x2c00) {
        ApproachLinear(board->mSpinSpeed, 0xfffff667, 0x28);
        RouletteBoard_StartSlowdown(board);
    } else {
        if (board->mRollSound == 1) {
            board->mRollSoundHandle = 0;
            board->mRollSound = 2;
        }
    }

    if (board->mBraking != 0) {
        ApproachLinear(board->mSpinSpeed, 0, 0x10);
    }

    u16 angle = (u16)board->mOrbitAngle;
    int dist = board->mBallRadius;
    int idx = angle >> 4;
    s16 val1 = FX_CosIdx(idx);
    s16 val0 = FX_SinIdx(idx);

    Vector3 ballPos;
    ballPos.x = FX_MUL(dist, val1);
    ballPos.y = FX_MUL(dist, 0x400) + 0x780;
    ballPos.z = FX_MUL(dist, val0);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ballPos.x, ballPos.y, ballPos.z);

    board->mBall.mat4x3 = data_020a0e68;
}



void RouletteBoard_StartSlowdown(RouletteBoard *board) {
    board->mSpinSpeed += 0x140;
    board->mSlowBias = -board->mBoardAngle;
    *(struct Pair *)&board->mState = pmf_RouletteBoard_StateSlow;
}



void RouletteBoard_StateSlow(RouletteBoard *board) {
    int spinAbs;
    int pull;
    short sum;
    ShortTbl12 stops;
    short diff;
    short idx;
    int extra;
    int angidx;
    short t0, t1;
    int x, y, z;
    Vector3 ballPos;

    spinAbs = board->mSpinSpeed - 0x140;
    if (spinAbs < 0)
        spinAbs = -spinAbs;
    pull = FX_Div(FX_MUL(spinAbs, spinAbs), board->mBallRadius) - 0xc00;

    board->mAngleStep = FX_Div(FX_MUL(board->mSpinSpeed, 0x500), board->mBallRadius);

    board->mOrbitAngle += board->mAngleStep;

    board->mBallRadiusVel = FX_MUL(board->mBallRadiusVel, 0x600);
    board->mBallRadiusVel += pull;
    board->mBallRadius += board->mBallRadiusVel;

    if (board->mBallRadius > 0x2c00) {
        board->mBallRadius = 0x2c00;
        board->mBallRadiusVel = 0;
    } else if (board->mBallRadius < 0x1600) {
        board->mBallRadius = 0x1600;
        board->mBallRadiusVel = 0;
    }

    sum = board->mOrbitAngle + board->mSlowBias;

    stops = *(ShortTbl12 *)sRouletteSlowStops;

    for (idx = 0; idx < 12; idx++) {
        short ad;
        diff = sum - stops.v[idx];
        ad = diff < 0 ? (short)(-diff) : diff;
        if (ad < 0xaab)
            break;
    }

    board->mStopTile = sRouletteStopTiles[idx];

    extra = 0;
    if (board->mBallRadius < 0x2000) {
        if (diff < -0x400 && (diff - board->mAngleStep) >= -0x400) {
            board->mOrbitAngle = (stops.v[idx] - 0x400) - board->mSlowBias;
            board->mSpinSpeed = -FX_MUL(board->mSpinSpeed, 0xf00);
            ApproachLinear(board->mSpinSpeed, 0xfffff667, 0x30);
            if (board->mRollSound != 0) {
                Sound::PlayBank2_2D(0x159);
                board->mRollSound = 0;
            }
        } else if (diff > 0x400 && (diff - board->mAngleStep) <= 0x400) {
            board->mOrbitAngle = (stops.v[idx] + 0x400) - board->mSlowBias;
            board->mSpinSpeed = -FX_MUL(board->mSpinSpeed, 0xe00);
            if (board->mRollSound != 0) {
                Sound::PlayBank2_2D(0x159);
                board->mRollSound = 0;
            }
        } else {
            if (diff < 0)
                diff = -diff;
            if (diff > 0x400) {
                board->mBallRadius = 0x2000;
                board->mBallRadiusVel = 0;
            }
        }
        extra = FX_MUL(board->mBallRadius - 0x2000, 0x100);
    }

    ApproachLinear(board->mSpinSpeed, 0, 0x30);

    angidx = ((unsigned short)(sum + board->mBoardAngle)) >> 4;
    t0 = FX_CosIdx(angidx);
    t1 = FX_SinIdx(angidx);

    x = FX_MUL(board->mBallRadius, t0);
    y = FX_MUL(board->mBallRadius, 0x400) + 0x780 + extra;
    z = FX_MUL(board->mBallRadius, t1);
    ballPos.x = x;
    ballPos.y = y;
    ballPos.z = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ballPos.x, ballPos.y, ballPos.z);

    board->mBall.mat4x3 = data_020a0e68;
}


 void RouletteBoard_Update(RouletteBoard *c) {
    c->mBoardAngle += c->mBoardSpin;
    c->mWheel.data.bones[2].rotX = -c->mBoardAngle;
    c->mWheel.UpdateVerts();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    c->mWheel.mat4x3 = data_020a0e68;
    (c->*(c->mState))();
    {
        short s = c->mRollSound;
        if (s == 0)
            return;
        if (s == 2)
            c->mRollSoundHandle = Sound_PlayIfNotActive(c->mRollSoundHandle, 2, 0x158, 0);
        else
            c->mRollSoundHandle = Sound_PlayIfNotActive(c->mRollSoundHandle, 2, 0x157, 0);
    }
}

 void RouletteBoard_Draw(char *slider) {
    RouletteBoard *board = (RouletteBoard *)slider;
    board->mWheel.Render(0);
    board->mBall.Render(0);
}


void RouletteStrip_Reset(RouletteStrip *strip) {
    int i;
    for (i = 0; i < 5; i++)
        strip->racers[i] = 0;
    strip->payoutIndex = 4;
    strip->count = 0;
}



void RouletteStrip_AddRacer(RouletteStrip *strip, RouletteRacer *racer) {
    int i;
    strip->count += 1;
    for (i = 0; i < 5; i++) {
        if (strip->racers[i] == 0) {
            strip->racers[i] = racer;
            return;
        }
    }
}



void RouletteStrip_Draw(RouletteStrip *strip, int y) {
    int idx = GetGameLanguage();
    void *p = sRouletteTextSpritesByLanguage[idx][20];
    Hud_RenderSprite(p, 0xc, y, -1, -1);

    Mg_DrawBigNumber(0x1e, y, sRoulettePayoutMultipliers[strip->payoutIndex], 0, -1, 2, 0);

    {
        int b = y + 0x10;
        RenderOamBothScreens(sRouletteRacerSprites, 0x20, b, -1, 1, (void *)0);
        Mg_DrawBigNumber(0x2e, b, strip->count, 0, -1, 1, 0);
    }
}



void RouletteStrip_DrawRacers(RouletteStrip *strip) {
    int i;
    for (i = 0; i < 5; i++) {
        RouletteRacer *racer = strip->racers[i];
        int tile, frame, payoutIndex, x, y;
        if (racer == 0)
            return;
        racer->priority = 0;
        payoutIndex = strip->payoutIndex;
        tile = racer->tile;
        frame = sRouletteStripSpriteBase[payoutIndex];
        if (data_020a0db0 & 8)
            frame++;
        if (tile >= 0xc && tile <= 0x13)
            frame += 2;
        x = sRouletteTiles[tile].x >> 12;
        y = sRouletteTiles[tile].y >> 12;
        if (payoutIndex == 3)
            x += 0x58;
        Hud_RenderSprite(sRouletteStripSprites[frame], x, y, -1, -1);
    }
}



void RouletteStrip_LinkAll(char *p) {
    int i;
    for (i = 0; i < 4; i++) {
        sRouletteStrips[i] = (RouletteStrip *)p;
        p += 0x18;
    }
}



void RouletteStrip_ResetAll(void) {
    int i;
    for (i = 0; i < 4; i++) {
        RouletteStrip_Reset(sRouletteStrips[i]);
        sRouletteStrips[i]->payoutIndex = (short)i;
    }
    sRouletteResultIndex = -1;
    sRouletteResultsTimer = 0;
}


 int RouletteResults_Tick(void) {
    if (ApproachLinear(sRouletteResultsTimer, 0, 1) != 0) {
        int idx;
        sRouletteResultIndex = sRouletteResultIndex + 1;
        idx = sRouletteResultIndex;
        sRouletteResultsTimer = 0x78;
        while (idx < 4) {
            if (sRouletteStrips[idx]->count != 0)
                break;
            sRouletteResultIndex = sRouletteResultIndex + 1;
            idx = sRouletteResultIndex;
        }
        if (idx < 4) {
            func_02012790(0x26);
        }
    }
    return (sRouletteResultIndex >= 4) ? 1 : 0;
}


void RouletteResults_Draw(void) {
    int n = sRouletteResultIndex;
    int y;
    int i;
    if (n < 0)
        return;
    n = n + 1;
    if (n > 4)
        n = 4;
    y = 0x28;
    for (i = 0; i < n; i++) {
        RouletteStrip *strip = sRouletteStrips[i];
        if (strip->count > 0) {
            RouletteStrip_Draw(strip, y);
            y += 0x24;
        }
    }
    if (sRouletteResultIndex >= 4)
        return;
    RouletteStrip_DrawRacers(sRouletteStrips[sRouletteResultIndex]);
}


/* Empty, like the constructor; out of line because __cxa_vec_cleanup takes
   its address. */
RouletteRacer::~RouletteRacer() {}

/* The destructor pair, D1 at 0x0210788c and D0 at 0x02107920, is not
   defined here. It is declared inline as the class's first member in
   dScMgRoulette_c.h: that is the only form that makes the compiler emit D1
   below D0 with no D2, as the cartridge has them. */
