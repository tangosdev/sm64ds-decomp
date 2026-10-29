//cpp
/* ov006/dScMgMCarlo_c: the Monte Carlo card minigame. Pairs of matching
 * faces on neighbouring board slots are picked with the stylus and removed,
 * and the board deals itself back in behind them.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder.
 *
 * leftovers:
 *   - the func_ov004_ helpers and the data_ov006_ board globals are
 *     unnamed in symbols.txt. The card methods and the static board
 *     helpers have coined names; the ROM has only addresses for them.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_m_carlo_AssetId { ASSET_D_2D_MG_CARD_NCL_BIN = 0x00bb };

#include "dScMgMCarlo_c.h"
#include "types.h"
#include "decl_common.h"
#include "dScMgCard_c.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "math/Approach.h"

/* Coined from the card transitions in DealIn and Update. The byte-sized
 * mState field retains its existing layout. */
enum CardState {
    CARD_WAITING = 0,
    CARD_ENTERING = 1,
    CARD_IDLE = 2,
    CARD_SELECTED = 3,
    CARD_MOVING = 4,
    CARD_LEAVING = 5
};

/* Engine helpers; the parameter types are the ones the mangled names
 * encode. */
namespace GXS {
void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

extern "C" {
extern dMgMCarloCardObj_c *sMCarloFirstPick;  /* first pick */
extern dMgMCarloCardObj_c *sMCarloCards;      /* board list head */
extern dMgMCarloCardObj_c *sMCarloCardsTail;  /* board list tail */
extern dMgMCarloCardObj_c *sMCarloSecondPick; /* second pick */
extern short sMCarloBoardDelay;
extern int sMCarloCardTotal;
extern int sMCarloDealProgress;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int sMCarloCardsMoving;
extern int sMCarloCols;
extern int sMCarloRows;
extern int sMCarloPayout;
extern int sMCarloDeckSize;
extern int sMCarloFaceCounts[8]; /* the eight face weights */
extern int data_020a0db0;
extern unsigned short sMCarloCardFrameSprites[];
extern int sMgCardSprites[];
extern int sMCarloCardSprites[];
extern void Vec2_Sub(int *o, int *a, int *b);
extern void func_0203d630(int *p, int m);
extern int sMgCoinsBetPlaced;
extern int func_0203d5dc(void *a, void *b);
extern int sMCarloHudSprite;
extern void Mg_DrawNumber(int a, int b, int c, int d, int e, int f, int g);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void Camera_InitDealerView(void *p);
void Mg_DrawCredits(char *a0, int a1, int a2, int a3);
void MgCoins_Draw(void);
void Mg_DrawScoreTally(char *a0, int a1, int a2, int a3);
void Dealer_Draw(void *p);
int Dealer_IsIdle(void *p);
void MgCoins_Update(void);
void Dealer_Update(void *c);
void Dealer_StartShuffle(char *c, int unused, short a2, int a3);
void MgCoins_Reset(char *p);
void FreeGfxSlotsById(int arg);
void MgCoins_StartPayout(char *p);
extern unsigned short gMgCoinsStatus;
extern void LoadCard2SubBg3(void *c);
extern int GetGameLanguage(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern int Dealer_Load(char *p);
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
}

// @symbol _ZN18dMgMCarloCardObj_cC1Ev
dMgMCarloCardObj_c::dMgMCarloCardObj_c() : mPrev(0), mNext(0) {}

// @symbol dScMgMCarlo_c_classInit
/* `new` inlines the implicit constructor: the base chain, this class's
   vptr, then mDealer and the card array in declaration order. */
extern "C" void *dScMgMCarlo_c_classInit() {
    return new dScMgMCarlo_c;
}

// @symbol _ZN13dScMgMCarlo_c13InitResourcesEv
/* Slot 0. Loads the language's board graphics and the shared palette
 * (0xbb), sets the engine mode bytes, parks the 3D matrix at the family
 * camera and arms mDealer. The round starts with the virtual
 * StartRound(-1), which deals the first board. */
s32 dScMgMCarlo_c::InitResources() {
    void *tileData;
    void *paletteData;

    Mg_SetTopScreenYOffset(0x20);
    LoadCard2SubBg3(this);
    tileData = (void *)LoadFile(sMCarloCardNcgByLanguage[GetGameLanguage()]);
    paletteData = (void *)LoadFile(ASSET_D_2D_MG_CARD_NCL_BIN);
    DecompressLZ16(tileData, (void *)0x6600000);
    GXS::LoadOBJPltt(paletteData, 0, 0x100);
    Deallocate(tileData);
    Deallocate(paletteData);

    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(unsigned short *)0x4000008 = (*(unsigned short *)0x4000008 & ~3) | 1;
    *(unsigned short *)0x400100a = *(unsigned short *)0x400100a & ~3;
    Camera_InitDealerView(pad_4660);
    if (Dealer_Load((char *)&mDealer) == 0)
        return 0;

    Mg_PlaceBannersOnTopScreen();
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    {
        int score = Mg_GetSavedRecord1();
        if (gMgScene != 0)
            gMgScene->mHudScore = score;
    }
    StartRound(-1);
    this->mRoundTimer = 0;
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c10StartRoundEi
/* Slot 18: restart the round from a fresh board. SetupBoard deals a new
 * layout, the round state re-arms (mRoundState = 1), and the shared prompt
 * window re-opens watching mRoundProgress.
 *
 * MgCoins_Reset never reads its argument, but the ROM still computes
 * this + 0x6000 for it, which is mArray[76].mTargetY. */
void dScMgMCarlo_c::StartRound(int arg) {
    SetupBoard(mArray);
    sMCarloPickAllowed = 0;
    mRoundProgress = 0;
    mDealer.mReserved = 1;
    Dealer_StartShuffle((char *)&mDealer, 4, 4, (int)&mRoundProgress);
    mDealer.mHand = 1;
    mRoundTimer = 0;
    mRoundState = STATE_START_PROMPT;
    MgCoins_Reset((char *)&mArray[76].mTargetY);
    gMgCoinsShowDelta = 1;
}

// @symbol _ZN13dScMgMCarlo_c11CanEndStateEi
/* Slot 19: the egg-exit sequence, run on the same round state (mRoundState)
 * as Behavior. 4 frees the Gfx slots. 5 waits for the shared window, then
 * hands the dealt total on (or plays the wrap-up cue). 6 hides the board
 * one card at a time from the tail. 7 returns 1 while the active scene has
 * credits; otherwise it flips every dealt card away. 8 runs the 0x1e timer
 * out and returns 1 when it reaches zero. */
int dScMgMCarlo_c::CanEndState(int mode) {
    switch (mRoundState) {
    case STATE_FREE_GFX:
        FreeGfxSlotsById(0x1d);
        mRoundState += 1;
        break;
    case STATE_WAIT_DEALER:
        if (Dealer_IsIdle(&mDealer) != 0) {
            int dealt = sMCarloCardTotal;
            if (dealt != 0) {
                MgCoins_StartSpend(dealt);
                mRoundTimer = -1;
            } else {
                MgCoins_StartPayout((char *)sMCarloPayout);
            }
            mRoundProgress = 0;
            mRoundState += 1;
        }
        break;
    case STATE_HIDE_CARDS: {
        dMgMCarloCardObj_c *head = sMCarloCards;
        if (head != 0 && head->mVisible != 0) {
            if (ApproachLinear2(mRoundTimer, 0, 1) != 0) {
                dMgMCarloCardObj_c *card;
                int cardIndex;
                int hiddenCards;
                mRoundTimer = 0x10;
                hiddenCards = mRoundProgress;
                card = sMCarloCardsTail;
                for (cardIndex = 0; cardIndex < hiddenCards; cardIndex++) {
                    if (card == 0)
                        break;
                    card = card->mPrev;
                }
                if (card != 0)
                    card->mVisible = 0;
                mRoundProgress += 1;
            }
        } else {
            if (gMgCoinsStatus <= 1)
                mRoundState += 1;
        }
        break;
    }
    case STATE_FLIP_CARDS: {
        dScMgBase_c *active = (dScMgBase_c *)gMgScene;
        if ((active != 0 ? active->mCredits : 0) != 0)
            return 1;
        dScMgMCarlo_c::FlipDealtCards();
        mRoundTimer = 0x1e;
        mRoundState += 1;
        break;
    }
    case STATE_EXIT_DELAY:
        if (ApproachLinear2(mRoundTimer, 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}

// @symbol _ZN13dScMgMCarlo_c8BehaviorEv
/* Slot 6: the round, on the s16 state at mRoundState. Case 1 arms the prompt
 * blink and falls through into case 2 on purpose. gMgScene is
 * the active scene (dScMgBase_c.h declares it void *), so it is cast at
 * each use. */
s32 dScMgMCarlo_c::Behavior() {
    switch (mRoundState) {
    case STATE_START_PROMPT:
        mRoundState++;
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        /* fall through */
    case STATE_INITIAL_DEAL:
        sMCarloDealProgress = mRoundProgress * 5 << 12;
        if (mRoundProgress == 4) {
            if (dScMgMCarlo_c::BoardBusy() == 0) {
                mRoundProgress = 0;
                mRoundState++;
            }
        }
        break;
    case STATE_PLAY:
        if (dScMgMCarlo_c::BoardReady() == 0) {
            int visibleCards;
            if (sMCarloPickAllowed == 1)
                sMCarloPickAllowed = 0;
            visibleCards = sMCarloDealProgress >> 12;
            if (sMCarloDeckSize != 0 && visibleCards > 10 && visibleCards <= 18 &&
                Dealer_IsIdle(&mDealer) != 0) {
                mRoundProgress = 0;
                mDealer.mReserved = 0;
                Dealer_StartReveal(&mDealer, 2, &mRoundProgress);
                if (sMCarloDeckSize == 2)
                    mDealer.mHand = 0;
            } else {
                int revealLimit = mRoundProgress + 18;
                if (visibleCards >= revealLimit) {
                    int hasSelectedPair = (sMCarloFirstPick != 0 && sMCarloSecondPick != 0);
                    if (hasSelectedPair == 0)
                        sMCarloDealProgress = revealLimit << 12;
                }
            }
        } else {
            if (sMCarloPickAllowed == 0) {
                if (dScMgMCarlo_c::HasRemovablePair() != 0) {
                    sMCarloPickAllowed = 1;
                } else {
                    if (sMCarloCardTotal != 0) {
                        if (mDealer.mHand == 1) {
                            mDealer.mHand = 0;
                        } else if (Dealer_IsIdleAtRest(&mDealer) != 0) {
                            {
                                dScMgBase_c *active = (dScMgBase_c *)gMgScene;
                                if (active != 0) {
                                    if (active->mHudScore > 0)
                                        active->mHudScore = active->mHudScore - 1;
                                }
                            }
                            Dealer_PlayLose(&mDealer);
                            Mg_SetSharedState(MG_STATE_LOSE);
                            {
                                int dealt = sMCarloCardTotal;
                                dScMgBase_c *active = (dScMgBase_c *)gMgScene;
                                int creditsLeft = ((active != 0) ? active->mCredits : 0) - dealt;
                                int score = (active != 0) ? active->mHudScore : 0;
                                Mg_SaveRecordPair(creditsLeft, score);
                            }
                            mPromptEnabled = 0;
                            mRoundState++;
                        }
                    } else {
                        {
                            dScMgBase_c *active = (dScMgBase_c *)gMgScene;
                            if (active != 0) {
                                if (active->mHudScore < 0x270f)
                                    active->mHudScore = active->mHudScore + 1;
                                if (active->mHudScore > active->mBestHudScore)
                                    active->mBestHudScore = active->mHudScore;
                            }
                        }
                        Mg_SubmitScore(mHudScore);
                        Dealer_PlayWin(&mDealer);
                        Mg_SetSharedState(MG_STATE_WIN);
                        {
                            dScMgBase_c *active = (dScMgBase_c *)gMgScene;
                            int credits = (active != 0) ? active->mCredits : 0;
                            Mg_SaveRecordPair(sMCarloPayout + credits,
                                              (active != 0) ? active->mHudScore : 0);
                        }
                        mPromptEnabled = 0;
                        mRoundState++;
                    }
                    sMCarloPickAllowed = 0;
                }
            }
        }
        break;
    }

    MgCoins_Update();
    Dealer_Update(&mDealer);
    dScMgMCarlo_c::UpdateBoard();
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c6RenderEv
/* Slot 9. The board draws in two passes over the same tail-to-head walk:
 * cards with a positive vertical movement step first, then those with a
 * zero step. mYStep is an absolute magnitude, not a direction. */
s32 dScMgMCarlo_c::Render() {
    Camera_InitDealerView(pad_4660);
    Mg_DrawCredits((char *)this, 0xc, 0xc, 0);
    MgCoins_Draw();
    Mg_DrawScoreTally((char *)this, 0xe0, 0x14, 1);

    {
        dMgMCarloCardObj_c *card = sMCarloCardsTail;
        int cardIndex = 0;
        for (;;) {
            if (card == 0)
                break;
            if (card->mYStep > 0)
                card->Render();
            cardIndex++;
            card = card->mPrev;
            if (cardIndex >= 0x14)
                break;
        }
    }
    {
        int cardIndex = 0;
        dMgMCarloCardObj_c *card = sMCarloCardsTail;
        for (;;) {
            if (card == 0)
                break;
            if (card->mYStep == 0)
                card->Render();
            cardIndex++;
            card = card->mPrev;
            if (cardIndex >= 0x14)
                break;
        }
    }
    RenderHud();
    Dealer_Draw(&mDealer);
    return 1;
}

// @symbol _ZN13dScMgMCarlo_c9RenderHudEv
/* The ROM passes `this` and the body never reads it. Draw the deck
 * gauge while cards remain in the deck: the count -- sMCarloDeckSize,
 * seeded with the deck size by SetupBoard and drained one card at a time
 * as the deal proceeds -- drawn as a number at (0xe8, 0x28), with the
 * gauge's frame sprite behind it at (0xe8, 0x18). */
void dScMgMCarlo_c::RenderHud(void) {
    int deck = sMCarloDeckSize;
    if (deck == 0)
        return;
    Mg_DrawNumber(0xe8, 0x28, deck, -1, -1, 0, 0);
    Hud_RenderSprite((void *)sMCarloHudSprite, 0xe8, 0x18, -1, -1);
}

// @symbol _ZN18dMgMCarloCardObj_c4InitEi
/* Reset an unlinked card. The first twenty slots stagger the deal by
 * column; reserve cards wait one frame. Slot 19 marks the last visible card. */
void dMgMCarloCardObj_c::Init(int slot) {
    mSlot = (short)slot;
    if (slot >= 0x14) {
        mDealDelay = 1;
    } else {
        mDealDelay = (short)((slot % 5) * 2 + 1);
    }
    mLift = 0;
    if (slot == 0x13)
        sMCarloCardsTail = this;
    mVisible = 1;
    mXStep = 0;
    mYStep = 0;
    mState = CARD_WAITING;
    mNext = 0;
    mPrev = mNext;
}

// @symbol _ZN18dMgMCarloCardObj_c6DealInEi
/* Move toward a board slot. Cards coming from reserve start above the
 * board and become the last visible card; waiting cards use the same start.
 * Other cards move from their current position. The target is the slot's
 * column and row, and the absolute scaled delta supplies the movement step. */
void dMgMCarloCardObj_c::DealIn(int slot) {
    int delta[3];

    if (slot >= 0x14) {
        return;
    }

    if (mSlot >= 0x14) {
        mX = (0x70 - ((sMCarloDeckSize >> 2) << 1)) << 12;
        mY = -0x30000;
        sMCarloCardsTail = this;
        mState = CARD_ENTERING;
    } else if (mState == CARD_WAITING) {
        mX = (0x70 - ((sMCarloDeckSize >> 2) << 1)) << 12;
        mY = -0x30000;
        mState = CARD_MOVING;
        sMCarloCardsMoving++;
    } else {
        mState = CARD_MOVING;
        sMCarloCardsMoving++;
    }

    mTargetX = ((slot % 5) * 32 + 0x30) << 12;
    mTargetY = ((slot / 5) * 0x30) << 12;
    mSlot = (short)slot;

    Vec2_Sub(delta, &mTargetX, &mX);
    mXStep = delta[0];
    mYStep = delta[1];
    func_0203d630(&mXStep, 0x124);

    if (mXStep < 0) {
        mXStep = -mXStep;
    }
    if (mYStep < 0) {
        mYStep = -mYStep;
    }
}

// @symbol _ZN18dMgMCarloCardObj_c8FlipAwayEi
/* Leave in reverse column order after the round ends. */
void dMgMCarloCardObj_c::FlipAway(int slot) {
    mDealDelay = (short)((4 - slot % 5) * 2);
    mState = CARD_LEAVING;
}

// @symbol _ZN18dMgMCarloCardObj_c10IsPairWithEPS_
/* Can this card and that card be removed
 * together? Same face value, and the two board slots within one step in
 * BOTH axes (slot is col = slot%5, row = slot/5) -- the board
 * adjacency test. Called on the second pick against the first in Update,
 * and across the whole board by HasRemovablePair. */
int dMgMCarloCardObj_c::IsPairWith(dMgMCarloCardObj_c *other) {
    int slot, otherSlot, columnDistance, rowDistance;
    if (other->mFace != mFace)
        goto fail;
    otherSlot = other->mSlot;
    slot = mSlot;
    columnDistance = slot % 5 - otherSlot % 5;
    rowDistance = slot / 5 - otherSlot / 5;
    if (columnDistance < 0)
        columnDistance = -columnDistance;
    if (columnDistance >= 2)
        goto fail;
    if (rowDistance < 0)
        rowDistance = -rowDistance;
    if (rowDistance >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}

// @symbol _ZN18dMgMCarloCardObj_c7HitTestEv
/* Is this card the one under the stylus?
 * Only when the round is armed (sMCarloPickAllowed) and the board is
 * settled (no card still flying or flipping in), the touch panel has to
 * report a new press, and this card's on-screen position (fixed-point
 * >> 12) has to fall in the 0x20-by-0x30 window around the reported touch
 * point -- the same 32x48 slot grid DealIn lays the board out on. */
int dMgMCarloCardObj_c::HitTest(void) {
    u8 touchSlot;
    int touchOffset;
    int touching;
    int touchXOffset, touchYOffset;

    if (sMCarloPickAllowed == 0)
        return 0;
    if (dScMgMCarlo_c::BoardBusy() != 0)
        goto fail;

    touchSlot = gActivePlayerSlot;
    touchOffset = touchSlot * 4;
    touching = 0;
    if (gTouchHeld[touchOffset]) {
        if (gTouchEdge[touchOffset])
            touching = 1;
    }
    if (touching == 0)
        goto fail;

    touchXOffset = gTouchX[touchSlot * 4] - (mX >> 12);
    touchYOffset = gTouchY[touchSlot * 4] - (mY >> 12);
    if (touchXOffset > 7 && touchXOffset < 0x28 && touchYOffset > 0 && touchYOffset < 0x31)
        return 1;
fail:
    return 0;
}

// @symbol _ZN18dMgMCarloCardObj_c6UpdateEi
/* Advance the card toward its current board slot. Touching an idle card
 * selects it; touching a selected card deselects it. A matching second pick
 * starts the pair's removal delay. Incoming cards settle when their position
 * and lift reach the targets. Leaving cards wait out their stagger, then
 * slide left off the board. */
void dMgMCarloCardObj_c::Update(int slot) {
    switch (mState) {
    case CARD_WAITING:
        mDealDelay -= 1;
        if (mDealDelay != 0)
            return;
        sMCarloDeckSize--;
        DealIn(slot);
        return;

    case CARD_IDLE:
        if (mSlot != slot) {
            DealIn(slot);
            return;
        }
        if (HitTest() == 0)
            return;
        if (sMCarloFirstPick == 0) {
            sMCarloFirstPick = this;
            mState = CARD_SELECTED;
            Sound::PlayBank2_2D(0x153);
            return;
        }
        if (IsPairWith(sMCarloFirstPick) != 0) {
            sMCarloSecondPick = this;
            sMCarloBoardDelay = 0x20;
            mState = CARD_SELECTED;
            Sound::PlayBank2_2D(0x154);
            if (sMgCoinsBetPlaced == 0)
                sMgCoinsBetPlaced = 1;
            return;
        }
        func_02012790(0xe);
        {
            dMgMCarloCardObj_c *first = sMCarloFirstPick;
            first->mState = CARD_IDLE;
        }
        sMCarloFirstPick = 0;
        return;

    case CARD_SELECTED:
        if (HitTest() == 0)
            return;
        mState = CARD_IDLE;
        if (sMCarloFirstPick != this)
            return;
        if (sMCarloSecondPick != 0)
            return;
        Sound::PlayBank2_2D(0x155);
        sMCarloFirstPick = 0;
        return;

    case CARD_ENTERING:
    case CARD_MOVING:
        ApproachLinear(mX, mTargetX, mXStep);
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mLift, 0x4000, 0x300);
        if (func_0203d5dc(&mX, &mTargetX) != 0)
            return;
        if (mLift != 0x4000)
            return;
        mState = CARD_IDLE;
        ApproachLinear(sMCarloCardsMoving, 0, 1);
        mXStep = 0;
        mYStep = 0;
        return;

    case CARD_LEAVING:
        if (ApproachLinear2(mDealDelay, 0, 1) == 0)
            return;
        ApproachLinear(mX, -0x30000, 0x10000);
        return;

    default:
        break;
    }
}

// @symbol _ZN18dMgMCarloCardObj_c6RenderEv
/* Draw this card through the dMeter_c sprite bank.
 * Hidden cards (mVisible 0) and ones still waiting to be dealt (mState 0)
 * draw nothing, and a selected card blinks according to bit 3 of
 * data_020a0db0. The bank index is the face's bank (mFace + 1) of five flip
 * frames -- (mLift >> 12), 0x4000 fully raised = frame 4 -- and the card
 * sits at its fixed-point position shifted to pixels plus a 24-pixel screen
 * offset. Slot 0 of the element vtable; Render is the class's key function,
 * so this TU is what emits _ZTV/_ZTI/_ZTS for the element class. */
void dMgMCarloCardObj_c::Render(void) {
    unsigned char state;
    if (mVisible == 0)
        return;
    state = mState;
    if (state == CARD_WAITING)
        return;
    if (state == CARD_SELECTED) {
        if ((data_020a0db0 & 8) != 0)
            return;
    }
    {
        int frameIndex = (mFace + 1) * 5 + (mLift >> 12);
        unsigned short sprite = sMCarloCardFrameSprites[frameIndex];
        Hud_RenderSprite((void *)sMCarloCardSprites[sprite], (mX >> 12) + 24, (mY >> 12) + 24, -1,
                         -1);
    }
}

// @symbol _ZN13dScMgMCarlo_c10SetupBoardEP18dMgMCarloCardObj_c
/* Lay out a fresh round. The active
 * base's star count picks the tier -- under 5 stars, 6 face values at 6
 * cards each (36); under 10, 7 at 8 (56); past that, all 8 at 0xa (80) --
 * and both the deck counter and the dealt total start at faces-times-weight.
 * Then, re-dealing until the opening board is playable: the eight face
 * weights are seeded (the first faces get the weight, the rest zero), every
 * one of the 0x50 card elements is reset (Init), the first dealt-total of
 * them takes a fresh weighted face (DrawCardValue), the round state clears,
 * and the dealt cards are threaded onto the board's doubly-linked list in
 * array order. The do-while repeats the whole deal until the twenty board
 * slots hold at least one removable pair (HasRemovablePair never looks past
 * slot 0x13, so the stock behind the board does not count). */
void dScMgMCarlo_c::SetupBoard(dMgMCarloCardObj_c *cards) {
    int stars;
    dScMgBase_c *active;
    short face;
    short cardIndex;
    short slot;
    dMgMCarloCardObj_c *card;
    dMgMCarloCardObj_c *previous;
    dMgMCarloCardObj_c *linkTail;
    dMgMCarloCardObj_c *savedNext;

    active = (dScMgBase_c *)gMgScene;
    stars = (active != 0) ? active->mHudScore : 0;

    if (stars < 5) {
        sMCarloCols = 6;
        sMCarloRows = 6;
        sMCarloPayout = 5;
    } else if (stars < 0xa) {
        sMCarloCols = 8;
        sMCarloRows = 7;
        sMCarloPayout = 8;
    } else {
        sMCarloCols = 0xa;
        sMCarloRows = 8;
        sMCarloPayout = 0xa;
    }

    sMCarloDeckSize = sMCarloRows * sMCarloCols;
    sMCarloCardTotal = sMCarloRows * sMCarloCols;

    do {
        for (face = 0; face < 8; face++) {
            if (face < sMCarloRows)
                sMCarloFaceCounts[face] = sMCarloCols;
            else
                sMCarloFaceCounts[face] = 0;
        }

        cardIndex = 0;
        card = cards;
        for (; cardIndex < 0x50; cardIndex++) {
            card->Init(cardIndex);
            if (cardIndex < sMCarloCardTotal) {
                card->mFace = (unsigned char)dScMgMCarlo_c::DrawCardValue();
            }
            card++;
        }

        slot = 0;
        sMCarloFirstPick = 0;
        sMCarloSecondPick = 0;
        sMCarloBoardDelay = 0;
        sMCarloCardsMoving = 0;
        sMCarloDealProgress = 0;
        sMCarloCards = cards;

        if (sMCarloCardTotal - 1 > 0) {
            previous = cards;
            do {
                linkTail = &cards[slot + 1];
                savedNext = previous->mNext;
                previous->mNext = linkTail;
                linkTail->mPrev = previous;
                while (linkTail->mNext != 0) {
                    linkTail = linkTail->mNext;
                }
                linkTail->mNext = savedNext;
                previous++;
                slot++;
            } while (slot < sMCarloCardTotal - 1);
        }
    } while (dScMgMCarlo_c::HasRemovablePair() == 0);
}

// @symbol _ZN13dScMgMCarlo_c16HasRemovablePairEv
/* Does the board still hold two
 * cards that can be removed together (IsPairWith true for some pair)? The
 * walk skips the spare cards past slot 0x13 (they are never on the board),
 * and the inner walk starts one past the outer card so each pair is tested
 * once. SetupBoard re-deals while this is false so a fresh board always has
 * at least one removable pair; Behavior uses it to catch a stuck board. */
int dScMgMCarlo_c::HasRemovablePair(void) {
    dMgMCarloCardObj_c *first, *second;
    first = sMCarloCards;
    while (first != 0 && first->mSlot < 0x14) {
        second = first->mNext;
        while (second != 0 && second->mSlot < 0x14) {
            if (first->IsPairWith(second) != 0)
                return 1;
            second = second->mNext;
        }
        first = first->mNext;
    }
    return 0;
}

// @symbol _ZN13dScMgMCarlo_c9BoardBusyEv
/* Busy while the deal count is changing, a card is moving, or both
 * picks are waiting for removal. A single selected card does not block input. */
int dScMgMCarlo_c::BoardBusy(void) {
    int busy = 1;
    short target;
    short shown;
    shown = (short)(sMCarloDealProgress >> 12);
    target = (short)sMCarloCardTotal;
    if (target > 20)
        target = 20;
    if (shown == target) {
        if (sMCarloCardsMoving == 0) {
            if (sMCarloFirstPick == 0 || sMCarloSecondPick == 0)
                busy = 0;
        }
    }
    return busy;
}

// @symbol _ZN13dScMgMCarlo_c10BoardReadyEv
/* Ready once the visible count reaches the board size, movement has
 * stopped, and no selected pair is waiting for removal. */
int dScMgMCarlo_c::BoardReady(void) {
    int ready = 0;
    short target;
    short shown;
    shown = (short)(sMCarloDealProgress >> 12);
    target = (short)sMCarloCardTotal;
    if (target > 20)
        target = 20;
    if (shown == target) {
        if (sMCarloCardsMoving == 0) {
            if (sMCarloFirstPick == 0 || sMCarloSecondPick == 0)
                ready = 1;
        }
    }
    return ready;
}

// @symbol _ZN13dScMgMCarlo_c13DrawCardValueEv
/* One weighted draw from the
 * eight-face deck. Roll the seeded RNG scaled by the weight total, then walk
 * the weights subtracting until the running total goes negative -- that face
 * is the draw, and its weight is spent. The same routine dScMgCard_c runs on
 * its six-face deck (src/minigames/d_s_mg_card.cpp); the weights themselves
 * live in this overlay's data and SetupBoard refills them per board size. */
int dScMgMCarlo_c::DrawCardValue(void) {
    unsigned char drawnFace = 0;
    int total = 0;
    int face;
    int roll;
    for (face = 0; face < 8; face++)
        total += sMCarloFaceCounts[face];
    roll = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    total = (total * roll) >> 0xc;
    for (face = 0; face < 8; face++) {
        total -= sMCarloFaceCounts[face];
        if (total < 0) {
            drawnFace = (unsigned char)face;
            sMCarloFaceCounts[face]--;
            break;
        }
    }
    return drawnFace;
}

// @symbol _ZN13dScMgMCarlo_c14FlipDealtCardsEv
/* Between rounds, queue every card
 * currently on the board for the flip-out, walking the board list head to
 * tail and numbering the walk so FlipAway's flip stagger mirrors the deal
 * stagger card by card. */
void dScMgMCarlo_c::FlipDealtCards(void) {
    dMgMCarloCardObj_c *node = sMCarloCards;
    short slot = 0;
    if ((sMCarloDealProgress >> 12) <= 0)
        return;
    do {
        if (node == 0)
            return;
        node->FlipAway(slot);
        slot = slot + 1;
        node = node->mNext;
    } while (slot < (sMCarloDealProgress >> 12));
}

// @symbol _ZN13dScMgMCarlo_c11UpdateBoardEv
/* Resolve a selected pair after its delay, unlink both cards, and restart
 * the deal count at the earlier slot so the remaining cards close the gap.
 * While a pair is selected, update only cards entering from reserve. Otherwise
 * advance the visible-card count and update every card in that prefix. */
void dScMgMCarlo_c::UpdateBoard(void) {
    dMgMCarloCardObj_c *head = sMCarloCards;
    dMgMCarloCardObj_c *card;
    short slot;

    if (sMCarloFirstPick != 0) {
        if (sMCarloSecondPick != 0) {
            if (--sMCarloBoardDelay == 0) {
                if (sMCarloFirstPick->mSlot > sMCarloSecondPick->mSlot)
                    sMCarloDealProgress = sMCarloSecondPick->mSlot << 12;
                else
                    sMCarloDealProgress = sMCarloFirstPick->mSlot << 12;

                {
                    dMgMCarloCardObj_c *first = sMCarloFirstPick;
                    if (sMCarloCards == first)
                        sMCarloCards = first->mNext;
                    if (sMCarloCardsTail == first)
                        sMCarloCardsTail = first->mPrev;
                    if (first->mPrev != 0)
                        first->mPrev->mNext = first->mNext;
                    if (first->mNext != 0)
                        first->mNext->mPrev = first->mPrev;
                    first->mNext = 0;
                    first->mPrev = first->mNext;
                }
                {
                    dMgMCarloCardObj_c *second = sMCarloSecondPick;
                    if (sMCarloCards == second)
                        sMCarloCards = second->mNext;
                    if (sMCarloCardsTail == second)
                        sMCarloCardsTail = second->mPrev;
                    if (second->mPrev != 0)
                        second->mPrev->mNext = second->mNext;
                    if (second->mNext != 0)
                        second->mNext->mPrev = second->mPrev;
                    second->mNext = 0;
                    second->mPrev = second->mNext;
                }

                sMCarloSecondPick = 0;
                sMCarloFirstPick = 0;
                ApproachLinear(sMCarloCardTotal, 0, 2);
            }
            card = head;
            for (slot = 0; slot < (sMCarloDealProgress >> 12); slot++) {
                if (card == 0)
                    return;
                if (card->mState == CARD_ENTERING)
                    card->Update(slot);
                card = card->mNext;
            }
            return;
        }
    }

    {
        int target = (short)sMCarloCardTotal;
        if (target > 0x14)
            target = 0x14;
        ApproachLinear(sMCarloDealProgress, target << 12, 0x800);
    }
    card = head;
    for (slot = 0; slot < (sMCarloDealProgress >> 12); slot++) {
        if (card == 0)
            return;
        card->Update(slot);
        card = card->mNext;
    }
}

// @symbol _ZN18dMgMCarloCardObj_cD1Ev
/* Empty: the card has no base and nothing to destroy. */
dMgMCarloCardObj_c::~dMgMCarloCardObj_c() {}

/* D1 and D0 are not written here. The destructor is inline in the header,
 * which makes InitResources the key function; the vtable it emits names
 * D1 and D0, so the compiler emits both, in ROM order. Writing them out
 * here puts D0 ahead of D1 and adds a D2 the ROM does not have. */
/* The sprite-index remap table. Its initializer reads the retail table, so
 * it is not a constant expression: the compiler emits
 * __sinit_d_s_mg_m_carlo.cpp to copy the 19 words at overlay load. */
int sMCarloCardSprites[19] = {
    sMgCardSprites[0],  sMgCardSprites[1],  sMgCardSprites[2],  sMgCardSprites[11],
    sMgCardSprites[12], sMgCardSprites[13], sMgCardSprites[14], sMgCardSprites[3],
    sMgCardSprites[4],  sMgCardSprites[5],  sMgCardSprites[6],  sMgCardSprites[9],
    sMgCardSprites[10], sMgCardSprites[7],  sMgCardSprites[8],  sMgCardSprites[15],
    sMgCardSprites[16], sMgCardSprites[17], sMgCardSprites[18],
};
