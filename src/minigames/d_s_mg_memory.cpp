//cpp
/* Memory Match. Twelve cards sit on the touch screen; two equal values
 * are a pair, and too many misses ends the round.
 *
 * leftovers:
 *   - func_ and data_ helpers are still the linker names.
 *   - CheckFinished reads mCredits through WordAt. As a plain member the
 *     multiply-add takes its registers the other way round.
 *   - ShuffleCards: mValueCounts[cardValue] hoists the base; the slot is
 *     added to `this` first and the array reached through a cast back to
 *     the class. The card walks (DrawCards, RoundWaitDeal, ShuffleCards)
 *     step a `char *` the same way -- mCards[i] does not match there.
 *   - cstd::atan2 stays the mangled symbol (Fix12<int> by value).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_memory_AssetId {
    ASSET_D_2D_MG_CARD_NCL_BIN = 0x00bb,
    ASSET_D_2D_MG_CARD_NCG_BIN = 0x00ba,
    ASSET_D_2D_MG_CARD_S_NCG_BIN = 0x00bc,
    ASSET_D_2D_MG_CARD_F_NCG_BIN = 0x00b7,
    ASSET_D_2D_MG_CARD_G_NCG_BIN = 0x00b8,
    ASSET_D_2D_MG_CARD_I_NCG_BIN = 0x00b9
};

#include "dScMgMemory_c.h"
#include "common.h"
#include "types.h"
#include "PlayerInput.h"
#include "decl_common.h"
#include "Sound.h"
#include "SpawnInfo.h"
#include "nitro/fx/fx.h"
namespace cstd {
s16 atan2(s32, s32);
}

/* State numbers follow the card PMF table initialized by
 * __sinit_ov006_021311c8. These descriptive names are reconstructed. */
enum MemoryCardState {
    CARD_MOVING = 0,
    CARD_IDLE = 1,
    CARD_SELECTABLE = 2,
    CARD_FLIPPING_UP = 3,
    CARD_WAITING = 4,
    CARD_FLIPPING_DOWN = 5,
    CARD_FLYING_AWAY = 6
};

typedef void (dScMgMemory_c::*dScMgMemory_cState)();
typedef void (dScMgMemory_c::*dScMgMemory_cCardState)(int);
typedef void (dScMgMemory_c::*dScMgMemory_cPlayerState)(int);
extern "C" dScMgMemory_cState sMemoryResultTable[];
extern "C" dScMgMemory_cState sMemoryStateTable[];
extern dScMgMemory_cState sMemoryRoundTable[];
extern dScMgMemory_cCardState sMemoryCardTable[];
/* Player markers use the int-argument handlers, like the card handlers. */
extern "C" dScMgMemory_cPlayerState sMemoryPlayerTable[];

