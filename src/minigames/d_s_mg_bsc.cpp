//cpp
/* ov006/dScMgBSC_c -- the Lucky Stars card minigame, 20 functions.
 *
 * Functions run in REVERSE of ROM order; do not reorder.
 *
 * leftovers:
 *   - most data_ globals still have address-based names.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_bsc_AssetId { ASSET_D_2D_MG_BIG_SMALL_NCL_BIN = 0x00b3 };

#include "dScMgBSC_c.h"
#include "nitro/hw/registers.h"
#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_bgcnt.h"
namespace s20b1e34 { void Mg_DrawScoreTally(void*, int, int, int); }
using s20b1e34::Mg_DrawScoreTally;
namespace s20b1bc8 { void Mg_DrawCredits(char *a0, int, int, int); }
using s20b1bc8::Mg_DrawCredits;
namespace s20b0a38 { void Mg_ResumeSharedState(void); }
using s20b0a38::Mg_ResumeSharedState;
namespace s20b0aa0 { void FreeGfxSlotsById(int); }
using s20b0aa0::FreeGfxSlotsById;
namespace s20af68c { void Hud_RenderSprite(void *attr, int, int, int, int); }
using s20af68c::Hud_RenderSprite;
namespace s20ad674 { s32 GetGameLanguage(void); }
using s20ad674::GetGameLanguage;
void MgCoins_Reset(void);
void MgCoins_Update(void);
void MgCoins_StartLose(void);
void MgCoins_StartPayout(int);

/* The state table: one pointer-to-member per state, indexed by mState.
 * Declared outside extern "C" it still links by its plain name. */
typedef void (dScMgBSC_c::*StateHandler)();
extern StateHandler sBSCStateTable[];
/* The ROM PMF constants the state table copies from (unlicensed .data),
 * in retail copy order. */
extern StateHandler pmf_dScMgBSC_c_StateDealCards;
extern StateHandler pmf_dScMgBSC_c_StateWaitDeal;
extern StateHandler pmf_dScMgBSC_c_StateChooseCard;
extern StateHandler pmf_dScMgBSC_c_StateMoveCards;
extern StateHandler pmf_dScMgBSC_c_StateWaitMove;
extern StateHandler pmf_dScMgBSC_c_StateRevealCards;
extern StateHandler pmf_dScMgBSC_c_StateJudgeCards;
extern StateHandler pmf_dScMgBSC_c_StateTableReaction;
extern StateHandler pmf_dScMgBSC_c_StateTableReaction_2;
extern StateHandler pmf_dScMgBSC_c_StateWaitTable_2;
extern StateHandler pmf_dScMgBSC_c_StateWaitTable;
extern StateHandler pmf_dScMgBSC_c_StateReturnCards;
extern StateHandler pmf_dScMgBSC_c_StateDone;
extern StateHandler pmf_dScMgBSC_c_StateDone_2;

extern "C" {
extern short gMgCoinsStatus;
extern int Math_Function_0203b14c(int *ptr, int target, int rate, int limit, int step);
extern int Dealer_IsIdle(Dealer *dealer);
extern int gMgCoinsInPlay;
extern void Dealer_PlayWin(void *c);
/* decl_common.h already declares this with a void pointer, so the
 * definition's char pointer cannot be repeated here. */
extern void Dealer_PlayLose(void *c);
extern int sBscLaneX[];
extern int sMgCoinsBetPlaced;
extern void Dealer_StartShuffle(Dealer *dealer, int unused, short count, s16 *counter);
void Dealer_Draw(void *table);
void Dealer_Update(Dealer *c);
extern unsigned char gMainScreenLayers;
extern int data_0208ee44;
extern void LoadCard2SubBg3(void *);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern void *LoadFile(int handle);
extern int Dealer_Load(void *c);
}

/* Real C++ declarations for the engine calls below; the namespaces and the
 * reference parameter are what give the ROM's mangled names. */
namespace Sound {
void PlayBank2_2D(u32 id);
}

/* Moves value toward target by step; nonzero once it gets there. */

/* `new` inlines the implicit constructor: the base chain, this class's
   vptr, then mDealer and mCardPos in declaration order. */
extern "C" void *dScMgBSC_c_classInit() {
    return new dScMgBSC_c;
}

/* Slot 0, and the key function: the destructor is inline in the header, so
 * this is the first virtual defined out of line, and defining it here emits
 * the vtable and the D1 and D0 pair. The last call dispatches virtually
 * (mwcc does not devirtualize it). */
