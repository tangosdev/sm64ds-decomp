//cpp
/* ov006/dScMgBSC_c -- the Lucky Stars card minigame, 20 functions.
 *
 * `#pragma opt_strength_reduction off` is file-wide and needed:
 * StateDealCards and StateRevealCards depend on it, a bracket around them
 * does not bind (last setting wins), and the other functions do not care.
 * Functions run in REVERSE of ROM order; do not reorder.
 *
 * leftovers:
 *   - the func_ov004 and func_ov006 helpers and most data_ globals are
 *     unnamed in symbols.txt; mCredits and mBestHudScore are unnamed in the
 *     header.
 *   - the factory builds the object by hand because the class has no
 *     constructor declared yet.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_bsc_AssetId {
    ASSET_D_2D_MG_BIG_SMALL_NCL_BIN = 0x00b3
};

#pragma opt_strength_reduction off

#include "dScMgBSC_c.h"
#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"
#include "PlayerInput.h"

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
extern void Mg_ResumeSharedState(void);
extern int Dealer_IsIdle(int* r0);
extern void MgCoins_StartPayout(int x);
extern void MgCoins_StartLose(void);
extern int gMgCoinsInPlay;
extern void Dealer_PlayWin(void* c);
/* decl_common.h already declares this with a void pointer, so the
 * definition's char pointer cannot be repeated here. */
extern void Dealer_PlayLose(void* c);
extern void Mg_SubmitScore(int self);
extern void Mg_SetSharedState(int v);
extern void Mg_SaveRecordPair(int a, int b);
extern int sBscLaneX[];
extern int sMgCoinsBetPlaced;
extern int RandomIntInternal(int* seed);
extern void Dealer_StartShuffle(char *c, int unused, short a2, int a3);
extern int data_0209e650;
extern "C" void FreeGfxSlotsById(int arg);
extern "C" void MgCoins_Reset(void);
void Camera_InitDealerView(void *camera);
void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
void Mg_DrawCredits(void *scene, int a1, int a2, int a3);
void Mg_DrawScoreTally(void *scene, int a1, int a2, int a3);
void Dealer_Draw(void *table);
void MgCoins_Update(void);
void Dealer_Update(char *c);
extern unsigned char data_0209d45c[];
extern int data_0208ee44[];
extern void LoadCard2SubBg3(void);
extern s32 GetGameLanguage(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int LoadFile(int handle);
extern int Dealer_Load(char *c);
void *_ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
void __cxa_vec_ctor(void*, int, int, void*, void*);
void func_0203d738(void);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV10dScMgBSC_c[];
}

/* Real C++ declarations for the engine calls below; the namespaces and the
 * reference parameter are what give the ROM's mangled names. */
namespace Sound {
void PlayBank2_2D(u32 id);
}

namespace GXS {
void LoadOBJPltt(const void *plt, u32 base, u32 size);
}

/* Moves value toward target by step; nonzero once it gets there. */
int ApproachLinear(int& value, int target, int step);

// @symbol dScMgBSC_c_classInit
extern "C" void* dScMgBSC_c_classInit()
{
    char* scene = (char*)_ZN7fBase_cnwEj(sizeof(dScMgBSC_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int*)scene = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(int*)scene = (int)&_ZTV10dScMgBSC_c[2];
        Dealer_Construct(scene + 0x4f38);
        __cxa_vec_ctor(scene + 0x51a8, 2, 8,
                     (void*)func_0203d738,
                     (void*)NullDestructor_0203d47c);
    }
    return scene;
}

// @symbol _ZN10dScMgBSC_c13InitResourcesEv
/* Slot 0, and the key function: the destructor is inline in the header, so
 * this is the first virtual defined out of line, and defining it here emits
 * the vtable and the D1 and D0 pair. The last call dispatches virtually
 * (mwcc does not devirtualize it). pad_4660 is the head of the camera
 * block, untyped in the header. */
