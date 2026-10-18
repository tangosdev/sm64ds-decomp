//cpp
/* Memory Master (MG_MEMORY_J). Deal the cards, move the markers, judge pairs.
 *
 * Functions are in ROM order; do not reorder. The local pragmas keep their
 * emission. Earlier probes are in
 * notes/experiments/pr2875-source-repair-0920.json.
 *
 * leftovers:
 * - DrawCards, RoundWaitDeal, RoundReadyCards, RoundHideCards,
 *   RoundShowCards, ResultFinish, ResetGame and InitPlayers step char* by
 *   0x18 or 0x14 from this. A card or player pointer changes those loops.
 * - CardFlyAway keeps FlyAwaySceneView. Direct mCards access misses.
 * - CheckFinished calls inline_fn(this+0xa8) and this+0xb4.
 *   mCredits and mHudScore directly miss. unk_5407 is only cleared.
 * - ResultWait decrements mResultTimer as (int)this+0x53e4.
 * - Camera_InitDealerView sets the camera at this+0x4660 and tail-calls
 *   Camera_UpdateMatrices. Render and InitResources call that wrapper.
 * - Mg_DrawScoreTally, Mg_SaveRecordPair and Dealer_Load
 *   stay the linker names. Their bodies are not in this file.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgMemory2_c_AssetId { ASSET_D_2D_MG_CARD_NCL_BIN = 0x00bb };

#include "dScMgMemory2_c.h"
#include "SpawnInfo.h"
#include "Sound.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "decl_common.h"
#include "PlayerInput.h"

/* CardFlyAway, ResultWait and RoundDealHard retain the partial views below.
 * Their array extents span unrelated fields to retain specific address
 * calculations; they are not original class layouts. */

typedef void (dScMgMemory2_c::*dScMgMemory2_cState)();
typedef void (dScMgMemory2_c::*dScMgMemory2_cCardState)(int);
extern "C" dScMgMemory2_cCardState sMemory2CardTable[];

typedef struct FlyAwayCardView {
    /* 0x00 */ s32 x;
    /* 0x04 */ u8 unk04[0x12 - 0x04];
    /* 0x12 */ u8 visible;
    /* 0x13 */ u8 active;
    /* 0x14 */ u8 state;
    /* 0x15 */ u8 frame;
    /* 0x16 */ u8 flyAwayStarted;
    /* 0x17 */ u8 pad_17;
} FlyAwayCardView;

typedef struct FlyAwaySceneView {
    /* 0x0000 */ FlyAwayCardView raw[1];
    /* 0x0018 */ u8 unk18[0x51a8 - 0x18];
    /* 0x51a8 */ FlyAwayCardView cards[25];
    /* 0x5400 */ u8 unk5400[0xa];
    /* 0x540a */ u8 difficulty;
} FlyAwaySceneView;

struct ScoreView {
    char pad[0xb4];
    int hudScore;
    int bestHudScore;
};

typedef struct {
    char _pad0[0x13];
    u8 active; /* +0x13 */
    char _pad1[4];
} DealCardView; /* 0x18 */

typedef struct {
    char _pad0[0x51a8];
    DealCardView cards[23]; /* 0x51a8 */
    char _pad1[8];
    int substate; /* 0x53d8 */
    char _pad2[0xa];
    short dealSignals; /* 0x53e6 */
    short cardsDealt;  /* 0x53e8 */
    u16 targetPairs;   /* 0x53ea */
    char _pad3[0x1e];
    u8 difficulty; /* 0x540a */
} DealSceneView;

extern "C" dScMgMemory2_cState sMemory2ResultTable[];
extern "C" dScMgMemory2_cState sMemory2StateTable[];
extern "C" dScMgMemory2_cState sMemory2RoundTable[];