s32 dScMgBSC_c::InitResources() {
    void *paletteData;
    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    LoadCard2SubBg3(this);
    LoadCompressedFileAt(sBscBigSmallNcgByLanguage[GetGameLanguage()], G2S_GetOBJCharPtr());
    paletteData = LoadFile(ASSET_D_2D_MG_BIG_SMALL_NCL_BIN);
    GXS_LoadOBJPltt(paletteData, 0, 0x100);
    Deallocate(paletteData);
    Camera_InitDealerView(&mCamera);
    if (Dealer_Load(&mDealer) == 0)
        return 0;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    data_0208ee44 = 1;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    this->StartRound(3);
    return 1;
}

/* Slot 6: runs the current state from the table. */
s32 dScMgBSC_c::Behavior() {
    (this->*sBSCStateTable[mState])();
    MgCoins_Update();
    Dealer_Update(&mDealer);
    return 1;
}

/* Slot 9. Card positions are 20.12 fixed point. Sprite frames 0 to 2 are
 * the flip; from frame 3 the card's value picks the face. */
s32 dScMgBSC_c::Render() {
    int cardIndex;

    Camera_InitDealerView(&mCamera);
    if (mState >= STATE_WAIT_DEAL) {
        for (cardIndex = 0; cardIndex < 2; cardIndex++) {
            if (mCardFrame[cardIndex] < 3) {
                Hud_RenderSprite(sBscCardSprites[mCardFrame[cardIndex]],
                                 mCardPos[cardIndex].x >> 12, mCardPos[cardIndex].y >> 12, -1, -1);
            } else {
                Hud_RenderSprite(sBscCardSprites[mCardFrame[cardIndex] + mCardValue[cardIndex] * 2],
                                 mCardPos[cardIndex].x >> 12, mCardPos[cardIndex].y >> 12, -1, -1);
            }
        }
    }
    MgCoins_Draw();
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    Dealer_Draw(&mDealer);
    return 1;
}

/* Slot 18. Mode 3 reads the meter score into mHudScore and posts it; every
   mode then restarts the state machine. The redundant `!= 4 && != 5`
   compares are in the ROM and must stay. */
void dScMgBSC_c::StartRound(int mode) {
    if (mode != 4 && mode != 5 && mode == 3) {
        mHudScore = Mg_GetSavedRecord1();
        Mg_SubmitScore(mHudScore);
    }
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mState = STATE_DEAL_CARDS;
}

/* Slot 19. In state 13, once the table's reaction has finished, mode 4
   moves to state 9 and mode 5 to state 10. When the round has settled
   (state 12, with gMgCoinsStatus set) the final scores go to the
   meter. */
s32 dScMgBSC_c::CanEndState(int mode) {
    int state;
    short coinStatus;
    if (mState == STATE_WAIT_RESULT) {
        if (Dealer_IsIdle(&mDealer) != 0) {
            if (mode == 4) {
                FreeGfxSlotsById(0x1d);
                mState = STATE_WAIT_WIN;
            } else if (mode == 5) {
                FreeGfxSlotsById(0x1d);
                mState = STATE_WAIT_LOSE;
            }
        } else {
            return 0;
        }
    }
    state = mState;
    if (state >= STATE_WAIT_COINS) {
        coinStatus = gMgCoinsStatus;
        if (coinStatus == 1)
            goto settle;
    }
    return 0;
settle:
    if (state == STATE_WAIT_COINS && coinStatus == 1) {
        Mg_SaveRecordPair(mCredits, mHudScore);
    }
    return 1;
}

/* Slot 21: raises the bet to one of four tiers from mHudScore / 5,
   capped at tier 3. */
void dScMgBSC_c::OnDealDone() {
    int score = mHudScore;
    int tier = score / 5;
    if ((unsigned int)tier > 3) {
        tier = 3;
    }
    MgCoins_RaiseBet(tier + 1);
}

/* State 0: deals two cards with distinct random values out of five
 * (valueUsed marks the values taken), starts the deal animation and the
 * prompt, and moves to StateWaitDeal. */
