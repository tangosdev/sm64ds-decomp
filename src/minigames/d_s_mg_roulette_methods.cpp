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
// - The camera at 0x4660 stays an offset: it is the head of the base's
//   pad_4660, which names nothing before mCameraEyeX.
// - Behavior keeps two raw spellings, each with its measured reason beside
//   it: the four mMeter strips it adds racers to, and the reload of a
//   racer's tile in phase 2.
// - The RouletteBoard_, RouletteRacer_ and RouletteStrip_ helpers are
//   still free functions over their record (most take a char* or int*),
//   and the racer helpers keep the partial views RacerXY and RacerPos they
//   were matched with. mMeter stays bytes, reached through the local
//   RouletteStrip view. Most func_ and data_ helpers are unnamed in
//   symbols.txt.
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
#include "types.h"
#include "dScMgRoulette_c.h"
#include "dExtSimpleModel_c.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"

/* Local views of the records the helpers walk. */

/* Three consecutive Fix12 words, written a component at a time. */
struct Vec3 {
    int x, y, z;
};

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

/* The board's tile centres: sRouletteTiles holds x and
   sRouletteTilePos the same array offset to y, stride 8. decl_common.h
   types both as `char[]`, so every read casts. */
struct TileXY {
    int x, y;
};

/* RacerXY and RacerPos are two partial views of RouletteRacer
   (dScMgRoulette_c.h); each helper keeps the view it was matched with. */
struct RacerXY {
    int pad[4];
    int stepX;
    int stepY;
    int targetX;
    int targetY;
};

struct RacerPos {
    char pad[0x18];
    int f18;
    int f1c;
    char pad2[0x2c - 0x20];
    short f2c;
};

typedef struct {
    int x, y;
} Vec2_Fix12;

/* One payout strip, 0x18 bytes. The scene's mMeter holds four, linked into
   sRouletteStrips; each collects the racers paid at one multiplier. */
struct RouletteStrip {
    RouletteRacer *racers[5]; /* 0x00, filled in order; 0 ends the list */
    s16 payoutIndex;  /* 0x14, sRoulettePayoutMultipliers index; ResetAll
                         gives strip i index i */
    s16 count;        /* 0x16, racers added this round */
};
typedef char RouletteStrip_size_must_be_0x18[sizeof(RouletteStrip) == 0x18 ? 1 : -1];

/* The scene seen from its racers. Behavior's racer loops step a copy of
   `this` by one racer (0x34 bytes) and read racers[0] off it, which is how
   the ROM addresses them: the state at p + 0x51da. Indexing mRacers[i]
   instead keeps Behavior's size but changes its registers. */
typedef struct RacerScene {
    char _p0[0x51a8];
    RouletteRacer racers[5]; /* 0x51a8, the scene's mRacers */
} RacerScene;

/* A value launder: the round trip through long long keeps the compiler
   from folding the expression into its use, which the ROM does not do.
   Behavior's deal position needs it; without it Behavior shrinks from
   0x6e8 to 0x6e4 bytes. */
#define LNDR(e) ((int)(((long long)(e))))
/* A raw short of the scene, for unk_53f4: phase 3's clear is its only
   access in this unit, so there is nothing to name it from. */
#define H(o) (*(short*)(c + (o)))
/* The active minigame scene's mCredits, or 0 with no scene. */
#define SCENE_BONUS ((gMgScene != 0) \
                        ? gMgScene->mCredits : 0)


namespace cstd {
int fdiv(int a, int b);
}

namespace G2S {
int GetBG2CharPtr();
int GetBG2ScrPtr();
int GetBG0ScrPtr();
}

namespace GX {
void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}

namespace GXS {
void LoadBGPltt(const void *data, unsigned int offset, unsigned int size);
void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}


extern "C" {

int GetGameLanguage(void);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
void Vec2_Sub(int *out, int *a, int *b);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int Sound_PlayIfNotActive(int a, int b, int c, int d);
int RandomIntInternal(int *seed);
void *LoadFile(int id);
void DecompressLZ16(void *src, int dst);
void Ov004_Deallocate(void *p);
void FreeGfxSlotsById(int a);
/* local extern: declared only in the sibling scene headers (dScMgAmida_c.h,
   dScMgSmartball_c.h, ...), none of which this TU includes. */
void NullDestructor_0203d47c(void);
void Camera_UpdateMatrices(char *cam);

void func_020731dc(void *object, void *destructor, void **node);
void func_0203d680(Vec2_Fix12 *out, const Vec2_Fix12 *in, int scale);
void *Ov004_LoadFile(int id);
void Mg_LoseCredits(int c);
void Mg_GainCredits(int c);
void Mg_DrawCredits(char *, int, int, int);
void Mg_DrawNumberRotated(int, int, int, int, int, int, int);
void MgCoins_StartPayout(void);
void MgCoins_Update(void);
void MgCoins_Reset(void);
void Camera_InitDealerView(char *c);
int Dealer_IsAnimDone(char *a);
int Dealer_IsIdle(char *c);
void Dealer_Draw(char *c);
void Dealer_Update(char *thiz);
int Dealer_Load(char *c);

extern int sRouletteResultsTimer;
extern int sRouletteResultIndex;
extern char *sRouletteStrips[];
extern int data_020a0db0;
extern s32 data_ov006_0212ed24[];
extern void *sRouletteStripSprites[];
extern void *sRouletteTextSpritesByLanguage[];
extern int sRoulettePayoutMultipliers[];
extern void *sRouletteRacerSprites;
extern Matrix4x3 data_020a0e68;
extern short data_02082214[];
extern short sRouletteSlowStops[];
extern short data_ov006_0212ed34[];
extern struct Pair pmf_RouletteBoard_StateSlow;
extern struct Pair2W pmf_RouletteBoard_StateSpin;
extern int data_0209e650[];
extern int sRouletteTileBounds[];
extern int sRouletteRowEdgesY[];
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];
extern int sRouletteRacerGoalGuard;
extern int sRouletteRacerGoal[];
extern void *sRouletteRacerGoalDtorRec;
extern s32 sMgCoinRiseFrames;
extern u16 gMgCoinsStatus;
extern u8 data_0209d45c;
extern u8 data_0209d454;