s32 dScMgBSC_c::InitResources()
{
    int fh;
    data_0209d45c[0] = 0x11;
    LoadCard2SubBg3();
    LoadCompressedFileAt(sBscBigSmallNcgByLanguage[GetGameLanguage()], (void *)0x6600000);
    fh = LoadFile(ASSET_D_2D_MG_BIG_SMALL_NCL_BIN);
    GXS::LoadOBJPltt((void *)fh, 0, 0x100);
    Deallocate((void *)fh);
    Camera_InitDealerView(pad_4660);
    if (Dealer_Load((char *)&mShared) == 0)
        return 0;
    Mg_SetTopScreenYOffset(0x20);
    Mg_PlaceBannersOnTopScreen();
    data_0208ee44[0] = 1;
    mCredits = Mg_GetSavedRecord0();
    mCreditsShown = mCredits;
    this->StartRound(3);
    return 1;
}

// @symbol _ZN10dScMgBSC_c8BehaviorEv
/* Slot 6: runs the current state from the table. */
s32 dScMgBSC_c::Behavior()
{
    (this->*sBSCStateTable[mState])();
    MgCoins_Update();
    Dealer_Update((char *)&mShared);
    return 1;
}

// @symbol _ZN10dScMgBSC_c6RenderEv
/* Slot 9. Card positions are 20.12 fixed point. Sprite frames 0 to 2 are
 * the flip; from frame 3 the card's value picks the face. */
s32 dScMgBSC_c::Render()
{
    int i;

    Camera_InitDealerView(pad_4660);
    if (mState >= 1) {
        for (i = 0; i < 2; i++) {
            if (mCardFrame[i] < 3) {
                Hud_RenderSprite(sBscCardSprites[mCardFrame[i]], mCardPos[i].x >> 12,
                                 mCardPos[i].y >> 12, -1, -1);
            } else {
                Hud_RenderSprite(sBscCardSprites[mCardFrame[i] + mCardValue[i] * 2],
                                 mCardPos[i].x >> 12, mCardPos[i].y >> 12, -1, -1);
            }
        }
    }
    MgCoins_Draw();
    Mg_DrawCredits(this, 0xc, 0xc, 0);
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    Dealer_Draw(&mShared);
    return 1;
}

// @symbol _ZN10dScMgBSC_c10StartRoundEi
/* Slot 18. Mode 3 reads the meter score into mHudScore and posts it; every
   mode then restarts the state machine. The redundant `!= 4 && != 5`
   compares are in the ROM and must stay. */
void dScMgBSC_c::StartRound(int mode)
{
    if (mode != 4 && mode != 5 && mode == 3) {
        mHudScore = Mg_GetSavedRecord1();
        Mg_SubmitScore(mHudScore);
    }
    MgCoins_Reset();
    gMgCoinsShowDelta = 1;
    mState = 0;
}

// @symbol _ZN10dScMgBSC_c11CanEndStateEi
/* Slot 19. In state 13, once the table's reaction has finished, mode 4
   moves to state 9 and mode 5 to state 10. When the round has settled
   (state 12, with gMgCoinsStatus set) the final scores go to the
   meter. */
s32 dScMgBSC_c::CanEndState(int mode)
{
    int state;
    short gate;
    if (mState == 0xd) {
        if (Dealer_IsIdle((int *)&mShared) != 0) {
            if (mode == 4) {
                FreeGfxSlotsById(0x1d);
                mState = 9;
            } else if (mode == 5) {
                FreeGfxSlotsById(0x1d);
                mState = 0xa;
            }
        } else {
            return 0;
        }
    }
    state = mState;
    if (state >= 0xc) {
        gate = gMgCoinsStatus;
        if (gate == 1) goto settle;
    }
    return 0;
settle:
    if (state == 0xc && gate == 1) {
        Mg_SaveRecordPair(mCredits, mHudScore);
    }
    return 1;
}

// @symbol _ZN10dScMgBSC_c10OnDealDoneEv
/* Slot 21: picks one of four reactions from the score, mHudScore / 5
   capped at 3. */
void dScMgBSC_c::OnDealDone()
{
    int score = mHudScore;
    int tier = score / 5;
    if ((unsigned int)tier > 3) {
        tier = 3;
    }
    MgCoins_RaiseBet(tier + 1);
}

// @symbol _ZN10dScMgBSC_c14StateDealCardsEv
/* State 0: deals two cards with distinct random values out of five
 * (flags marks the values taken), starts the deal animation and the
 * prompt, and moves to StateWaitDeal. */