void dScMgBSC_c::StateDealCards() {
    unsigned char valueUsed[5];
    int valueIndex;
    int cardIndex;
    unsigned char cardValue;

    for (valueIndex = 0; valueIndex < 5; valueIndex++)
        valueUsed[valueIndex] = 0;

    for (cardIndex = 0; cardIndex < 2; cardIndex++) {
        mCardPos[cardIndex].x = 0x80000;
        mCardPos[cardIndex].y = -0x40000;
        cardValue = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) % 5u;
        for (;;) {
            if (valueUsed[cardValue] == 0) {
                mCardValue[cardIndex] = cardValue;
                valueUsed[cardValue] = 1;
                break;
            }
            cardValue++;
            cardValue %= 5;
        }
        mCardFrame[cardIndex] = 0;
    }

    mOtherCard = 0;
    mChosenCard = 1;
    mCardsDealt = 0;
    Dealer_StartShuffle(&mDealer, 3, 2, &mCardsDealt);

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    mState += 1;
}

/* State 1: slides each card to its lane (x toward
 * sBscLaneX, y toward 0x60000). The dealer animation increments
 * mCardsDealt as cards are dealt; the loop reloads that count each pass.
 * Moves on once both cards are in place. */
void dScMgBSC_c::StateWaitDeal() {
    int cardIndex;
    int cardsArrived;
    int xReached;

    cardsArrived = 0;
    for (cardIndex = 0; cardIndex < mCardsDealt; cardIndex++) {
        if (Math_Function_0203b14c(&mCardPos[cardIndex].x, sBscLaneX[cardIndex], 0x800, 0x10000,
                                   0x200) == 0)
            xReached = 1;
        else
            xReached = 0;

        if (Math_Function_0203b14c(&mCardPos[cardIndex].y, 0x60000, 0x800, 0x10000, 0x200) == 0) {
            if (xReached != 0)
                cardsArrived++;
        }
    }

    if (cardsArrived < 2)
        return;
    mState++;
}

/* State 2: waits for a touch within 16 pixels of a lane's x and 24 of y
 * 0x60. The touched card becomes mChosenCard and the machine moves on. */
void dScMgBSC_c::StateChooseCard() {
    unsigned int playerSlot = gActivePlayerSlot;
    int touchPressed = 0;
    int lane;
    if (gTouchHeld[playerSlot * 4] != 0) {
        if (gTouchEdge[playerSlot * 4] != 0)
            touchPressed = 1;
    }
    if (touchPressed == 0)
        return;
    for (lane = 0; lane < 2; lane++) {
        int dx = (int)((TouchInfo *)gTouchHeld)[gActivePlayerSlot].x - (sBscLaneX[lane] >> 12);
        int dy = (int)((TouchInfo *)gTouchHeld)[gActivePlayerSlot].y - 0x60;
        if (dx < -0x10)
            continue;
        if (dx > 0x10)
            continue;
        if (dy < -0x18)
            continue;
        if (dy > 0x18)
            continue;
        mChosenCard = lane;
        mOtherCard = lane ^ 1;
        mRevealTick = 0;
        mState += 1;
        sMgCoinsBetPlaced = 1;
        Sound::PlayBank2_2D(0x153);
    }
}

/* State 3: moves the chosen card down and the other up, then primes the
 * 30-frame timer. */
void dScMgBSC_c::StateMoveCards() {
    int chosenArrived = ApproachLinear(mCardPos[mChosenCard].y, 0x90000, 0x4000) & 1;
    int otherArrived = ApproachLinear(mCardPos[mOtherCard].y, 0x30000, 0x4000) & 1;

    if (chosenArrived == 0)
        return;
    if (otherArrived == 0)
        return;

    mStateTimer = 0x1e;
    mState += 1;
}

/* State 4: waits out the timer, then plays the move sound. */
void dScMgBSC_c::StateWaitMove() {
    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;
    Sound::PlayBank2_2D(0x148);
    mState = mState + 1;
}

/* State 5: flips both cards, one frame every four ticks, until the other
 * card is face up. Then settles the stakes: the higher card wins, which
 * adds twice gMgCoinsInPlay to mCredits and one to the score; a loss
 * takes one away. */
void dScMgBSC_c::StateRevealCards() {
    int cardIndex;
    mRevealTick += 1;
    if (mRevealTick < 4)
        return;
    mRevealTick = 0;
    for (cardIndex = 0; cardIndex < 2; cardIndex++)
        mCardFrame[cardIndex] += 1;
    if (mCardFrame[mOtherCard] < 4)
        return;
    mStateTimer = 0x1e;
    mState += 1;
    mPromptEnabled = 0;
    {
        u8 chosenValue = mCardValue[mChosenCard];
        u8 otherValue = mCardValue[mOtherCard];
        int credits = mCredits;
        int score = mHudScore;
        if (chosenValue > otherValue) {
            credits += gMgCoinsInPlay * 2;
            score = score + 1;
        } else {
            score = score - 1;
        }
        Mg_SaveRecordPair(credits, score);
    }
}