/* This file's helpers. The file runs from the highest address down, so
   most calls between them are forward references. */
void RouletteResults_Draw(void);
void RouletteStrip_AddRacer(int *p, int val);
void RouletteStrip_Reset(int *p);
void RouletteBoard_Update(RouletteBoard *c);
void RouletteBoard_StateSlow(RouletteBoard *board);
void RouletteBoard_StartSlowdown(RouletteBoard *board);
void RouletteBoard_StateSpin(RouletteBoard *board);
void RouletteBoard_SetStateSpin(RouletteBoard *board);
int RouletteBoard_FreeModels(RouletteBoard *o);
short Roulette_TileAt(int x, int y);
int RouletteRacer_GetPayout(short *r0, int r1);
void RouletteRacer_CheckLandingB(int *p);
int RouletteRacer_CheckLandingA(int *p);
void RouletteRacer_Update(RouletteRacer *t);
void RouletteRacer_Release(RouletteRacer *racer);
void RouletteRacer_Init(void *self);
void RouletteRacer_MoveToTile(RacerPos *self, int idx);
void RouletteRacer_SetTarget(RacerXY *self, int *src);
void RouletteRacer_SetVelocityToward(int *out, int *a, int scale);
void RouletteRacer_AimAt(int *a, int *b);
void RouletteStrip_DrawRacers(void **sl);
void RouletteStrip_Draw(short *a0, int a1);
}

// @symbol _ZN15dScMgRoulette_c13InitResourcesEv
/* Sets up both screens: the sub BG layers, the board tiles, map and
   palette, and the shared OBJ tiles and palettes for both engines. Then
   sets up the camera, the dealer and the board and starts the first round. */
s32 dScMgRoulette_c::InitResources() {
    char *raw = (char *)this;
    void *bgChars, *bgMap, *f6, *f5;

    Mg_SetTopScreenYOffset(0x20);

    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x404;
    *(volatile u16 *)0x04001008 = (*(volatile u16 *)0x04001008 & 0x43) | 0x504;

    bgChars = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NCG_BIN);
    bgMap = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NSC_BIN);
    f6 = LoadFile(ASSET_D_2D_YAKU_NSC_BIN);
    f5 = LoadFile(ASSET_D_2D_MG_BG_ROULETTE_NCL_BIN);
    DecompressLZ16(bgChars, G2S::GetBG2CharPtr());
    DecompressLZ16(bgMap, G2S::GetBG2ScrPtr());
    DecompressLZ16(f6, G2S::GetBG0ScrPtr());
    GXS::LoadBGPltt(f5, 0x1a0, 0x60);
    Deallocate(bgChars);
    Deallocate(bgMap);
    Deallocate(f6);
    Deallocate(f5);

    {
        volatile u16 *a = (volatile u16 *)0x04001008;
        *a = *a & ~0x40;
        *(volatile u16 *)0x0400100c = *(volatile u16 *)0x0400100c & ~0x40;
        SetSubBg0Offset(0, 0);
        SetSubBg2Offset(0, 0);
        *a = (*a & ~3) | 2;
        *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & ~3) | 2;
    }

    f6 = LoadFile(ASSET_D_2D_MG_ROULETTE_NCG_BIN);
    f5 = LoadFile(ASSET_D_2D_MG_ROULETTE_NCL_BIN);
    DecompressLZ16(f6, 0x6400000);
    DecompressLZ16(f6, 0x6600000);
    GX::LoadOBJPltt(f5, 0, 0x100);
    GXS::LoadOBJPltt(f5, 0, 0x100);
    Deallocate(f6);
    Deallocate(f5);

    data_0209d45c = 0x11;
    data_0209d454 = 0x15;
    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    Camera_InitDealerView(raw + 0x4660);
    if (Dealer_Load((char *)&mDealer) == 0)
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

// @symbol _ZN15dScMgRoulette_c8BehaviorEv
/* One frame of the round, by mPhase: 1 deals the racers out one at a time,
   2 runs the countdown and scores the board, 3 pays out per landed tile and
   4 announces the result. The racer update loops run every frame. */