namespace GX {
void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

namespace GXS {
void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

extern "C" {
extern void Mg_DrawScoreTally(void *c, int a, int b, int d);
extern void Mg_DrawBlinkSprite(void *c, int arg1, int arg2);
/* Unused. Deleting this named prototype renumbers @NNN later in the file. */
extern short atan2_hold_d_s_mg_memory_1(int y, int x);
extern short data_02082214[];
void MgCoins_StartLoseOneMore(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 sMemoryCardFrameSprites[];
extern void *sMemoryCardSprites[];
extern unsigned short sMemoryFlipFrameTimes[];
extern void Mg_SaveRecordPair(int a, int b);
extern s16 data_02082214[];
extern u16 *sMemoryCardPositionsPerDifficulty[];
extern int MgCoins_StartPayout(int);
extern short gMgCoinsStatus;
extern void Mg_SetPerfectBanner(void);
extern void Mg_SetSharedState(int c);
extern void Mg_SubmitScore(int self);
extern void Dealer_PlayWin(void *c);
extern void Dealer_StartShuffle(char *c, int unused, short count, int ready);
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern void FreeGfxSlotsById(int x);
void Camera_InitDealerView(char *p);
void Mg_DrawCredits(char *c, int a, int b, int d);
void Dealer_Draw(char *p);
void MgCoins_Update(void);
int Dealer_Update(void *p);
void LoadCard2SubBg3(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int Dealer_Load(void *c);
int Mg_GetSavedRecord0(void);
int Mg_GetSavedRecord1(void);
void MgCoins_Reset(void);
int Dealer_IsIdle(int *p);
void Mg_SetTopScreenYOffset(int v);
void Mg_PlaceBannersOnTopScreen(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

/* Load-bearing: source order, and the opt pragmas on DrawCards and
 * CardFlyAway. Under deferred emission the last pragma setting wins for the
 * whole file; defer_codegen off lets push/pop scope them to one function. */
#pragma defer_codegen off

// @symbol _ZN13dScMgMemory_cD1Ev
// @symbol _ZN13dScMgMemory_cD0Ev
/* Both D1 and D0 come from this one definition. */
dScMgMemory_c::~dScMgMemory_c() {
    /* mDealer is destroyed automatically before the inherited scene. */
}

void dScMgMemory_c::DrawMessage() {
    if (mMessageVisible == 0)
        return;
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
}

void dScMgMemory_c::DrawCursor() {
    if (mCursor.enabled == 0)
        return;
    Mg_DrawBlinkSprite(this, 0xe0, 0xa0);
}

void dScMgMemory_c::UpdateCursor() {
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

void dScMgMemory_c::ShowCursor() {
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 0x80000;
    mCursor.y = 0xa8000;
    mCursor.blinkTimer = 0;
    mCursor.frame = 0;
}

void dScMgMemory_c::HideCursor() {
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

void dScMgMemory_c::PlayerWait(int /* player */) {}

void dScMgMemory_c::PlayerMove(int player) {
    int targetX = player * 16 + 12;
    int dx, dy, angleIndex;

    dx = targetX - (mPlayers[player].x >> 12);
    dy = -44 - (mPlayers[player].y >> 12);

    mPlayers[player].angle = cstd::atan2(dy, dx);

    mPlayers[player].speed += 0x200;

    angleIndex = (u16)mPlayers[player].angle >> 4;
    {
        short trigValue = data_02082214[angleIndex * 2 + 1];
        int speed = mPlayers[player].speed;
        mPlayers[player].x += FX_MUL(trigValue, speed);
    }

    angleIndex = (u16)mPlayers[player].angle >> 4;
    {
        short trigValue = data_02082214[angleIndex * 2];
        int speed = mPlayers[player].speed;
        mPlayers[player].y += FX_MUL(trigValue, speed);
    }

    dx = targetX - (mPlayers[player].x >> 12);
    dy = -44 - (mPlayers[player].y >> 12);

    if (dx < -3)
        return;
    if (dx > 3)
        return;
    if (dy < -3)
        return;
    if (dy > 3)
        return;

    mPlayers[player].x = targetX << 12;
    mPlayers[player].y = -0x2c000;
    mPlayers[player].state = PLAYER_WAIT;
}

void dScMgMemory_c::PlayerDrop(int player) {
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

void dScMgMemory_c::InitPlayers() {
    int markerCount;
    int playerIndex;
    int stagger;

    if (gMgScene != 0)
        markerCount = gMgScene->mCredits;
    else
        markerCount = 0;
    if (markerCount >= 3)
        markerCount = 3;

    playerIndex = 0;
    mMaxMisses = (unsigned char)markerCount;
    if (markerCount <= 0)
        return;

    stagger = 0;
    do {
        mPlayers[playerIndex].active = 1;
        mPlayers[playerIndex].state = PLAYER_DROP;
        mPlayers[playerIndex].delay = stagger;
        playerIndex++;
        stagger += 0x10;
    } while (playerIndex < markerCount);
}

inline int *WordAt(char *p) {
    return (int *)p;
}

void dScMgMemory_c::CheckFinished() {
    if (mPairsFound >= mTargetPairs) {
        mResultTimer = 0x40;
        mState = STATE_RESULT;
        mSubstate = RESULT_WAIT;
        /* Read mCredits through WordAt: as a plain member the multiply-add takes
       its registers the other way round. */
        Mg_SaveRecordPair(*WordAt((char *)&mCredits) + (3 - mMisses) * 6, mHudScore + 1);
    }
    if (mMisses >= mMaxMisses) {
        mResultTimer = 0x40;
        mState = STATE_RESULT;
        mSubstate = RESULT_WAIT;
        Mg_SaveRecordPair(mCredits, mHudScore - 1);
    }
}

void dScMgMemory_c::JudgePair() {
    int first, second;
    unsigned char *firstState, *secondState;
    if (mSelectedCount < 2)
        return;
    first = mSelectedCards[0];
    second = mSelectedCards[1];
    firstState = &mCards[first].state;
    if (*firstState != CARD_WAITING)
        return;
    secondState = &mCards[second].state;
    if (*secondState != CARD_WAITING)
        return;
    if (mCards[first].value == mCards[second].value) {
        mCards[first].active = 0;
        mCards[first].visible = 0;
        mCards[second].active = 0;
        mCards[second].visible = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        mPairsFound++;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        mMisses++;
        MgCoins_StartLoseOneMore();
        Sound::PlayBank2_2D(0x13e);
        if (mMisses < mMaxMisses) {
            *firstState = CARD_FLIPPING_DOWN;
            *secondState = CARD_FLIPPING_DOWN;
            mSelectedCount = 0;
        }
    }
}

#pragma push
#pragma opt_propagation off
void dScMgMemory_c::DrawCards() {
    char *raw = (char *)this;
    int cardIndex;
    int spriteIndex, dimmed;
    char *cardBase;

    /* sMemoryCardFrameSprites is indexed as [value][frame], five frames per value. */
    cardBase = raw;
    cardIndex = 0;
    do {
        dScMgMemory_c *card = (dScMgMemory_c *)cardBase;
        if (card->mCards[0].visible != 0) {
            dimmed = 0;
            if (mState != STATE_JUDGE_PAIRS)
                dimmed = 1;
            spriteIndex =
                sMemoryCardFrameSprites[card->mCards[0].value * 5 + card->mCards[0].frame];
            Hud_RenderSprite(sMemoryCardSprites[spriteIndex], card->mCards[0].x >> 12,
                             card->mCards[0].y >> 12, -1, dimmed);
        }
        cardIndex += 1;
        cardBase += 0x18;
    } while (cardIndex < 12);
}

#pragma pop

void dScMgMemory_c::UpdateCards() {
    for (int cardIndex = 0; cardIndex < 12; cardIndex++) {
        if (mCards[cardIndex].active)
            (this->*sMemoryCardTable[mCards[cardIndex].state])(cardIndex);
    }
}

#pragma push
#pragma opt_lifetimes off
void dScMgMemory_c::CardFlyAway(int cardIndex) {
    int j;
    int n;
    unsigned char *started;
    int *cardX;

    cardX = &mCards[cardIndex].x;
    started = &mCards[cardIndex].flyAwayStarted;
    *cardX -= 0x10000;
    if (*started == 0) {
        if (mDifficulty == 1) {
            if (cardIndex == 6 || cardIndex == 0xb) {
                *started = 1;
            }
            n = (cardIndex >= 7) ? 0xc - cardIndex : 7 - cardIndex;
            for (j = 1; j < n; j++) {
                if (mCards[cardIndex + j].active != 0) {
                    if (((mCards[cardIndex].x - mCards[cardIndex + j].x) >> 12) <= 4) {
                        *started = 1;
                        mCards[cardIndex + j].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        } else {
            if ((cardIndex & 3) == 3) {
                *started = 1;
            }
            j = 4 - (cardIndex & 3);
            for (n = 1; n < j; n++) {
                if (mCards[cardIndex + n].active != 0) {
                    if (((mCards[cardIndex].x - mCards[cardIndex + n].x) >> 12) <= 4) {
                        *started = 1;
                        mCards[cardIndex + n].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
    }
    if ((*cardX >> 12) > -0x18) {
        return;
    }
    mCards[cardIndex].active = 0;
    mCards[cardIndex].visible = 0;
}

#pragma pop

void dScMgMemory_c::CardFlipDown(int cardIndex) {
    unsigned short timer;
    mCards[cardIndex].animTimer += 1;
    timer = mCards[cardIndex].animTimer;
    if (timer < (sMemoryFlipFrameTimes[mCards[cardIndex].frame] & 0xff))
        return;
    mCards[cardIndex].animTimer = 0;
    mCards[cardIndex].frame -= 1;
    if (mCards[cardIndex].frame == 0)
        mCards[cardIndex].state = CARD_SELECTABLE;
}

void dScMgMemory_c::CardWait(int /* card */) {}

void dScMgMemory_c::CardFlipUp(int card) {
    ++mCards[card].animTimer;
    if (mCards[card].animTimer < (sMemoryFlipFrameTimes[mCards[card].frame] & 0xff))
        return;
    mCards[card].animTimer = 0;
    ++mCards[card].frame;
    if (mCards[card].frame > 4) {
        mCards[card].frame = 4;
        mCards[card].state = CARD_WAITING;
    }
}

void dScMgMemory_c::CardSelect(int card) {
    unsigned int selectionIndex = mSelectedCount;
    unsigned int playerSlot;
    int touchPressed;
    int touchDeltaX;
    int touchDeltaY;
    if (selectionIndex >= 2) {
        return;
    }
    playerSlot = gActivePlayerSlot;
    touchPressed = 0;
    if (gTouchHeld[gActivePlayerSlot * 4] != 0) {
        if (gTouchEdge[gActivePlayerSlot * 4] != 0) {
            touchPressed = 1;
        }
    }
    if (touchPressed == 0) {
        return;
    }
    touchDeltaX = gTouchX[playerSlot * 4] - (mCards[card].x >> 12);
    touchDeltaY = gTouchY[playerSlot * 4] - (mCards[card].y >> 12);
    if (touchDeltaX < (-0x10)) {
        return;
    }
    if (touchDeltaX > 0x10) {
        return;
    }
    if (touchDeltaY < (-0x16)) {
        return;
    }
    if (touchDeltaY > 0x16) {
        return;
    }
    mSelectedValues[selectionIndex] = mCards[card].value;
    mSelectedCards[mSelectedCount] = (u8)card;
    ++mSelectedCount;
    mCards[card].state = CARD_FLIPPING_UP;
    func_02012718(0x143, mCards[card].x);
    if (mInputSeen != 0) {
        return;
    }
    ++mInputSeen;
    Mg_SaveRecordPair(mCredits, mHudScore);
}

void dScMgMemory_c::CardIdle(int /* card */) {}

void dScMgMemory_c::CardMove(int card) {
    int positionIndex = card * 2;
    u16 *positions = sMemoryCardPositionsPerDifficulty[mDifficulty];
    int dx, dy, targetX, targetY;
    targetX = positions[positionIndex];
    dx = targetX - (mCards[card].x >> 12);
    targetY = positions[positionIndex + 1];
    dy = targetY - (mCards[card].y >> 12);

    mCards[card].angle = (u16)cstd::atan2(dy, dx);

    {
        s16 trigValue = data_02082214[(((u16)mCards[card].angle >> 4) << 1) + 1];
        mCards[card].x += (int)(((s64)trigValue * mCards[card].speed + 0x800) >> 0xc);
    }
    {
        s16 trigValue = data_02082214[((u16)mCards[card].angle >> 4) << 1];
        mCards[card].y += (int)(((s64)trigValue * mCards[card].speed + 0x800) >> 0xc);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    mCards[card].x = targetX << 12;
    mCards[card].y = targetY << 12;
    mCards[card].state = CARD_IDLE;
}

void dScMgMemory_c::ResultFinish() {
    s32 playerSlot;
    s32 fiveCardRow;
    s32 fiveCardColumn;
    s32 fourCardRow;
    s32 fourCardColumn;
    s32 touchPressed;
    u16 *timer;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0) {
        timer = &mCardTimer;
        *timer = (u16)(*timer - 1);
        return;
    }

    playerSlot = gActivePlayerSlot;
    touchPressed = 0;
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
        if (mDifficulty == 1) {
            for (fiveCardRow = 0; fiveCardRow < 2; fiveCardRow++) {
                for (fiveCardColumn = 0; fiveCardColumn < 5; fiveCardColumn++) {
                    if (mCards[fiveCardRow * 5 + fiveCardColumn + 2].active != 0) {
                        mCards[fiveCardRow * 5 + fiveCardColumn + 2].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        } else {
            for (fourCardRow = 0; fourCardRow < 3; fourCardRow++) {
                for (fourCardColumn = 0; fourCardColumn < 4; fourCardColumn++) {
                    if (mCards[fourCardRow * 4 + fourCardColumn].active != 0) {
                        mCards[fourCardRow * 4 + fourCardColumn].state = CARD_FLYING_AWAY;
                        break;
                    }
                }
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = STATE_EXIT;
}

void dScMgMemory_c::ResultTurnCards() {
    int flipped;
    int cardIndex;
    if (mCardTimer != 0) {
        unsigned short *timer = &mCardTimer;
        *timer = *timer - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = STATE_EXIT;
        mCardTimer = 0;
        return;
    }
    flipped = 0;
    for (cardIndex = 0; cardIndex < 0xc; cardIndex++) {
        if (mCards[cardIndex].active != 0) {
            if (mCards[cardIndex].state == CARD_SELECTABLE) {
                mCards[cardIndex].state = CARD_FLIPPING_UP;
                flipped++;
            }
        }
    }
    if (flipped <= 2)
        Sound::PlayBank2_2D(0x145);
    else
        Sound::PlayBank2_2D(0x146);
    mSubstate = RESULT_FINISH;
    ShowCursor();
    mCardTimer = 0x20;
}

void dScMgMemory_c::ResultReward() {
    if (mCardTimer != 0) {
        --mCardTimer;
        if (mCardTimer != 0)
            return;
        if (mPairsFound < mTargetPairs)
            return;
        MgCoins_StartPayout(6 * (3 - mMisses));
        return;
    }
    if (gMgCoinsStatus == 1) {
        mCardTimer = 0;
        mSubstate = RESULT_TURN_CARDS;
    }
}

void dScMgMemory_c::ResultWait() {
    if (mResultTimer != 0) {
        mResultTimer -= 1;
        if ((short)mResultTimer <= 0) {
            mResultTimer = 0;
            if (mPairsFound >= mTargetPairs) {
                if (mMisses == 0)
                    Mg_SetPerfectBanner();
                Mg_SetSharedState(MG_STATE_WIN);
                if (mHudScore < 0x270f) {
                    int *score = &mHudScore;
                    *score += 1;
                }
                if (mHudScore > mBestHudScore)
                    mBestHudScore = mHudScore;
                Mg_SubmitScore(gMgScene ? gMgScene->mHudScore : 0);
                Dealer_PlayWin(&mDealer);
            } else {
                Mg_SetSharedState(MG_STATE_LOSE);
                if (mHudScore > 0) {
                    int *score = &mHudScore;
                    *score -= 1;
                }
                Dealer_PlayLose(&mDealer);
            }
        }
    } else {
        mPromptEnabled = 0;
        mCardTimer = 0x10;
    }
}

void dScMgMemory_c::RoundReveal() {
    int cardIndex;
    unsigned short *timer;
    if (mCardTimer != 0) {
        timer = &mCardTimer;
        *timer = *timer - 1;
        if ((short)mCardTimer > 0)
            return;
        InitPlayers();
        return;
    }
    for (cardIndex = 0; cardIndex < 0xc; cardIndex++) {
        mCards[cardIndex].state = CARD_SELECTABLE;
    }
    mSubstate = ROUND_START;
    mState = STATE_JUDGE_PAIRS;
}

/* Round dispatch ignores the result. A void return matches its native PMF
 * contract and preserves the complete emitted body; the historical return
 * type cannot be recovered from the unused value left in r0. */
void dScMgMemory_c::RoundWaitDeal() {
    char *raw = (char *)this;
    int waiting = 0;
    int cardIndex = 0;
    char *cardBase = raw;
    for (; cardIndex < 0xc;) {
        dScMgMemory_c *card = (dScMgMemory_c *)cardBase;
        if (card->mCards[0].active != 0) {
            if (card->mCards[0].state != CARD_IDLE) {
                waiting++;
                break;
            }
        }
        cardIndex++;
        cardBase += 0x18;
    }
    if (waiting != 0)
        return;
    cardBase = raw;
    cardIndex = 0;
    {
        unsigned char selectState = CARD_SELECTABLE;
        for (; cardIndex < 0xc;) {
            ((dScMgMemory_c *)cardBase)->mCards[0].state = selectState;
            cardIndex++;
            cardBase += 0x18;
        }
    }
    {
        mSubstate = ROUND_START;
        mState = STATE_JUDGE_PAIRS;
        return;
    }
}

void dScMgMemory_c::RoundDealHard() {
    if (mDealSignals < 3)
        return;
    mCards[11 - mCardsDealt].active = 1;
    ++mCardsDealt;
    if (mCardsDealt >= 12)
        mSubstate = ROUND_WAIT_DEAL;
}

void dScMgMemory_c::RoundDealNormal() {
    u8 difficulty;
    if (mDealSignals < 2)
        return;
    mCards[0xb - mCardsDealt].active = 1;
    mCardsDealt++;
    difficulty = mDifficulty;
    if (difficulty == 1) {
        if (mCardsDealt >= 10)
            mSubstate = ROUND_WAIT_DEAL;
        return;
    }
    if (mCardsDealt < 8)
        return;
    if (difficulty != 0)
        mSubstate = ROUND_DEAL_HARD;
    else
        mSubstate = ROUND_WAIT_DEAL;
}

void dScMgMemory_c::RoundDealEasy() {
    if (mDealSignals < 1)
        return;
    mCards[11 - mCardsDealt].active = 1;
    ++mCardsDealt;
    if (mDifficulty == 1) {
        if (mCardsDealt >= 5)
            mSubstate = ROUND_DEAL_NORMAL;
    } else {
        if (mCardsDealt >= 4)
            mSubstate = ROUND_DEAL_NORMAL;
    }
}

void dScMgMemory_c::RoundStart() {
    mDealSignals = 0;
    mCardsDealt = 0;
    if (mDifficulty >= 2)
        Dealer_StartShuffle((char *)&mDealer, 4, 3, (int)&mDealSignals);
    else
        Dealer_StartShuffle((char *)&mDealer, 4, 2, (int)&mDealSignals);
    mDealer.mReserved = 1;
    mSubstate = ROUND_DEAL_EASY;
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

void dScMgMemory_c::ShuffleCards() {
    char *raw = (char *)this;
    int cardValue;
    int valueIndex;
    u8 *valueCount;
    int cardIndex;
    char *cardBase;

    for (cardIndex = 0; cardIndex < 7; cardIndex++)
        mValueCounts[cardIndex] = 0;

    if (mDifficulty == 0) {
        cardBase = raw + 0x60;
        for (valueIndex = 4; valueIndex < 12; valueIndex++) {
            cardValue =
                (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            for (;;) {
                /* mValueCounts[cardValue] hoists the base. Same spelling below. */
                valueCount = &((dScMgMemory_c *)(raw + cardValue))->mValueCounts[0];
                if (*valueCount < 2) {
                    ((dScMgMemory_c *)cardBase)->mCards[0].value = cardValue;
                    *valueCount += 1;
                    break;
                }
                cardValue =
                    (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            }
            cardBase += 0x18;
        }
        cardBase = raw + 0x60;
        for (cardIndex = 4; cardIndex < 12; cardIndex++) {
            ((dScMgMemory_c *)cardBase)->mCards[0].x = 0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].y = -0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].speed = 0x8000;
            ((dScMgMemory_c *)cardBase)->mCards[0].visible = 1;
            ((dScMgMemory_c *)cardBase)->mCards[0].state = CARD_MOVING;
            cardBase += 0x18;
        }
    } else if (mDifficulty == 1) {
        cardBase = raw + 0x30;
        for (cardIndex = 2; cardIndex < 12; cardIndex++) {
            cardValue =
                (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            for (;;) {
                valueCount = &((dScMgMemory_c *)(raw + cardValue))->mValueCounts[0];
                if (*valueCount < 2) {
                    ((dScMgMemory_c *)cardBase)->mCards[0].value = cardValue;
                    *valueCount += 1;
                    break;
                }
                cardValue =
                    (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            }
            cardBase += 0x18;
        }
        cardBase = raw + 0x30;
        for (cardIndex = 2; cardIndex < 12; cardIndex++) {
            ((dScMgMemory_c *)cardBase)->mCards[0].x = 0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].y = -0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].speed = 0x8000;
            ((dScMgMemory_c *)cardBase)->mCards[0].visible = 1;
            ((dScMgMemory_c *)cardBase)->mCards[0].state = CARD_MOVING;
            cardBase += 0x18;
        }
    } else {
        cardBase = raw;
        for (cardIndex = 0; cardIndex < 12; cardIndex++) {
            cardValue =
                (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            for (;;) {
                valueCount = &((dScMgMemory_c *)(raw + cardValue))->mValueCounts[0];
                if (*valueCount < 2) {
                    ((dScMgMemory_c *)cardBase)->mCards[0].value = cardValue;
                    *valueCount += 1;
                    break;
                }
                cardValue =
                    (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            }
            cardBase += 0x18;
        }
        cardBase = raw;
        for (cardIndex = 0; cardIndex < 12; cardIndex++) {
            ((dScMgMemory_c *)cardBase)->mCards[0].x = 0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].y = -0x80000;
            ((dScMgMemory_c *)cardBase)->mCards[0].speed = 0x8000;
            ((dScMgMemory_c *)cardBase)->mCards[0].visible = 1;
            ((dScMgMemory_c *)cardBase)->mCards[0].state = CARD_MOVING;
            cardBase += 0x18;
        }
    }
}

void dScMgMemory_c::ResetGame() {
    int i;
    int j;
    i = 0;
    do {
        mCards[i].x = 0;
        mCards[i].y = 0;
        mCards[i].speed = 0;
        mCards[i].angle = 0;
        mCards[i].animTimer = 0;
        mCards[i].value = 0;
        mCards[i].unk_11 = 0;
        mCards[i].visible = 0;
        mCards[i].active = 0;
        mCards[i].state = CARD_MOVING;
        mCards[i].frame = 0;
        mCards[i].flyAwayStarted = 0;
        i++;
    } while (i < 12);
    j = 0;
    do {
        mPlayers[j].x = 0;
        mPlayers[j].y = 0;
        mPlayers[j].speed = 0;
        mPlayers[j].delay = 0;
        mPlayers[j].angle = 0;
        mPlayers[j].active = 0;
        mPlayers[j].unk_11 = 0;
        mPlayers[j].state = PLAYER_DROP;
        j++;
    } while (j < 3);
    mCursor.visible = 0;
    mCursor.enabled = 0;
    mCursor.frame = 0;
    mState = STATE_SETUP;
    mSubstate = ROUND_START;
    mRoundTimer = 0;
    mCardTimer = 0;
    mResultTimer = 0;
    mPairsFound = 0;
    mSelectedCount = 0;
    unk_5339 = 0;
    mMisses = 0;
    i = 0;
    do {
        mSelectedValues[i] = 0;
        mSelectedCards[i] = 0;
        i++;
    } while (i < 2);
    j = 0;
    do {
        mValueCounts[j] = 0;
        j++;
    } while (j < 7);
    mMessageVisible = 1;
    mInputSeen = 0;
}

void dScMgMemory_c::StateExit() {
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

void dScMgMemory_c::StateResult() {
    (this->*sMemoryResultTable[mSubstate])();
}

void dScMgMemory_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

void dScMgMemory_c::StatePlay() {
    (this->*sMemoryRoundTable[mSubstate])();
    UpdateCards();
}

void dScMgMemory_c::StateSetup() {
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    Dealer_PlayIdle((char *)&mDealer);
    mState = STATE_PLAY;
}

void dScMgMemory_c::SetupDifficulty() {
    mDifficulty = 0;
    mTargetPairs = 4;
    int score = mHudScore;
    if (score >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 6;
        return;
    }
    if (score >= 5) {
        mDifficulty = 1;
        mTargetPairs = 5;
    }
}

// @symbol _ZN13dScMgMemory_c10OnDealDoneEv
void dScMgMemory_c::OnDealDone() {
    MgCoins_PlaceBet(mMaxMisses);
}

// @symbol _ZN13dScMgMemory_c11CanEndStateEi
int dScMgMemory_c::CanEndState(int /* mode */) {
    if (mState == STATE_RESULT && mSubstate == RESULT_WAIT) {
        if (Dealer_IsIdle((int *)&mDealer) == 0)
            return 0;
        mSubstate = RESULT_REWARD;
        FreeGfxSlotsById(0x1d);
    }
    return mState == STATE_EXIT;
}

// @symbol _ZN13dScMgMemory_c10StartRoundEi
/* Slot 18: restarts the game, with up to three misses allowed from the meter. */

void dScMgMemory_c::StartRound(int /* arg */) {
    dScMgBase_c *scene;
    int missLimit;
    ResetGame();
    mState = STATE_SETUP;
    scene = gMgScene;
    missLimit = 0;
    if (scene != 0)
        missLimit = scene->mCredits;
    if (missLimit >= 3)
        missLimit = 3;
    ((void (*)(void *))MgCoins_Reset)(scene);
    gMgCoinsShowDelta = 1;
    mMaxMisses = (unsigned char)missLimit;
}

// @symbol _ZN13dScMgMemory_c6RenderEv
/* Slot 9. */
s32 dScMgMemory_c::Render() {
    Camera_InitDealerView((char *)pad_4660);
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();
    DrawMessage();
    DrawCursor();
    DrawCards();
    Dealer_Draw((char *)&mDealer);
    return 1;
}

// @symbol _ZN13dScMgMemory_c8BehaviorEv
/* Slot 6: runs the current state from the table, indexed by mState. */
s32 dScMgMemory_c::Behavior() {
    (this->*sMemoryStateTable[mState])();
    MgCoins_Update();
    Dealer_Update(&mDealer);
    return 1;
}

// @symbol _ZN13dScMgMemory_c13InitResourcesEv
/* Slot 0. */
s32 dScMgMemory_c::InitResources() {
    void *tiles;
    void *palette;

    data_0209d45c = 0x11;
    LoadCard2SubBg3();
    tiles = LoadFile(sMemoryCardNcgByLanguage[GetGameLanguage()]);
    palette = LoadFile(ASSET_D_2D_MG_CARD_NCL_BIN);
    DecompressLZ16(tiles, (void *)0x6400000);
    GX::LoadOBJPltt(palette, 0, 0x100);
    DecompressLZ16(tiles, (void *)0x6600000);
    GXS::LoadOBJPltt(palette, 0, 0x100);
    Deallocate(tiles);
    Deallocate(palette);
    data_0209d454 = 0x18;
    ResetGame();
    Camera_InitDealerView((char *)pad_4660);
    if (Dealer_Load(&mDealer) == 0)
        return 0;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    mHudScore = Mg_GetSavedRecord1();
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mMaxMisses = 3;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    return 1;
}

// @symbol dScMgMemory_c_classInit
/* `new` inlines the implicit constructor: the base chain, this class's
   vptr, then mDealer. */
extern "C" dScMgMemory_c *dScMgMemory_c_classInit() {
    return new dScMgMemory_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_021311c8 shard).
 * The sprite-pointer table plus the five PMF state tables plain-copied
 * from native member-pointer constants, in retail copy order: the permuted sprite
 * pointers, the five main states, the six round states, the four result
 * states, the seven card states, then the three player states. */
extern "C" int sMgCardSprites[];

void *sMemoryCardSprites[15] = {
    (void *)sMgCardSprites[0],  (void *)sMgCardSprites[1],  (void *)sMgCardSprites[2],
    (void *)sMgCardSprites[11], (void *)sMgCardSprites[12], (void *)sMgCardSprites[13],
    (void *)sMgCardSprites[14], (void *)sMgCardSprites[3],  (void *)sMgCardSprites[4],
    (void *)sMgCardSprites[5],  (void *)sMgCardSprites[6],  (void *)sMgCardSprites[9],
    (void *)sMgCardSprites[10], (void *)sMgCardSprites[7],  (void *)sMgCardSprites[8],
};
dScMgMemory_cState sMemoryStateTable[5] = {
    &dScMgMemory_c::StateSetup,  &dScMgMemory_c::StatePlay, &dScMgMemory_c::StateJudge,
    &dScMgMemory_c::StateResult, &dScMgMemory_c::StateExit,
};
dScMgMemory_cState sMemoryRoundTable[6] = {
    &dScMgMemory_c::RoundStart,    &dScMgMemory_c::RoundDealEasy, &dScMgMemory_c::RoundDealNormal,
    &dScMgMemory_c::RoundDealHard, &dScMgMemory_c::RoundWaitDeal, &dScMgMemory_c::RoundReveal,
};
dScMgMemory_cState sMemoryResultTable[4] = {
    &dScMgMemory_c::ResultWait,
    &dScMgMemory_c::ResultReward,
    &dScMgMemory_c::ResultTurnCards,
    &dScMgMemory_c::ResultFinish,
};
dScMgMemory_cCardState sMemoryCardTable[7] = {
    &dScMgMemory_c::CardMove,    &dScMgMemory_c::CardIdle, &dScMgMemory_c::CardSelect,
    &dScMgMemory_c::CardFlipUp,  &dScMgMemory_c::CardWait, &dScMgMemory_c::CardFlipDown,
    &dScMgMemory_c::CardFlyAway,
};
dScMgMemory_cPlayerState sMemoryPlayerTable[3] = {
    &dScMgMemory_c::PlayerDrop,
    &dScMgMemory_c::PlayerMove,
    &dScMgMemory_c::PlayerWait,
};

extern "C" BaseSpawnInfo<dScMgMemory_c> g_profile_MG_MEMORY = {dScMgMemory_c_classInit, 0x016a,
                                                               0x016a};

/* Twelve interleaved screen x/y pairs per difficulty. The flat indexing
 * in CardMove preserves the pinned compiler's measured address formation. */
extern "C" {
u16 sMemoryCardPositionsEasy[24] = {224, 160, 160, 160, 96,  160, 32,  160, 224, 136, 160, 136,
                                    96,  136, 32,  136, 224, 64,  160, 64,  96,  64,  32,  64};
u16 sMemoryCardPositionsNormal[24] = {200, 160, 136, 160, 224, 136, 176, 136, 128, 136, 80, 136,
                                      32,  136, 224, 64,  176, 64,  128, 64,  80,  64,  32, 64};
u16 sMemoryCardPositionsHard[24] = {209, 160, 155, 160, 101, 160, 47,  160, 209, 104, 155, 104,
                                    101, 104, 47,  104, 209, 48,  155, 48,  101, 48,  47,  48};
u16 *sMemoryCardPositionsPerDifficulty[3] = {sMemoryCardPositionsEasy, sMemoryCardPositionsNormal,
                                             sMemoryCardPositionsHard};
/* Frames0..4 are used by the flip handlers. The sixth stored value is
 * also24; its original purpose is unknown. */
u16 sMemoryFlipFrameTimes[6] = {8, 6, 4, 6, 24, 24};
int sMemoryCardNcgByLanguage[5] = {ASSET_D_2D_MG_CARD_NCG_BIN, ASSET_D_2D_MG_CARD_S_NCG_BIN,
                                   ASSET_D_2D_MG_CARD_F_NCG_BIN, ASSET_D_2D_MG_CARD_G_NCG_BIN,
                                   ASSET_D_2D_MG_CARD_I_NCG_BIN};
/* Seven card values, five flip frames each, indexed by value*5+frame. */
u16 sMemoryCardFrameSprites[35] = {0, 0, 0, 0, 0, 0,  1, 2, 4, 3, 0,  1,  2, 6, 5, 0,  1, 2,
                                   8, 7, 0, 1, 2, 10, 9, 0, 1, 2, 12, 11, 0, 1, 2, 14, 13};
}