/* State 6: 30 frames after the flip, bumps the score (capped at 9999) and
 * the best score on a win, plays jingle 4 or 5, queues the table's win or
 * loss reaction, and parks in state 13. The bump goes through a pointer on
 * purpose: the ROM takes the member's address first. */
void dScMgBSC_c::StateJudgeCards() {
    int *score;

    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;

    if (mCardValue[mChosenCard] > mCardValue[mOtherCard]) {
        if (mHudScore < 0x270f) {
            score = &mHudScore;
            *score = *score + 1;
        }
        if (mHudScore > mBestHudScore)
            mBestHudScore = mHudScore;
        Mg_SubmitScore(mHudScore);
        Mg_SetSharedState(MG_STATE_WIN);
        Dealer_PlayWin(&mDealer);
        mState = STATE_WAIT_RESULT;
    } else {
        if (mHudScore > 0) {
            score = &mHudScore;
            *score = *score - 1;
        }
        Mg_SetSharedState(MG_STATE_LOSE);
        Dealer_PlayLose(&mDealer);
        mState = STATE_WAIT_RESULT;
    }
}

/* States 7 and 8: queue the table's win or loss reaction. Nothing in this
 * file sets state 7 or 8; StateJudgeCards does the same work inline. */
void dScMgBSC_c::StateTableReaction() {
    if (mState == STATE_WIN_REACTION) {
        Dealer_PlayWin(&mDealer);
        mState = STATE_WAIT_WIN;
    } else {
        Dealer_PlayLose(&mDealer);
        mState = STATE_WAIT_LOSE;
    }
}

/* States 9 and 10: wait for the table's reaction to finish, then pay out
 * a win (state 12) or play the loss sting and gather the cards (state 11). */
void dScMgBSC_c::StateWaitTable() {
    if (Dealer_IsIdle(&mDealer) == 0)
        return;
    if (mState == STATE_WAIT_WIN) {
        MgCoins_StartPayout(gMgCoinsInPlay << 1);
        mState = STATE_WAIT_COINS;
    } else {
        MgCoins_StartLose();
        mState = STATE_RETURN_CARDS;
    }
}

/* State 11, while gMgCoinsStatus is set: if the credits are spent,
 * first walks both cards back to the deck (mCardsDealt re-read every pass,
 * as in StateWaitDeal). Then refreshes the meter and settles in state 12. */
void dScMgBSC_c::StateReturnCards() {
    int cardsReturned;
    int cardIndex;
    Vector2 *cardPos;

    if (gMgCoinsStatus != 1) {
        return;
    }
    if (mCredits <= 0) {
        cardsReturned = 0;
        cardIndex = 0;
        if (mCardsDealt > 0) {
            cardPos = &mCardPos[0];
            do {
                if (Math_Function_0203b14c((int *)cardPos, -0x40000, 0x800, 0xc000, 0x200) == 0) {
                    cardsReturned++;
                }
                cardIndex++;
                cardPos++;
            } while (cardIndex < mCardsDealt);
        }
        if (cardsReturned < 2) {
            return;
        }
    }
    Mg_ResumeSharedState();
    mState = STATE_WAIT_COINS;
}

/* States 12 and 13: at rest. */
void dScMgBSC_c::StateDone() {}

/* D1 and D0 are not written out: the vtable InitResources emits pulls them
 * in, in ROM order. Written out of line, D0 would come out first. */

/* Static-init globals (was the handwritten __sinit_ov006_0213326c shard).
 * The state table plain-copied from the ROM PMF constants, in retail
 * copy order. */
StateHandler sBSCStateTable[14] = {
    pmf_dScMgBSC_c_StateDealCards,
    pmf_dScMgBSC_c_StateWaitDeal,
    pmf_dScMgBSC_c_StateChooseCard,
    pmf_dScMgBSC_c_StateMoveCards,
    pmf_dScMgBSC_c_StateWaitMove,
    pmf_dScMgBSC_c_StateRevealCards,
    pmf_dScMgBSC_c_StateJudgeCards,
    pmf_dScMgBSC_c_StateTableReaction,
    pmf_dScMgBSC_c_StateTableReaction_2,
    pmf_dScMgBSC_c_StateWaitTable_2,
    pmf_dScMgBSC_c_StateWaitTable,
    pmf_dScMgBSC_c_StateReturnCards,
    pmf_dScMgBSC_c_StateDone,
    pmf_dScMgBSC_c_StateDone_2,
};