s32 dScMgRoulette_c::Behavior() {
    char *c = (char *)this;

    switch (mPhase) {
    case 0:
        break;
    case 1:
        if (Dealer_IsAnimDone((char *)&mDealer) != 0) {
            if (mBoard.mRollSound == 0) {
                mBoard.mRollSound = 1;
            }
        }
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (Dealer_IsAnimDone((char *)&mDealer) != 0) {
            mPhaseTimer -= 1;
            if (mPhaseTimer == 0) {
                mCameraPreset = 1;
                if (mDealIndex < mRacerCount) {
                    int dealPos[2];
                    int dealY;
                    dealY = LNDR(0 - (mDealIndex << 16)) + 0x80000;
                    dealPos[0] = 0x20000;
                    dealPos[1] = dealY;
                    RouletteRacer_AimAt((int *)&mRacers[mDealIndex], dealPos);
                    mDealIndex += 1;
                    mPhaseTimer = 8;
                    Mg_LoseCredits(1);
                    Sound::PlayBank2_2D(0x14d);
                } else {
                    int i = 0;
                    if (mRacerCount > 0) {
                        char *p = c;
                        do {
                            ((RacerScene *)p)->racers[0].state = 1;
                            i++;
                            p += 0x34;
                        } while (i < mRacerCount);
                    }
                    mPhaseTimer = 0x258;
                    Dealer_PlayIdle((char *)&mDealer);
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
                int onTile;
                int i = 0;
                if (mRacerCount > 0) {
                    char *racerRaw = c;
                    char *racer = (char *)mRacers;
                    do {
                        ((RacerScene *)racerRaw)->racers[0].tile =
                            Roulette_TileAt(((RacerScene *)racerRaw)->racers[0].targetX >> 12,
                                            ((RacerScene *)racerRaw)->racers[0].targetY >> 12);
                        onTile = 0;
                        /* racers[0].tile, read back raw: through the member the
                           compiler reuses the value it just stored and Behavior
                           grows from 0x6e8 to 0x6f0 bytes; the ROM reloads it. */
                        tileType = *(short *)(racerRaw + 0x51d4);
                        if (tileType >= 0xc && tileType <= 0x1c)
                            onTile = 1;
                        if (onTile != 0)
                            goto landedInRange;
                        if (tileType >= 0x1d && tileType <= 0x22)
                            onTile = 1;
                        else
                            onTile = 0;
                        if (onTile == 0)
                            goto landedElsewhere;
                    landedInRange:
                        RouletteRacer_SetVelocityToward(
                            (int *)racer, (int *)&((TileXY *)sRouletteTiles)[tileType].x, 0x100);
                        goto nextRacer;
                    landedElsewhere:
                        RouletteRacer_MoveToTile((RacerPos *)racer, tileType);
                    nextRacer:
                        i++;
                        racerRaw += 0x34;
                        racer += 0x34;
                    } while (i < mRacerCount);
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
                int j = 0;
                if (mRacerCount > 0) {
                    char *rowRaw = c;
                    char *row = (char *)mRacers;
                    do {
                        if (((RacerScene *)rowRaw)->racers[0].tile == 0x25) {
                            RouletteRacer_Init(row);
                            if (sounded == 0) {
                                Sound::PlayBank2_2D(0x15c);
                                sounded = 1;
                            }
                        } else {
                            ((RacerScene *)rowRaw)->racers[0].state = 3;
                            mTargetScore += 1;
                        }
                        j++;
                        rowRaw += 0x34;
                        row += 0x34;
                    } while (j < mRacerCount);
                }
                Mg_SaveRecordPair(mRacerCount - mTargetScore + SCENE_BONUS, 0);
            }
        }
        break;
    }
    case 3: {
        int stopped;
        short wait;
        int tile;
        int j;
        int payout;
        char *p;
        stopped = (mBoard.mSpinSpeed == 0) ? 1 : 0;
        if (stopped != 0) {
            j = 0;
            mPromptEnabled = 0;
            wait = mPhaseTimer;
            mPhaseTimer -= 1;
            if (wait == 0) {
                tile = mBoard.mStopTile;
                mScore = 0;
                if (mRacerCount > 0) {
                    p = (char *)mRacers;
                    do {
                        payout = RouletteRacer_GetPayout((short *)p, tile);
                        /* mMeter[3..0], the strips for payouts 2, 3, 6 and 12.
                           Spelled as mMeter[n] or through a typed view of the
                           strips, Behavior keeps its 0x6e8 bytes but its
                           registers change. */
                        if (payout == 2) {
                            RouletteStrip_AddRacer((int *)(c + 0x52f4), (int)p);
                        } else if (payout == 3) {
                            RouletteStrip_AddRacer((int *)(c + 0x52dc), (int)p);
                        } else if (payout == 6) {
                            RouletteStrip_AddRacer((int *)(c + 0x52c4), (int)p);
                        } else if (payout == 0xc) {
                            RouletteStrip_AddRacer((int *)(c + 0x52ac), (int)p);
                        }
                        mScore += payout;
                        j++;
                        p += 0x34;
                    } while (j < mRacerCount);
                }
                Mg_SaveRecordPair(mScore + SCENE_BONUS, 0);
                H(0x53f4) = 0;
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
                Dealer_PlayWin(&mDealer);
                Mg_SetSharedState(MG_STATE_WIN);
            } else if (score < target) {
                Dealer_PlayLose(&mDealer);
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

    {
        int n = 0;
        if (mRacerCount > 0) {
            char *p = (char *)mRacers;
            do {
                RouletteRacer_Update((RouletteRacer *)p);
                n++;
                p += 0x34;
            } while (n < mRacerCount);
        }
    }
    {
        int n = 0;
        if (mRacerCount > 0) {
            char *p = (char *)mRacers;
            do {
                RouletteRacer_CheckLandingB((int *)p);
                n++;
                p += 0x34;
            } while (n < mRacerCount);
        }
    }
    RouletteBoard_Update(&mBoard);
    Dealer_Update((char *)&mDealer);
    MgCoins_Update();
    return 1;
}

// @symbol Roulette_DrawResultsThunk
/* A tail-call veneer to RouletteResults_Draw. */
extern "C" {
void Roulette_DrawResultsThunk(char *) {
    RouletteResults_Draw();
}
}

// @symbol _ZN15dScMgRoulette_c6RenderEv
/* Draws the racers back to front, the cursor on the selected tile, the
   countdown (its volume ramps over the last 3 seconds), and moves the camera
   to the current preset. */
s32 dScMgRoulette_c::Render() {
    char *raw = (char *)this;
    Mg_DrawCredits(raw, 0xc, 0xc, 0);
    MgCoins_Draw();

    {
        int i = mRacerCount - 1;
        for (; i >= 0; i--)
            RouletteRacer_Draw((char *)&((RacerScene *)raw)->racers[i]);
    }

    Roulette_DrawResultsThunk(raw);

    {
        int idle = (int)(((long long)(mBoard.mSpinSpeed == 0)));
        if (idle != 0) {
            if (mPhase < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = mBoard.mStopTile;
                    Hud_RenderSprite((void *)data_ov006_02138c18,
                                     ((TileXY *)sRouletteTiles)[idx].x >> 12,
                                     ((TileXY *)sRouletteTilePos)[idx].x >> 12, /* = [idx].y */
                                     -1, 1);
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
            Mg_DrawNumberRotated(0x80, 0x60, seconds, 1, -1, func_02053200(vol), 0);
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
                mCameraTargetX = a1;
                mCameraTargetY = b1;
                mCameraTargetZ = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                mCameraEyeX = d2;
                mCameraEyeY = e2;
                mCameraEyeZ = f2;
            }
            mCameraAngle = angle;
            Camera_UpdateMatrices(raw + 0x4660);
        } else {
            Camera_InitDealerView(raw + 0x4660);
        }
        if (preset != 0)
            RouletteBoard_Draw((char *)&mBoard);
        else
            Dealer_Draw((char *)&mDealer);
    }

    return 1;
}

// @symbol _ZN15dScMgRoulette_c16CleanupResourcesEv
/* Frees the slider's two model files. */
s32 dScMgRoulette_c::CleanupResources() {
    RouletteBoard_FreeModels(&mBoard);
    return 1;
}

// @symbol _ZN15dScMgRoulette_c11CanEndStateEi
/* Runs the end of the round, phases 5 to 9. Returns 1 when it is done. */
int dScMgRoulette_c::CanEndState(int /* mode */) {

    switch (mPhase) {
    case 5:
        FreeGfxSlotsById(0x1d);
        if (RouletteResults_Tick() != 0)
            (mPhase)++;
        break;
    case 6:
        if (Dealer_IsIdle((char *)&mDealer) != 0) {
            mPhaseTimer = 0x3c;
            mDealIndex = 0;
            (mPhase)++;
        }
        break;
    case 7:
        (mPhaseTimer)--;
        if (mPhaseTimer == 0) {
            if (mScore != 0)
                MgCoins_StartPayout();
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

// @symbol _ZN15dScMgRoulette_c10StartRoundEi
/* Resets the round: the dealer, the racers, the score and the board, with
   one racer per bonus the scene holds, up to five. InitResources calls it
   with -1. */
void dScMgRoulette_c::StartRound(int /* arg */) {

    int i;
    RouletteRacer *racer;
    int count;

    RouletteStrip_ResetAll();
    Dealer_StartSwap((int *)&mDealer);

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

// @symbol RouletteRacer_AimAt
extern "C" {
void RouletteRacer_AimAt(int *racer, int *target) {
    ((RouletteRacer *)racer)->homeX = target[0];
    ((RouletteRacer *)racer)->homeY = target[1];
    RouletteRacer_SetVelocityToward(racer, target, 0x80);
}
}

// @symbol RouletteRacer_SetVelocityToward
extern "C" {
void RouletteRacer_SetVelocityToward(int *out, int *target, int scale) {
    Vec2_Fix12 v1;
    Vec2_Fix12 v2;
    out[6] = target[0];
    out[7] = target[1];
    Vec2_Sub((int *)&v1, out + 6, out);
    func_0203d680(&v2, &v1, scale);
    out[4] = v2.x;
    out[5] = v2.y;
    if (out[4] < 0)
        out[4] = -out[4];
    if (out[5] < 0)
        out[5] = -out[5];
}
}

// @symbol RouletteRacer_SetTarget
extern "C" {
void RouletteRacer_SetTarget(RacerXY *racer, int *target) {
    int v[2];
    racer->targetX = target[0];
    racer->targetY = target[1];
    Vec2_Sub(v, &racer->targetX, (int *)racer);
    racer->stepX = v[0];
    racer->stepY = v[1];
    if (func_0203d434(&racer->stepX)) {
        func_0203d630(&racer->stepX, 0x18000);
    }
    if (racer->stepX < 0)
        racer->stepX = -racer->stepX;
    if (racer->stepY < 0)
        racer->stepY = -racer->stepY;
}
}

// @symbol RouletteRacer_MoveToTile
extern "C" {
void RouletteRacer_MoveToTile(RacerPos *racer, int tile) {
    int buf[7];
    int f18 = racer->f18;
    int a, b, lr, t, hi;
    buf[0] = f18;
    buf[1] = racer->f1c;
    if ((unsigned short)(short)(tile - 0x23) <= 1) {
        a = ((TileXY *)sRouletteTiles)[tile].x;
        b = ((TileXY *)sRouletteTilePos)[tile].x;
        buf[2] = a;
        buf[3] = b;
        lr = a + 0x8000;
        hi = a + 0x18000;
        if (f18 >= lr) {
            if (f18 <= hi)
                hi = f18;
            lr = hi;
        }
        b = buf[3];
        t = buf[1];
        {
            int lo = b - 0x20000;
            buf[0] = lr;
            lr = lo;
            hi = b + 0x20000;
            if (t >= lo) {
                if (t <= hi)
                    hi = t;
                lr = hi;
            }
        }
        buf[1] = lr;
        RouletteRacer_SetVelocityToward((int *)racer, buf, 0x100);
        return;
    }
    {
        int early = (racer->f2c <= 0xb) ? 1 : 0;
        if (early == 0)
            return;
    }
    a = ((TileXY *)sRouletteTiles)[tile].x;
    b = ((TileXY *)sRouletteTilePos)[tile].x;
    buf[4] = a;
    buf[5] = b;
    lr = a - 0x10000;
    hi = a + 0x10000;
    if (buf[0] >= lr) {
        if (buf[0] <= hi)
            hi = buf[0];
        lr = hi;
    }
    b = buf[5];
    t = buf[1];
    buf[0] = lr;
    lr = b - 0xc000;
    hi = b + 0xc000;
    if (t >= lr) {
        if (t <= hi)
            hi = t;
        lr = hi;
    }
    buf[1] = lr;
    RouletteRacer_SetVelocityToward((int *)racer, buf, 0x100);
}
}

// @symbol RouletteRacer_Init
/* sRouletteRacerGoal is a function-local `static Vector2 goal(0xc000, 0xc000)`
   spelled out by hand. Written that way it is byte-identical (measured
   2026-10-25), but the compiler then emits the object, its guard and its
   registration node as this TU's own .bss, and this text-only TU owns none:
   the three sit at 0x021428cc/0x021428d4/0x02142948, among the
   __sinit_ov006_021322bc globals' nodes. */
extern "C" {
void RouletteRacer_Init(void *racer) {
    int b = sRouletteRacerGoalGuard & 1;
    if (b == 0) {
        sRouletteRacerGoal[0] = 0xc000;
        sRouletteRacerGoal[1] = 0xc000;
        func_020731dc(sRouletteRacerGoal, (void *)NullDestructor_0203d47c,
                      &sRouletteRacerGoalDtorRec);
        sRouletteRacerGoalGuard |= 1;
    }
    RouletteRacer_SetVelocityToward((int *)racer, sRouletteRacerGoal, 0x80);
    ((RouletteRacer *)racer)->state = 7;
}
}

// @symbol RouletteRacer_PlaceAtStart
extern "C" {
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
}

// @symbol RouletteRacer_Release
extern "C" {
void RouletteRacer_Release(RouletteRacer *racer) {
    racer->tile = 0x25;
    racer->state = 1;
    RouletteRacer_SetVelocityToward((int *)racer, &racer->homeX, 0x100);
}
}

// @symbol RouletteRacer_Update
extern "C" {
void RouletteRacer_Update(RouletteRacer *racer) {
    u8 state;
    int b;
    s16 type;

    state = racer->state;
    if (state == 0 || state == 3) {
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    }

    state = racer->state;
    if (state == 1) {
        RouletteRacer_CheckLandingA((int *)racer);
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    } else if (state == 7) {
        s32 out[2];
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
        Vec2_Sub(out, &racer->targetX, (int *)racer);
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
        Vec2_Sub(out, &racer->targetX, (int *)racer);
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
            RouletteRacer_SetTarget((RacerXY *)racer, vec);
        } else {
            type = Roulette_TileAt(racer->targetX >> 12, racer->targetY >> 12);
            racer->tile = type;
            if (type != 0x25) {
                b = 0;
                type = racer->tile;
                if (type >= 0xc && type <= 0x1c)
                    b = 1;
                if (b != false)
                    goto use_table;
                if (type >= 0x1d && type <= 0x22)
                    b = 1;
                else
                    b = 0;
                if (b == false)
                    goto no_table;
            use_table:
                RouletteRacer_SetVelocityToward((int *)racer, (int *)&sRouletteTiles[type * 8],
                                                0x100);
                goto snd;
            no_table:
                RouletteRacer_MoveToTile((RacerPos *)racer, type);
            snd:
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
}

// @symbol RouletteRacer_CheckLandingA
extern "C" {
int RouletteRacer_CheckLandingA(int *racer) {
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
    dx = (((RouletteRacer *)racer)->x >> 0xc) - gTouchX[idx * stride];
    dy = (((RouletteRacer *)racer)->y >> 0xc) - gTouchY[idx * 4];
    if ((((dx >= (-8)) && (dx < 8)) && (dy >= (-8))) && (dy < 8)) {
        Sound::PlayBank2_2D(0x15d);
        sRouletteLandedRacer = (void *)racer;
        ((RouletteRacer *)racer)->state = 2;
        ((RouletteRacer *)racer)->grabX = dx << 0xc;
        ((RouletteRacer *)racer)->grabY = dy << 0xc;
        return 1;
    }
    return 0;
}
}

// @symbol RouletteRacer_CheckLandingB
extern "C" {
void RouletteRacer_CheckLandingB(int *racer) {
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
    if (((RouletteRacer *)racer)->state != 1)
        return;
    idx = gActivePlayerSlot;
    px = ((RouletteRacer *)racer)->x;
    py = ((RouletteRacer *)racer)->y;
    touchX = gTouchX[idx * stride];
    tile = ((RouletteRacer *)racer)->tile;
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
    ((RouletteRacer *)racer)->state = 2;
    ((RouletteRacer *)racer)->grabX = dx << 12;
    ((RouletteRacer *)racer)->grabY = dy << 12;
}
}

// @symbol RouletteRacer_Draw
extern "C" {
void RouletteRacer_Draw(char *racer) {
    RouletteRacer *t = (RouletteRacer *)racer;
    if (t->state == 6)
        return;
    RenderOamBothScreens(sRouletteRacerSprites, t->x >> 0xc, t->y >> 0xc, -1, t->priority,
                         (void *)0);
    t->priority = 1;
}
}

// @symbol RouletteRacer_GetPayout
/* The three range tests are kept as 0 or 1 values held in a local and
   tested; the ROM materialises them. The 0x23 case tests `tile < 6`, the
   opposite of the case beside it, and the two share one `payout = 2`. */
extern "C" {
int RouletteRacer_GetPayout(short *racer, int tile) {
    int landed = ((RouletteRacer *)racer)->tile;
    int payout = 0;
    int inRange;

    inRange = (landed <= 0xb) ? 1 : 0;
    if (inRange != 0) {
        if (landed == tile)
            payout = 0xc;
    } else {
        inRange = (landed >= 0xc && landed <= 0x1c) ? 1 : 0;
        if (inRange != 0) {
            inRange = (landed - 0xc) << 2;
            if (tile == *(short *)((char *)data_ov006_0212edb8 + inRange) ||
                tile == *(short *)((char *)data_ov006_0212edba + inRange))
                payout = 6;
        } else {
            inRange = (landed >= 0x1d && landed <= 0x22) ? 1 : 0;
            if (inRange != 0) {
                inRange = (landed - 0x1d) << 3;
                if (tile == *(short *)((char *)data_ov006_0212ed88 + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8a + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8c + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8e + inRange))
                    payout = 3;
            } else if ((landed == 0x23 && tile < 6) || (landed == 0x24 && tile >= 6)) {
                payout = 2;
            }
        }
    }
    ((RouletteRacer *)racer)->payout = payout;
    return payout;
}
}

// @symbol Roulette_TileAt
extern "C" {
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
}

// @symbol RouletteBoard_LoadModels
extern "C" {
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
}

// @symbol RouletteBoard_FreeModels
extern "C" {
int RouletteBoard_FreeModels(RouletteBoard *res) {
    if (res->mWheelFile)
        Ov004_Deallocate(res->mWheelFile);
    if (res->mBallFile)
        Ov004_Deallocate(res->mBallFile);
    return 1;
}
}

// @symbol RouletteBoard_StartSpin
extern "C" {
void RouletteBoard_StartSpin(char *slider) {
    RouletteBoard *board = (RouletteBoard *)slider;
    unsigned rand;
    board->mBallRadius = 0x4200;
    board->mBallRadiusVel = 0;
    board->mOrbitAngle = 0;
    board->mBoardAngle = 0;
    board->mAngleStep = 0x400;
    board->mBoardSpin = -0x200;
    rand = (unsigned)RandomIntInternal(data_0209e650);
    board->mSpinSpeed = ((rand & 0x7fffffff) >> 0x13) + 0x2000;
    board->mBraking = 0;
    RouletteBoard_SetStateSpin(board);
}
}

// @symbol RouletteBoard_SetStateSpin
extern "C" {
void RouletteBoard_SetStateSpin(RouletteBoard *board) {
    *(Pair2W *)&board->mState = pmf_RouletteBoard_StateSpin;
}
}

// @symbol RouletteBoard_StateSpin
extern "C" {
void RouletteBoard_StateSpin(RouletteBoard *board) {
    int spin = board->mSpinSpeed;
    int pull = cstd::fdiv(FX_MUL(spin, spin), board->mBallRadius) - 0xc00;

    spin = board->mSpinSpeed;
    int scaled = FX_MUL(spin, 0x500);
    int step = cstd::fdiv(scaled, board->mBallRadius);
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
    s16 val1 = data_02082214[idx * 2 + 1];
    s16 val0 = data_02082214[idx * 2];

    int x, y, z;
    struct Vec3 t;
    x = FX_MUL(dist, val1);
    y = FX_MUL(dist, 0x400) + 0x780;
    z = FX_MUL(dist, val0);
    ((int *)&t)[0] = x;
    ((int *)&t)[1] = y;
    ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1],
                                        ((int *)&t)[2]);

    board->mBall.mat4x3 = data_020a0e68;
}
}

// @symbol RouletteBoard_StartSlowdown
extern "C" {
void RouletteBoard_StartSlowdown(RouletteBoard *board) {
    board->mSpinSpeed += 0x140;
    board->mSlowBias = -board->mBoardAngle;
    *(struct Pair *)&board->mState = pmf_RouletteBoard_StateSlow;
}
}

// @symbol RouletteBoard_StateSlow
extern "C" {
void RouletteBoard_StateSlow(RouletteBoard *board) {
    int spinAbs;
    int pull;
    short sum;
    ShortTbl12 stops;
    short diff;
    short idx;
    int extra;
    int angidx, idx2;
    short t0, t1;
    int x, y, z;
    Vec3 t;

    spinAbs = board->mSpinSpeed - 0x140;
    if (spinAbs < 0)
        spinAbs = -spinAbs;
    pull = cstd::fdiv((int)((((long long)spinAbs * spinAbs) + 0x800) >> 12), board->mBallRadius) -
           0xc00;

    board->mAngleStep = cstd::fdiv((int)((((long long)board->mSpinSpeed * 0x500) + 0x800) >> 12),
                                   board->mBallRadius);

    board->mOrbitAngle += board->mAngleStep;

    board->mBallRadiusVel = (int)((((long long)board->mBallRadiusVel * 0x600) + 0x800) >> 12);
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

    board->mStopTile = data_ov006_0212ed34[idx];

    extra = 0;
    if (board->mBallRadius < 0x2000) {
        if (diff < -0x400 && (diff - board->mAngleStep) >= -0x400) {
            board->mOrbitAngle = (stops.v[idx] - 0x400) - board->mSlowBias;
            board->mSpinSpeed = -(int)((((long long)board->mSpinSpeed * 0xf00) + 0x800) >> 12);
            ApproachLinear(board->mSpinSpeed, 0xfffff667, 0x30);
            if (board->mRollSound != 0) {
                Sound::PlayBank2_2D(0x159);
                board->mRollSound = 0;
            }
        } else if (diff > 0x400 && (diff - board->mAngleStep) <= 0x400) {
            board->mOrbitAngle = (stops.v[idx] + 0x400) - board->mSlowBias;
            board->mSpinSpeed = -(int)((((long long)board->mSpinSpeed * 0xe00) + 0x800) >> 12);
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
        extra = (int)((((long long)(board->mBallRadius - 0x2000) * 0x100) + 0x800) >> 12);
    }

    ApproachLinear(board->mSpinSpeed, 0, 0x30);

    angidx = ((unsigned short)(sum + board->mBoardAngle)) >> 4;
    idx2 = angidx * 2;
    t0 = data_02082214[idx2 + 1];
    t1 = data_02082214[idx2];

    x = (int)((((long long)board->mBallRadius * t0) + 0x800) >> 12);
    y = (int)((((long long)board->mBallRadius * 0x400) + 0x800) >> 12) + 0x780 + extra;
    z = (int)((((long long)board->mBallRadius * t1) + 0x800) >> 12);
    ((int *)&t)[0] = x;
    ((int *)&t)[1] = y;
    ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1],
                                        ((int *)&t)[2]);

    board->mBall.mat4x3 = data_020a0e68;
}
}

// @symbol RouletteBoard_Update
extern "C" void RouletteBoard_Update(RouletteBoard *c) {
    c->mBoardAngle += c->mBoardSpin;
    /* -boardAngle lands at bones + 0x82, inside the third 0x40-byte
       BMD_Bone's unk_00; BMD_File.h names no field there. */
    *(short *)((char *)c->mWheel.data.bones + 0x82) = (short)(-c->mBoardAngle);
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

// @symbol RouletteBoard_Draw
extern "C" void RouletteBoard_Draw(char *slider) {
    RouletteBoard *board = (RouletteBoard *)slider;
    board->mWheel.Render(0);
    board->mBall.Render(0);
}

// @symbol RouletteStrip_Reset
extern "C" {
void RouletteStrip_Reset(int *strip) {
    int i;
    RouletteStrip *s = (RouletteStrip *)strip;
    for (i = 0; i < 5; i++)
        s->racers[i] = 0;
    s->payoutIndex = 4;
    s->count = 0;
}
}

// @symbol RouletteStrip_AddRacer
extern "C" {
void RouletteStrip_AddRacer(int *strip, int racer) {
    RouletteStrip *s = (RouletteStrip *)strip;
    int i;
    s->count += 1;
    for (i = 0; i < 5; i++) {
        if (s->racers[i] == 0) {
            s->racers[i] = (RouletteRacer *)racer;
            return;
        }
    }
}
}

// @symbol RouletteStrip_Draw
extern "C" {
void RouletteStrip_Draw(short *strip, int y) {
    int idx = GetGameLanguage();
    void *p = *(void **)((char *)sRouletteTextSpritesByLanguage[idx] + 0x50);
    Hud_RenderSprite(p, 0xc, y, -1, -1);

    Mg_DrawBigNumber(0x1e, y, sRoulettePayoutMultipliers[((RouletteStrip *)strip)->payoutIndex], 0,
                     -1, 2, 0);

    {
        int b = y + 0x10;
        RenderOamBothScreens(sRouletteRacerSprites, 0x20, b, -1, 1, (void *)0);
        Mg_DrawBigNumber(0x2e, b, ((RouletteStrip *)strip)->count, 0, -1, 1, 0);
    }
}
}

// @symbol RouletteStrip_DrawRacers
extern "C" {
void RouletteStrip_DrawRacers(void **strip) {
    int i;
    for (i = 0; i < 5; i++) {
        RouletteRacer *obj = ((RouletteStrip *)strip)->racers[i];
        int m, idx, r0, a1, a2;
        if (obj == 0)
            return;
        obj->priority = 0;
        r0 = ((RouletteStrip *)strip)->payoutIndex;
        m = obj->tile;
        idx = data_ov006_0212ed24[r0];
        if (data_020a0db0 & 8)
            idx++;
        if (m >= 0xc && m <= 0x13)
            idx += 2;
        a1 = ((TileXY *)sRouletteTiles)[m].x >> 12;
        a2 = ((TileXY *)sRouletteTiles)[m].y >> 12;
        if (r0 == 3)
            a1 += 0x58;
        Hud_RenderSprite(sRouletteStripSprites[idx], a1, a2, -1, -1);
    }
}
}

// @symbol RouletteStrip_LinkAll
extern "C" {
void RouletteStrip_LinkAll(char *p) {
    int i;
    for (i = 0; i < 4; i++) {
        sRouletteStrips[i] = p;
        p += 0x18;
    }
}
}

// @symbol RouletteStrip_ResetAll
extern "C" {
void RouletteStrip_ResetAll(void) {
    int i;
    for (i = 0; i < 4; i++) {
        RouletteStrip_Reset((int *)sRouletteStrips[i]);
        ((RouletteStrip *)sRouletteStrips[i])->payoutIndex = (short)i;
    }
    sRouletteResultIndex = -1;
    sRouletteResultsTimer = 0;
}
}

// @symbol RouletteResults_Tick
extern "C" int RouletteResults_Tick(void) {
    if (ApproachLinear(sRouletteResultsTimer, 0, 1) != 0) {
        int idx;
        sRouletteResultIndex = sRouletteResultIndex + 1;
        idx = sRouletteResultIndex;
        sRouletteResultsTimer = 0x78;
        while (idx < 4) {
            if (((RouletteStrip *)sRouletteStrips[idx])->count != 0)
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

// @symbol RouletteResults_Draw
extern "C" {
void RouletteResults_Draw(void) {
    int n = sRouletteResultIndex;
    int arg;
    int i;
    if (n < 0)
        return;
    n = n + 1;
    if (n > 4)
        n = 4;
    arg = 0x28;
    for (i = 0; i < n; i++) {
        int *o = (int *)sRouletteStrips[i];
        if (((RouletteStrip *)o)->count > 0) {
            RouletteStrip_Draw((short *)o, arg);
            arg += 0x24;
        }
    }
    if (sRouletteResultIndex >= 4)
        return;
    RouletteStrip_DrawRacers((void **)sRouletteStrips[sRouletteResultIndex]);
}
}

// @symbol _ZN13RouletteRacerD1Ev
/* Empty, like the constructor; out of line because __cxa_vec_cleanup takes
   its address. */
RouletteRacer::~RouletteRacer() {}

/* The destructor pair, D1 at 0x0210788c and D0 at 0x02107920, is not
   defined here. It is declared inline as the class's first member in
   dScMgRoulette_c.h: that is the only form that makes the compiler emit D1
   below D0 with no D2, as the cartridge has them. */