void dScMgBSC_c::StateDealCards()
{
    unsigned char flags[5];
    unsigned char* p;
    int j;
    int i;
    unsigned char idx;

    p = flags;
    for (j = 0; j < 5; j++)
        *p++ = 0;

    for (i = 0; i < 2; i++) {
        mCardPos[i].x = 0x80000;
        mCardPos[i].y = -0x40000;
        idx = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) % 5u;
        for (;;) {
            if (flags[idx] == 0) {
                mCardValue[i] = idx;
                flags[idx] = 1;
                break;
            }
            idx++;
            idx %= 5;
        }
        mCardFrame[i] = 0;
    }

    mOtherCard = 0;
    mChosenCard = 1;
    mDealCount = 0;
    Dealer_StartShuffle((char *)&mShared, 3, 2, (int)&mDealCount);

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    mState += 1;
}

// @symbol _ZN10dScMgBSC_c13StateWaitDealEv
/* State 1: slides each card to its lane (x toward
 * sBscLaneX, y toward 0x60000). mDealCount is re-read every pass
 * because the animation helper counts it down. Moves on once both cards
 * are in place. */
void dScMgBSC_c::StateWaitDeal()
{
  int i;
  int arrived;
  int xArrived;
  char *xp;
  char *yp;
  short count;

  arrived = 0;
  i = 0;
  count = mDealCount;
  if (count <= 0)
    goto after_loop;

  xp = (char *)&mCardPos[0].x;
  yp = (char *)&mCardPos[0].y;
  do {
      if (Math_Function_0203b14c((int *)xp, sBscLaneX[i], 0x800, 0x10000, 0x200) == 0)
          xArrived = 1;
      else
          xArrived = 0;

      if (Math_Function_0203b14c((int *)yp, 0x60000, 0x800, 0x10000, 0x200) == 0) {
          if (xArrived != 0)
              arrived++;
      }
    xp += 8;
    yp += 8;
    i++;
    count = mDealCount;
  } while (i < count);

after_loop:
  if (arrived < 2)
    return;
  mState++;
}

// @symbol _ZN10dScMgBSC_c15StateChooseCardEv
/* State 2: waits for a touch within 16 pixels of a lane's x and 24 of y
 * 0x60. The touched card becomes mChosenCard and the machine moves on. */
void dScMgBSC_c::StateChooseCard()
{
    unsigned int sample = gActivePlayerSlot;
    int touched = 0;
    int lane;
    if (gTouchHeld[sample * 4] != 0) {
        if (gTouchEdge[sample * 4] != 0) touched = 1;
    }
    if (touched == 0) return;
    for (lane = 0; lane < 2; lane++) {
        unsigned char idx = *(volatile unsigned char *)&gActivePlayerSlot;
        unsigned char *touch = &gTouchHeld[(unsigned int)idx * 4];
        int dx = (int)touch[2] - (sBscLaneX[lane] >> 12);
        int dy = (int)touch[3] - 0x60;
        if (dx < -0x10) continue;
        if (dx > 0x10) continue;
        if (dy < -0x18) continue;
        if (dy > 0x18) continue;
        mChosenCard = lane;
        mOtherCard = lane ^ 1;
        mRevealStep = 0;
        mState += 1;
        sMgCoinsBetPlaced = 1;
        Sound::PlayBank2_2D(0x153);
    }
}

// @symbol _ZN10dScMgBSC_c14StateMoveCardsEv
/* State 3: moves the chosen card down and the other up, then primes the
 * 30-frame timer. */
void dScMgBSC_c::StateMoveCards()
{
    int chosenArrived = ApproachLinear(mCardPos[mChosenCard].y, 0x90000, 0x4000) & 1;
    int otherArrived = ApproachLinear(mCardPos[mOtherCard].y, 0x30000, 0x4000) & 1;

    if (chosenArrived == 0) return;
    if (otherArrived == 0) return;

    mStateTimer = 0x1e;
    mState += 1;
}

// @symbol _ZN10dScMgBSC_c13StateWaitMoveEv
/* State 4: waits out the timer, then plays the move sound. */
void dScMgBSC_c::StateWaitMove()
{
    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;
    Sound::PlayBank2_2D(0x148);
    mState = mState + 1;
}

// @symbol _ZN10dScMgBSC_c16StateRevealCardsEv
/* State 5: flips both cards, one frame every four ticks, until the other
 * card is face up. Then settles the stakes: the higher card wins, which
 * adds twice gMgCoinsInPlay to mCredits and one to the score; a loss
 * takes one away. */