namespace GX {
void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

namespace GXS {
void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

extern "C" {
extern void Mg_DrawScoreTally(void *thiz, int a, int b, int c);
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
extern "C" unsigned int func_02012790(unsigned int x);
extern "C" void MgCoins_StartLoseOneMore(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 sMemory2CardFrameSprites[];
extern void *sMemory2CardSprites[];
extern s32 sMemory2FlyAwayLimitPerMode[];
extern unsigned short sMemory2FlipFrameTimes[];
extern void Mg_SaveRecordPair(int a, int b);
extern u16 *sMemory2CardPositionsPerDifficulty[];
extern int sMemory2ResultRowsPerDifficulty[];
extern int sMemory2ResultColsPerDifficulty[];
extern void MgCoins_StartPayout(char *p);
extern short gMgCoinsStatus;
extern void Mg_SetPerfectBanner(void);
extern void Mg_SetSharedState(int);
extern void Mg_SubmitScore(int self);
extern void Dealer_PlayWin(void *c);
extern void Dealer_PlayLose(void *c);
extern void Dealer_StartShuffle(char *c, int unused, short count, int ready);
extern int sMemory2FourthDealsPerDifficulty[];
extern int sMemory2HardDealsPerMode[];
extern int sMemory2NormalDealsPerDifficulty[];
extern int sMemory2EasyDealsPerDifficulty[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int sMemory2PreviewCountPerDifficulty[];
extern int sMemory2PreviewTimePerDifficulty[];
extern void FreeGfxSlotsById(int x);
extern void Dealer_PlayIdle(char *p);
extern int Dealer_IsIdle(int *p);
extern void MgCoins_Reset(void);
void Camera_InitDealerView(char *);
void Mg_DrawCredits(char *, int, int, int);
void Dealer_Draw(char *);
extern "C" void MgCoins_Update(void);
extern "C" void Dealer_Update(char *p);
void LoadCard2SubBg3(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int Dealer_Load(void *c);
int Mg_GetSavedRecord0(void);
int Mg_GetSavedRecord1(void);
void Mg_SetTopScreenYOffset(int v);
void Mg_PlaceBannersOnTopScreen(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

#pragma defer_codegen off

// @symbol _ZN14dScMgMemory2_cD1Ev
/* Destroys mShared, then the inherited scene. CodeWarrior emits both D1 and
 * D0 from this one definition. */
dScMgMemory2_c::~dScMgMemory2_c() {}
// @symbol _ZN14dScMgMemory2_cD0Ev
/* Deleting variant, emitted from the destructor above. */

void dScMgMemory2_c::DrawMessage() {
    if (mMessageVisible == 0)
        return;
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
}

void dScMgMemory2_c::DrawCursor() {
    if (mCursor.enabled == 0)
        return;
    Mg_DrawBlinkSprite(this, 0xe0, 0xa0);
}

void dScMgMemory2_c::UpdateCursor() {
    if (mCursor.visible == 0)
        return;
    {
        unsigned short *timer = (unsigned short *)&mCursor.blinkTimer;
        *timer = *timer + 1;
        if (*timer < 0x14)
            return;
        *timer = 0;
    }
    {
        unsigned char *frame = &mCursor.frame;
        *frame = *frame + 1;
        *frame = *frame & 1;
    }
}

void dScMgMemory2_c::ShowCursor() {
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 966656;
    mCursor.y = 688128;
    mCursor.blinkTimer = 0;
    mCursor.frame = 0;
}

void dScMgMemory2_c::HideCursor() {
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

void dScMgMemory2_c::PlayerWait(int /* player */) {}

void dScMgMemory2_c::PlayerMove(int player) {
    int targetX = (player << 4) + 0xc;
    int targetY = -0x2c;
    u16 *angle;
    int dx;
    int dy;
    dx = targetX - (mPlayers[player].x >> 12);
    dy = targetY - (mPlayers[player].y >> 12);
    mPlayers[player].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
    mPlayers[player].speed = mPlayers[player].speed + 0x200;
    angle = (u16 *)&mPlayers[player].angle;
    mPlayers[player].x = mPlayers[player].x +
                         FX_MUL(data_02082214[((*angle >> 4) << 1) + 1], mPlayers[player].speed);
    mPlayers[player].y =
        mPlayers[player].y + FX_MUL(data_02082214[(*angle >> 4) << 1], mPlayers[player].speed);
    dx = targetX - (mPlayers[player].x >> 12);
    dy = targetY - (mPlayers[player].y >> 12);
    if (dx < -3 || dx > 3) {
        return;
    }
    if (dy < -3 || dy > 3) {
        return;
    }
    mPlayers[player].x = targetX << 12;
    mPlayers[player].y = targetY << 12;
    mPlayers[player].state = PLAYER_WAIT;
}

void dScMgMemory2_c::PlayerDrop(int player) {
    short *delay = &mPlayers[player].delay;
    if (*(unsigned short *)delay != 0) {
        *delay = *(unsigned short *)delay - 1;
        if (*delay < 0)
            *delay = 0;
        return;
    }
    mPlayers[player].x = 0xc000;
    mPlayers[player].y = 0xc000;
    mPlayers[player].unk_11 = 1;
    mPlayers[player].state = PLAYER_MOVE;
    mPlayers[player].speed = 0xc00;
}

void dScMgMemory2_c::InitPlayers() {
    char *raw = (char *)this;
    int markerCount;
    int markerIndex;
    int delay;
    char *entry;

    if (gMgScene != 0)
        markerCount = gMgScene->mCredits;
    else
        markerCount = 0;
    if (markerCount >= 5)
        markerCount = 5;

    entry = raw;
    markerIndex = 0;
    mMaxMisses = (unsigned char)markerCount;
    if (markerCount <= 0)
        return;

    delay = 0;
    do {
        *(unsigned char *)(entry + 0x5398) = 1;
        *(unsigned char *)(entry + 0x539a) = 0;
        *(short *)(entry + 0x5396) = delay;
        markerIndex++;
        entry += 0x14;
        delay += 0x10;
    } while (markerIndex < markerCount);
}

/* inline_fn stays: reading mCredits and mHudScore directly changes the code. */
void Mg_SaveRecordPair(int r0arg, int r1arg);
inline int *inline_fn(char *arg0) {
    return (int *)arg0;
}

void dScMgMemory2_c::CheckFinished() {
    char *raw = (char *)this;
    if (mPairsFound >= mTargetPairs) {
        mResultTimer = 0x40;
        mState = STATE_RESULT;
        mSubstate = RESULT_WAIT;
        Mg_SaveRecordPair(((5 - mMisses) * 5) + (*inline_fn(raw + 0xa8)),
                          (*inline_fn(raw + 0xb4)) + 1);
    }
    if (mMisses >= mMaxMisses) {
        mResultTimer = 0x40;
        mState = STATE_RESULT;
        mSubstate = RESULT_WAIT;
        Mg_SaveRecordPair(*inline_fn(raw + 0xa8), (*inline_fn(raw + 0xb4)) - 1);
    }
}

void dScMgMemory2_c::JudgePair() {
    int first, second;
    unsigned char *firstState, *secondState;
    if (mSelectedCount < 2)
        return;
    first = mSelectedCards[0];
    firstState = &mCards[first].state;
    second = mSelectedCards[1];
    if (*firstState != CARD_WAIT)
        return;
    secondState = &mCards[second].state;
    if (*secondState != CARD_WAIT)
        return;
    if (mCards[first].value == mCards[second].value) {
        mCards[first].active = 0;
        mCards[first].visible = 0;
        mCards[second].active = 0;
        mCards[second].visible = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        mPairsFound += 1;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        Sound::PlayBank2_2D(0x13e);
        mMisses += 1;
        MgCoins_StartLoseOneMore();
        if (mMisses < mMaxMisses) {
            *firstState = CARD_FLIP_DOWN;
            *secondState = CARD_FLIP_DOWN;
            mSelectedCount = 0;
        }
    }
}

// opt_propagation off keeps the card loop's constant live, as in the ROM.
#pragma push
#pragma opt_propagation off
void dScMgMemory2_c::DrawCards() {
    char *raw = (char *)this;
    int cardIndex;
    int sprite, dimmed;
    char *p;

    /* p walks the 20 cards. sMemory2CardFrameSprites is indexed [value][frame]. */
    p = raw;
    cardIndex = 0;
    do {
        if (*(u8 *)(p + 0x51ba) != 0) {
            dimmed = 0;
            if (mState != STATE_JUDGE_PAIRS)
                dimmed = 1;
            sprite = sMemory2CardFrameSprites[*(u8 *)(p + 0x51b8) * 5 + *(u8 *)(p + 0x51bd)];
            Hud_RenderSprite(sMemory2CardSprites[sprite], *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12, -1, dimmed);
        }
        cardIndex += 1;
        p += 0x18;
    } while (cardIndex < 20);
}
#pragma pop

void dScMgMemory2_c::UpdateCards() {
    int cardIndex;
    for (cardIndex = 0; cardIndex < 0x14; cardIndex++) {
        if (mCards[cardIndex].active)
            (this->*sMemory2CardTable[mCards[cardIndex].state])(cardIndex);
    }
}

void dScMgMemory2_c::CardFlyAway(int cardIndex) {
    FlyAwaySceneView *view = (FlyAwaySceneView *)this;
    s32 rowWidth;
    s32 column;
    s32 precedingOffset;
    s32 *currentX;
    s32 scanLimit;
    u8 *flyAwayStarted;

    view->cards[cardIndex].x -= 0x10000;
    if (view->cards[cardIndex].flyAwayStarted == 0) {
        rowWidth = sMemory2FlyAwayLimitPerMode[view->difficulty];
        column = cardIndex;
        if (cardIndex >= rowWidth) {
            scanLimit = rowWidth;
            do {
                column -= scanLimit;
            } while (column >= scanLimit);
        }
        if (column == 0) {
            view->cards[cardIndex].flyAwayStarted = 1;
            return;
        }
        scanLimit = column + 1;
        precedingOffset = 1;
        if (scanLimit > 1) {
            do {
                currentX = (s32 *)((u8 *)&view->raw[cardIndex] + 0x51a8);
                flyAwayStarted = (u8 *)&view->raw[cardIndex] + 0x51be;
                if (view->cards[cardIndex - precedingOffset].active != 0) {
                    if (((*currentX - view->cards[cardIndex - precedingOffset].x) >> 12) <= 4) {
                        *flyAwayStarted = 1;
                        view->cards[cardIndex - precedingOffset].state = CARD_FLY_AWAY;
                    }
                }
                precedingOffset++;
            } while (precedingOffset < scanLimit);
        }
    }
    if ((view->cards[cardIndex].x >> 12) > -0x18) {
        return;
    }
    *((u8 *)view + cardIndex * 24 + 0x51bb) = 0;
    *((u8 *)view + cardIndex * 24 + 0x51ba) = 0;
}

void dScMgMemory2_c::CardFlipDown(int cardIndex) {
    unsigned short elapsed;
    mCards[cardIndex].animTimer += 1;
    elapsed = mCards[cardIndex].animTimer;
    if (elapsed < (sMemory2FlipFrameTimes[mCards[cardIndex].frame] & 0xff))
        return;
    mCards[cardIndex].animTimer = 0;
    mCards[cardIndex].frame -= 1;
    if (mCards[cardIndex].frame == 0)
        mCards[cardIndex].state = CARD_SELECT;
}

void dScMgMemory2_c::CardWait(int /* card */) {}

void dScMgMemory2_c::CardFlipUp(int cardIndex) {
    unsigned short *timer = &mCards[cardIndex].animTimer;
    unsigned char *frame = &mCards[cardIndex].frame;
    *timer = *timer + 1;
    if (*timer < (sMemory2FlipFrameTimes[*frame] & 0xff))
        return;
    *timer = 0;
    *frame = *frame + 1;
    if (*frame > 4) {
        *frame = 4;
        mCards[cardIndex].state = CARD_WAIT;
    }
}

void dScMgMemory2_c::CardSelect(int cardIndex) {
    unsigned int selectionIndex;
    unsigned int playerSlot;
    int touchPressed;
    int dx, dy;

    selectionIndex = mSelectedCount;
    if (selectionIndex >= 2)
        return;

    playerSlot = gActivePlayerSlot;
    touchPressed = 0;
    if (gTouchHeld[playerSlot * 4] != 0) {
        if (gTouchEdge[playerSlot * 4] != 0) {
            touchPressed = 1;
        }
    }
    if (touchPressed == 0)
        return;

    dx = gTouchX[gActivePlayerSlot * 4] - (mCards[cardIndex].x >> 12);
    dy = gTouchY[gActivePlayerSlot * 4] - (mCards[cardIndex].y >> 12);

    if (dx < -0x10)
        return;
    if (dx > 0x10)
        return;
    if (dy < -0x16)
        return;
    if (dy > 0x16)
        return;

    mSelectedValues[selectionIndex] = mCards[cardIndex].value;
    mSelectedCards[mSelectedCount] = (u8)cardIndex;
    {
        u8 *selectedCount = &mSelectedCount;
        *selectedCount = *selectedCount + 1;
    }
    mCards[cardIndex].state = CARD_FLIP_UP;
    func_02012718(0x143, mCards[cardIndex].x);

    if (mInputSeen != 0)
        return;
    {
        u8 *inputSeen = &mInputSeen;
        *inputSeen = *inputSeen + 1;
    }
    Mg_SaveRecordPair(mCredits, mHudScore);
}

void dScMgMemory2_c::CardIdle(int /* card */) {}

void dScMgMemory2_c::CardMove(int cardIndex) {
    int coordinateIndex = cardIndex * 2;
    u16 *positions = sMemory2CardPositionsPerDifficulty[mDifficulty];
    int dx, dy, targetX, targetY;
    targetX = positions[coordinateIndex];
    dx = targetX - (mCards[cardIndex].x >> 12);
    targetY = positions[coordinateIndex + 1];
    dy = targetY - (mCards[cardIndex].y >> 12);

    mCards[cardIndex].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[((static_cast<u16>(mCards[cardIndex].angle) >> 4) << 1) + 1];
        mCards[cardIndex].x += FX_MUL(tv, mCards[cardIndex].speed);
    }
    {
        s16 tv = data_02082214[(static_cast<u16>(mCards[cardIndex].angle) >> 4) << 1];
        mCards[cardIndex].y += FX_MUL(tv, mCards[cardIndex].speed);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    mCards[cardIndex].x = targetX << 12;
    mCards[cardIndex].y = targetY << 12;
    *&mCards[cardIndex].state = CARD_IDLE;
}

#pragma push
#pragma opt_loop_invariants off
void dScMgMemory2_c::ResultFinish() {
    char *raw = (char *)this;
    int rowIndex;
    int lastActiveCard;
    int column;
    int rowWidth;
    int playerSlot;
    int touchPressed;
    unsigned short *timer;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0) {
        timer = &mCardTimer;
        *timer = *timer - 1;
        return;
    }

    touchPressed = 0;
    playerSlot = gActivePlayerSlot;
    if (gTouchHeld[playerSlot * 4] != 0) {
        if (gTouchEdge[playerSlot * 4] != 0) {
            touchPressed = 1;
        }
    }
    if (touchPressed == 0) {
        return;
    }

    func_02012790(0x62);

    if (mCredits == 0) {
        for (rowIndex = 0; rowIndex < sMemory2ResultRowsPerDifficulty[mDifficulty]; rowIndex++) {
            lastActiveCard = 0xff;
            column = 0;
            rowWidth = sMemory2ResultColsPerDifficulty[mDifficulty];
            for (; column < sMemory2ResultColsPerDifficulty[mDifficulty]; column++) {
                if (*(unsigned char *)(raw + 0x51bb + (rowIndex * rowWidth + column) * 0x18) != 0) {
                    lastActiveCard = column;
                    lastActiveCard = lastActiveCard + rowIndex * rowWidth;
                }
            }
            if (lastActiveCard != 0xff) {
                mCards[lastActiveCard].state = CARD_FLY_AWAY;
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = STATE_EXIT;
}
#pragma pop

void dScMgMemory2_c::ResultTurnCards() {
    if (mCardTimer) {
        unsigned short *timer = &mCardTimer;
        *timer = *timer - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = STATE_EXIT;
        mCardTimer = 0;
        return;
    }
    {
        int cardsToReveal = 0;
        int cardIndex = 0;
        for (; cardIndex < 0x14; cardIndex++) {
            if (mCards[cardIndex].active != 0) {
                if (mCards[cardIndex].state == CARD_SELECT) {
                    mCards[cardIndex].state = CARD_FLIP_UP;
                    cardsToReveal++;
                }
            }
        }
        if (cardsToReveal <= 2) {
            Sound::PlayBank2_2D(0x145);
        } else {
            Sound::PlayBank2_2D(0x146);
        }
    }
    mSubstate = RESULT_FINISH;
    ShowCursor();
    mCardTimer = 0x20;
}

void dScMgMemory2_c::ResultReward() {
    if (mCardTimer != 0) {
        unsigned short *timer = &mCardTimer;
        *timer = *timer - 1;
        if (mCardTimer != 0) {
            return;
        }
        if (mPairsFound < mTargetPairs) {
            return;
        }
        MgCoins_StartPayout((char *)(5 * (5 - mMisses)));
        return;
    }
    if (gMgCoinsStatus == 1) {
        mCardTimer = 0;
        mSubstate = RESULT_TURN_CARDS;
    }
}

void dScMgMemory2_c::ResultWait() {
    char *raw = (char *)this;
    struct ScoreView *scoreView;

    if (mResultTimer == 0)
        goto zero;

    *(unsigned short *)(((int)raw + 0x53e4)) = *(unsigned short *)(((int)raw + 0x53e4)) - 1;
    if (*(short *)(raw + 0x53e4) > 0)
        return;

    mResultTimer = 0;

    if (mPairsFound >= mTargetPairs) {
        if (mMisses == 0)
            Mg_SetPerfectBanner();
        Mg_SetSharedState(MG_STATE_WIN);
        scoreView = (struct ScoreView *)gMgScene;
        if (scoreView != 0) {
            if (scoreView->hudScore < 0x270f)
                scoreView->hudScore += 1;
            if (scoreView->hudScore > scoreView->bestHudScore)
                scoreView->bestHudScore = scoreView->hudScore;
        }
        Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
        Dealer_PlayWin(&mShared);
        return;
    }

    Mg_SetSharedState(MG_STATE_LOSE);
    scoreView = (struct ScoreView *)gMgScene;
    if (scoreView != 0) {
        if (scoreView->hudScore > 0)
            scoreView->hudScore -= 1;
    }
    Dealer_PlayLose(&mShared);
    return;

zero:
    mPromptEnabled = 0;
    mCardTimer = 0x10;
}

void dScMgMemory2_c::RoundReveal() {
    unsigned short *timer;
    if (mCardTimer != 0) {
        timer = &mCardTimer;
        *timer = *timer - 1;
        if ((short)mCardTimer > 0) {
            return;
        }
        InitPlayers();
        return;
    }
    {
        long i;
        for (i = 0; i < 0x14; i++) {
            mCards[i].state = CARD_SELECT;
        }
    }
    mSubstate = ROUND_START;
    mState = STATE_JUDGE_PAIRS;
}

void *dScMgMemory2_c::RoundWaitDeal() {
    char *raw = (char *)this;
    int pending = 0;
    int cardIndex = 0;
    char *p = raw;
    for (; cardIndex < 0x14;) {
        if (*(unsigned char *)(p + 0x51bb) != 0) {
            if (*(unsigned char *)(p + 0x51bc) != CARD_IDLE) {
                pending++;
                break;
            }
        }
        cardIndex++;
        p += 0x18;
    }
    if (pending != 0)
        return raw;
    p = raw;
    cardIndex = 0;
    {
        unsigned char selectState = CARD_SELECT;
        for (; cardIndex < 0x14;) {
            *(unsigned char *)(p + 0x51bc) = selectState;
            cardIndex++;
            p += 0x18;
        }
    }
    {
        char *base = raw + 0x5000;
        mSubstate = ROUND_START;
        mState = STATE_JUDGE_PAIRS;
        return base;
    }
}

void dScMgMemory2_c::RoundReadyCards() {
    char *raw = (char *)this;
    int pending = 0;
    int cardIndex;
    char *p;
    for (cardIndex = 0, p = raw; cardIndex < 0x14; cardIndex++, p += 0x18) {
        if (*(unsigned char *)(p + 0x51bb) != 0) {
            if (*(unsigned char *)(p + 0x51bc) == CARD_SELECT)
                *(unsigned char *)(p + 0x51bc) = CARD_IDLE;
        }
    }
    for (cardIndex = 0, p = raw; cardIndex < 0x14; cardIndex++, p += 0x18) {
        if (*(unsigned char *)(p + 0x51bb) != 0) {
            if (*(unsigned char *)(p + 0x51bc) != CARD_IDLE) {
                pending++;
                break;
            }
        }
    }
    if (pending == 0)
        mSubstate = ROUND_WAIT_DEAL;
}

void dScMgMemory2_c::RoundHideCards() {
    char *raw = (char *)this;
    int pending;
    int i;
    unsigned char *cardBase;
    if (mPreviewTimer != 0) {
        i = 0x53ec;
        *((unsigned short *)((((int)raw) + i))) -= 1;
        return;
    }
    cardBase = (unsigned char *)raw;
    for (i = 0; i < 0x14; i++) {
        if (cardBase[0x51bb] != 0) {
            if (cardBase[0x51bc] == CARD_WAIT) {
                cardBase[0x51bc] = CARD_FLIP_DOWN;
            }
        }
        cardBase += 0x18;
    }

    pending = 0;
    i = 0;
    for (cardBase = (unsigned char *)raw; i < 0x14; i++) {
        if (cardBase[0x51bb] != 0) {
            unsigned char cardState = cardBase[0x51bc];
            if ((cardState != CARD_IDLE) && (cardState != CARD_FLIP_DOWN)) {
                pending++;
                break;
            }
        }
        cardBase = cardBase + 0x18;
    }

    if (pending == 0) {
        mSubstate = ROUND_READY_CARDS;
    }
}

void dScMgMemory2_c::RoundShowCards() {
    char *raw = (char *)this;
    unsigned int activeOffset;
    int queued = 0;
    int shown = 0;
    int previewIndex;
    int unusedPreview = 0xff;
    int flipUpState = CARD_FLIP_UP;
    int previewOffset = 0x53fd;
    int stateOffset = 0x51bc;
    int stride = 0x18;

    for (previewIndex = 0; previewIndex < 8; previewIndex++) {
        unsigned char *slot = (unsigned char *)((raw + previewIndex) + previewOffset);
        unsigned char cardIndex = *slot;
        if (cardIndex == 0xff)
            continue;
        {
            int base = (int)raw + cardIndex * stride;
            activeOffset = 0x51bb;
            if (*(unsigned char *)(base + activeOffset) != 0) {
                unsigned char *cardState = (unsigned char *)(base + stateOffset);
                if (*cardState == CARD_IDLE) {
                    *cardState = (unsigned char)flipUpState;
                    *slot = (unsigned char)unusedPreview;
                    shown++;
                }
            } else {
                *slot = (unsigned char)unusedPreview;
            }
            queued++;
        }
    }

    if (shown == 1)
        Sound::PlayBank2_2D(0x148);
    else if (shown == 2)
        Sound::PlayBank2_2D(0x145);
    else if (shown >= 3)
        Sound::PlayBank2_2D(0x146);
    if (queued != 0)
        return;
    mSubstate = ROUND_HIDE_CARDS;
    mPreviewTimer = mDifficulty * 20 + 0x50;
}

void dScMgMemory2_c::RoundDealFourth() {
    int cardIndex;

    if (mDealSignals < 4) {
        return;
    }
    cardIndex = mTargetPairs * 2 - 1 - mCardsDealt;
    mCards[cardIndex].active = 1;
    mCardsDealt++;
    if (mCardsDealt >= sMemory2FourthDealsPerDifficulty[mDifficulty]) {
        mSubstate = ROUND_SHOW_CARDS;
    }
}

/* Once the dealer has signaled three deal steps, activate and count the
 * next card. At the
 * difficulty's limit (sMemory2HardDealsPerMode) move to substate 5 on difficulty 1, 4
 * otherwise. */
void dScMgMemory2_c::RoundDealHard() {
    DealSceneView *view = (DealSceneView *)this;
    u8 difficulty;
    if (view->dealSignals < 3)
        return;
    view->cards[view->targetPairs * 2 - 1 - view->cardsDealt].active = 1;
    mCardsDealt++;
    difficulty = view->difficulty;
    if (view->cardsDealt < sMemory2HardDealsPerMode[difficulty])
        return;
    if (difficulty == 1)
        view->substate = ROUND_SHOW_CARDS;
    else
        view->substate = ROUND_DEAL_FOURTH;
}

void dScMgMemory2_c::RoundDealNormal() {
    int cardIndex;
    if (mDealSignals < 2) {
        return;
    }
    cardIndex = mTargetPairs * 2 - 1 - mCardsDealt;
    mCards[cardIndex].active = 1;
    mCardsDealt++;
    if (mCardsDealt >= sMemory2NormalDealsPerDifficulty[mDifficulty]) {
        mSubstate = ROUND_DEAL_HARD;
    }
}

void dScMgMemory2_c::RoundDealEasy() {
    int cardIndex;
    if (mDealSignals < 1) {
        return;
    }
    cardIndex = mTargetPairs * 2 - 1 - mCardsDealt;
    mCards[cardIndex].active = 1;
    mCardsDealt++;
    if (mCardsDealt >= sMemory2EasyDealsPerDifficulty[mDifficulty]) {
        mSubstate = ROUND_DEAL_NORMAL;
    }
}

void dScMgMemory2_c::RoundStart() {
    mDealSignals = 0;
    mCardsDealt = 0;
    if (mDifficulty == 1)
        Dealer_StartShuffle((char *)&mShared, 4, 3, (int)&mDealSignals);
    else
        Dealer_StartShuffle((char *)&mShared, 4, 4, (int)&mDealSignals);
    mShared.ready = 1;
    mSubstate = ROUND_DEAL_EASY;
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

void dScMgMemory2_c::ShuffleCards() {
    char *raw = (char *)this;
    for (int cardIndex = 0; cardIndex < 11; cardIndex++)
        mValueCounts[cardIndex] = 0;
    for (int cardIndex = 0; cardIndex < 8; cardIndex++)
        mPreviewCards[cardIndex] = 0xff;

    u8 difficulty = mDifficulty;
    if (difficulty == 0) {
        int cardIndex = 0;
        char *p = raw;
        for (; cardIndex < 16; cardIndex++, p += 0x18) {
            u32 cardValue =
                1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
            u8 *valueCount;
        check0:
            valueCount = (u8 *)(raw + cardValue + 0x53f2);
            if (*valueCount < 2) {
                *(u8 *)(p + 0x51b8) = cardValue;
                (*valueCount)++;
            } else {
                cardValue =
                    1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
                goto check0;
            }
        }
        int positionIndex = 0;
        char *q = raw;
        for (; positionIndex < 16; positionIndex++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else if (difficulty == 1) {
        int cardIndex = 0;
        char *p = raw;
        for (; cardIndex < 18; cardIndex++, p += 0x18) {
            u32 cardValue =
                1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
            u8 *valueCount;
        check1:
            valueCount = (u8 *)(raw + cardValue + 0x53f2);
            if (*valueCount < 2) {
                *(u8 *)(p + 0x51b8) = cardValue;
                (*valueCount)++;
            } else {
                cardValue =
                    1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
                goto check1;
            }
        }
        int positionIndex = 0;
        char *q = raw;
        for (; positionIndex < 18; positionIndex++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else {
        int cardIndex = 0;
        char *p = raw;
        for (; cardIndex < 20; cardIndex++, p += 0x18) {
            u32 cardValue =
                1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
            u8 *valueCount;
        check2:
            valueCount = (u8 *)(raw + cardValue + 0x53f2);
            if (*valueCount < 2) {
                *(u8 *)(p + 0x51b8) = cardValue;
                (*valueCount)++;
            } else {
                cardValue =
                    1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
                goto check2;
            }
        }
        int positionIndex = 0;
        char *q = raw;
        for (; positionIndex < 20; positionIndex++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    }
}

void dScMgMemory2_c::ChoosePreviewCards() {
    int difficulty = mDifficulty;
    int previewCount = sMemory2PreviewCountPerDifficulty[difficulty];
    int cardCount = sMemory2PreviewTimePerDifficulty[difficulty];
    int previewIndex;
    int zero = 0;
    for (previewIndex = 0; previewIndex < previewCount; previewIndex++) {
        int cardIndex;
        cardIndex =
            (cardCount * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
        if (previewIndex != 0) {
            int duplicate;
            int previousIndex;
            do {
                duplicate = zero;
                for (previousIndex = zero; previousIndex < previewIndex; previousIndex++) {
                    if (cardIndex == mPreviewCards[previousIndex]) {
                        duplicate++;
                        break;
                    }
                }
                if (duplicate == 0)
                    break;
                cardIndex = (cardCount *
                             (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >>
                            15;
            } while (1);
        }
        mPreviewCards[previewIndex] = (unsigned char)cardIndex;
    }
}

void dScMgMemory2_c::ResetGame() {
    char *c = (char *)this;
    int i;
    char *p;
    int j;
    p = c;
    i = 0;
    do {
        *(int *)(p + 0x51a8) = 0;
        *(int *)(p + 0x51ac) = 0;
        *(int *)(p + 0x51b0) = 0;
        *(short *)(p + 0x51b4) = 0;
        *(short *)(p + 0x51b6) = 0;
        *(char *)(p + 0x51b8) = 0;
        *(char *)(p + 0x51b9) = 0;
        *(char *)(p + 0x51ba) = 0;
        *(char *)(p + 0x51bb) = 0;
        *(char *)(p + 0x51bc) = 0;
        *(char *)(p + 0x51bd) = 0;
        i++;
        *(char *)(p + 0x51be) = 0;
        p += 0x18;
    } while (i < 0x14);
    p = c;
    j = 0;
    do {
        *(int *)(p + 0x5388) = 0;
        *(int *)(p + 0x538c) = 0;
        *(int *)(p + 0x5390) = 0;
        *(short *)(p + 0x5396) = 0;
        *(short *)(p + 0x5394) = 0;
        *(char *)(p + 0x5398) = 0;
        *(char *)(p + 0x5399) = 0;
        j++;
        *(char *)(p + 0x539a) = 0;
        p += 0x14;
    } while (j < 3);
    p = c + 0x5000;
    *(char *)(p + 0x3d0) = 0;
    *(char *)(p + 0x3d2) = 0;
    *(char *)(p + 0x3d1) = 0;
    *(int *)(p + 0x3d4) = 0;
    *(int *)(p + 0x3d8) = 0;
    *(short *)(c + 0x53e0) = 0;
    *(short *)(c + 0x53e2) = 0;
    *(short *)(c + 0x53e4) = 0;
    *(char *)(p + 0x405) = 0;
    *(char *)(p + 0x406) = 0;
    *(char *)(p + 0x407) = 0;
    *(char *)(p + 0x408) = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char *)(p + 0x3ee) = 0;
        i++;
        *(char *)(p + 0x3f0) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char *)(p + 0x3f2) = 0;
    } while (j < 0xb);
    c += 0x5000;
    *(char *)(c + 0x40b) = 1;
    *(char *)(c + 0x40c) = 0;
}

void dScMgMemory2_c::StateExit() {
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

void dScMgMemory2_c::StateResult() {
    (this->*sMemory2ResultTable[mSubstate])();
}

void dScMgMemory2_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

void dScMgMemory2_c::StatePlay() {
    int roundState = mSubstate;
    (this->*sMemory2RoundTable[roundState])();
    UpdateCards();
}

void dScMgMemory2_c::StateSetup() {
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    Dealer_PlayIdle((char *)&mShared);
    mState = STATE_PLAY;
}

void dScMgMemory2_c::SetupDifficulty() {
    mDifficulty = 0;
    mTargetPairs = 8;
    int score = mHudScore;
    if (score >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 0xa;
        return;
    }
    if (score >= 5) {
        mDifficulty = 1;
        mTargetPairs = 9;
    }
}

// @symbol _ZN14dScMgMemory2_c10OnDealDoneEv
void dScMgMemory2_c::OnDealDone() {
    MgCoins_PlaceBet(mMaxMisses);
}

// @symbol _ZN14dScMgMemory2_c11CanEndStateEi
int dScMgMemory2_c::CanEndState(int /* mode */) {
    if (mState == STATE_RESULT && mSubstate == RESULT_WAIT) {
        if (Dealer_IsIdle((int *)&mShared) == 0)
            return 0;
        mSubstate = RESULT_REWARD;
        FreeGfxSlotsById(0x1d);
    }
    return mState == STATE_EXIT;
}

// @symbol _ZN14dScMgMemory2_c10StartRoundEi
/* Clear the board, go back to StateSetup and reload the miss limit. */
void dScMgMemory2_c::StartRound(int /* arg */) {
    dScMgBase_c *scene;
    int missLimit;
    ResetGame();
    mState = STATE_SETUP;
    scene = gMgScene;
    missLimit = 0;
    if (scene != 0)
        missLimit = scene->mCredits;
    if (missLimit >= 5)
        missLimit = 5;
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = (unsigned char)missLimit;
}

// @symbol _ZN14dScMgMemory2_c6RenderEv
/* Vtable slot 9: set the camera, then draw credits, coins, the message,
 * cursor, cards and dealer. */
s32 dScMgMemory2_c::Render() {
    /* pad_4660 is the minigame camera, in dScMgSingle3DBase_c's padding. */
    Camera_InitDealerView((char *)pad_4660);
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();
    DrawMessage();
    DrawCursor();
    DrawCards();
    Dealer_Draw((char *)&mShared);
    return 1;
}

// @symbol _ZN14dScMgMemory2_c8BehaviorEv
/* Vtable slot 6: run the current state, then the two per-frame calls every
 * minigame in this family makes. __sinit_ov006_021314e4 fills the state table
 * with StateSetup, StatePlay, StateJudge, StateResult and StateExit. */
s32 dScMgMemory2_c::Behavior() {
    (this->*sMemory2StateTable[mState])();
    MgCoins_Update();
    Dealer_Update((char *)&mShared);
    return 1;
}

// @symbol _ZN14dScMgMemory2_c13InitResourcesEv
/* Vtable slot 0: load the language's card art into both OBJ banks with the
 * shared palette, reset the board, then set up the timer and the dMeter_c.
 * LoadCard2SubBg3 is the shared free function that loads the sub-screen
 * card background. */
s32 dScMgMemory2_c::InitResources() {
    void *art;
    void *palette;

    data_0209d45c = 0x11;
    LoadCard2SubBg3();
    art = LoadFile(sMemory2CardNcgByLanguage[GetGameLanguage()]);
    palette = LoadFile(ASSET_D_2D_MG_CARD_NCL_BIN);
    DecompressLZ16(art, (void *)0x6400000);
    GX::LoadOBJPltt(palette, 0, 0x100);
    DecompressLZ16(art, (void *)0x6600000);
    GXS::LoadOBJPltt(palette, 0, 0x100);
    Deallocate(art);
    Deallocate(palette);
    data_0209d454 = 0x18;
    ResetGame();
    /* 0x4660..0x471c is dScMgSingle3DBase_c's minigame camera. */
    Camera_InitDealerView((char *)this + 0x4660);
    if (Dealer_Load(&mShared) == 0)
        return 0;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    mHudScore = Mg_GetSavedRecord1();
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = 5;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    return 1;
}

// @symbol dScMgMemory2_c_classInit
extern "C" void *dScMgMemory2_c_classInit() {
    return new dScMgMemory2_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_021314e4 shard).
 * The sprite-pointer table plus the five PMF state tables plain-copied
 * from the ROM constants, in retail copy order: the permuted sprite
 * pointers, the five main states, the ten round states, the four result
 * states, the seven card states, then the three player states. Native member
 * pointers emit the 29 eight-byte descriptor constants the initializer copies.
 * The manifest binds those compiler-local constants to the configured pmf_ names.
 * RoundWaitDeal returns a pointer; this existing dispatch table discards it. */
/* The non-actor profile is interleaved with the member-pointer constants. */
extern "C" BaseSpawnInfo<void> g_profile_MG_MEMORY_J = {dScMgMemory2_c_classInit, 0x016b, 0x016b};

extern "C" int sMgCardSprites[];

void *sMemory2CardSprites[23] = {
    (void *)sMgCardSprites[0],  (void *)sMgCardSprites[1],  (void *)sMgCardSprites[2],
    (void *)sMgCardSprites[11], (void *)sMgCardSprites[12], (void *)sMgCardSprites[13],
    (void *)sMgCardSprites[14], (void *)sMgCardSprites[3],  (void *)sMgCardSprites[4],
    (void *)sMgCardSprites[5],  (void *)sMgCardSprites[6],  (void *)sMgCardSprites[9],
    (void *)sMgCardSprites[10], (void *)sMgCardSprites[7],  (void *)sMgCardSprites[8],
    (void *)sMgCardSprites[15], (void *)sMgCardSprites[16], (void *)sMgCardSprites[17],
    (void *)sMgCardSprites[18], (void *)sMgCardSprites[19], (void *)sMgCardSprites[20],
    (void *)sMgCardSprites[21], (void *)sMgCardSprites[22],
};
dScMgMemory2_cState sMemory2StateTable[5] = {
    &dScMgMemory2_c::StateSetup,  &dScMgMemory2_c::StatePlay, &dScMgMemory2_c::StateJudge,
    &dScMgMemory2_c::StateResult, &dScMgMemory2_c::StateExit,
};
dScMgMemory2_cState sMemory2RoundTable[10] = {
    &dScMgMemory2_c::RoundStart,
    &dScMgMemory2_c::RoundDealEasy,
    &dScMgMemory2_c::RoundDealNormal,
    &dScMgMemory2_c::RoundDealHard,
    &dScMgMemory2_c::RoundDealFourth,
    &dScMgMemory2_c::RoundShowCards,
    &dScMgMemory2_c::RoundHideCards,
    &dScMgMemory2_c::RoundReadyCards,
    (dScMgMemory2_cState)&dScMgMemory2_c::RoundWaitDeal,
    &dScMgMemory2_c::RoundReveal,
};
dScMgMemory2_cState sMemory2ResultTable[4] = {
    &dScMgMemory2_c::ResultWait,
    &dScMgMemory2_c::ResultReward,
    &dScMgMemory2_c::ResultTurnCards,
    &dScMgMemory2_c::ResultFinish,
};
dScMgMemory2_cCardState sMemory2CardTable[7] = {
    &dScMgMemory2_c::CardMove,    &dScMgMemory2_c::CardIdle, &dScMgMemory2_c::CardSelect,
    &dScMgMemory2_c::CardFlipUp,  &dScMgMemory2_c::CardWait, &dScMgMemory2_c::CardFlipDown,
    &dScMgMemory2_c::CardFlyAway,
};
dScMgMemory2_cCardState sMemory2PlayerTable[3] = {
    &dScMgMemory2_c::PlayerDrop,
    &dScMgMemory2_c::PlayerMove,
    &dScMgMemory2_c::PlayerWait,
};