void dScMgBSC_c::StateRevealCards()
{
    int i;
    mRevealStep += 1;
    if (mRevealStep < 4)
        return;
    mRevealStep = 0;
    for (i = 0; i < 2; i++)
        mCardFrame[i] += 1;
    if (mCardFrame[mOtherCard] < 4)
        return;
    mStateTimer = 0x1e;
    mState += 1;
    mPromptEnabled = 0;
    {
        u8 chosen = mCardValue[mChosenCard];
        u8 other = mCardValue[mOtherCard];
        int base = mCredits;
        int score = mHudScore;
        if (chosen > other) {
            base += gMgCoinsInPlay * 2;
            score = score + 1;
        } else {
            score = score - 1;
        }
        Mg_SaveRecordPair(base, score);
    }
}

// @symbol _ZN10dScMgBSC_c15StateJudgeCardsEv
/* State 6: 30 frames after the flip, bumps the score (capped at 9999) and
 * the best score on a win, plays jingle 4 or 5, queues the table's win or
 * loss reaction, and parks in state 13. The bump goes through a pointer on
 * purpose: the ROM takes the member's address first. */
void dScMgBSC_c::StateJudgeCards()
{
    int *score;

    mStateTimer = mStateTimer - 1;
    if (mStateTimer > 0)
        return;

    if (mCardValue[mChosenCard] > mCardValue[mOtherCard])
    {
        if (mHudScore < 0x270f)
        {
            score = &mHudScore;
            *score = *score + 1;
        }
        if (mHudScore > mBestHudScore)
            mBestHudScore = mHudScore;
        Mg_SubmitScore(mHudScore);
        Mg_SetSharedState(MG_STATE_WIN);
        Dealer_PlayWin(&mShared);
        mState = 0xd;
    }
    else
    {
        if (mHudScore > 0)
        {
            score = &mHudScore;
            *score = *score - 1;
        }
        Mg_SetSharedState(MG_STATE_LOSE);
        Dealer_PlayLose(&mShared);
        mState = 0xd;
    }
}

// @symbol _ZN10dScMgBSC_c18StateTableReactionEv
/* States 7 and 8: queue the table's win or loss reaction. Nothing in this
 * file sets state 7 or 8; StateJudgeCards does the same work inline. */
void dScMgBSC_c::StateTableReaction()
{
    if (mState == 7) {
        Dealer_PlayWin(&mShared);
        mState = 9;
    } else {
        Dealer_PlayLose(&mShared);
        mState = 0xa;
    }
}

// @symbol _ZN10dScMgBSC_c14StateWaitTableEv
/* States 9 and 10: wait for the table's reaction to finish, then pay out
 * a win (state 12) or play the loss sting and gather the cards (state 11). */
void dScMgBSC_c::StateWaitTable()
{
    if (Dealer_IsIdle((int *)&mShared) == 0)
        return;
    if (mState == 9) {
        MgCoins_StartPayout(gMgCoinsInPlay << 1);
        mState = 0xc;
    } else {
        MgCoins_StartLose();
        mState = 0xb;
    }
}

// @symbol _ZN10dScMgBSC_c16StateReturnCardsEv
/* State 11, while gMgCoinsStatus is set: if the base score is spent,
 * first walks both cards back to the deck (mDealCount re-read every pass,
 * as in StateWaitDeal). Then refreshes the meter and settles in state 12. */
void dScMgBSC_c::StateReturnCards()
{
    int done;
    int i;
    CardPos *pos;

    if (gMgCoinsStatus != 1)
    {
        return;
    }
    if (mCredits <= 0)
    {
        done = 0;
        i = 0;
        if (mDealCount > 0)
        {
            pos = &mCardPos[0];
            do
            {
                if (Math_Function_0203b14c((int *)pos, -0x40000, 0x800, 0xc000, 0x200) == 0)
                {
                    done++;
                }
                i++;
                pos++;
            } while (i < mDealCount);
        }
        if (done < 2)
        {
            return;
        }
    }
    Mg_ResumeSharedState();
    mState = 0xc;
}

// @symbol _ZN10dScMgBSC_c9StateDoneEv
/* States 12 and 13: at rest. */
void dScMgBSC_c::StateDone()
{
}

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
