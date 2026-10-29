/* Memory Master (MG_MEMORY_J). Twenty cards, three player markers, one cursor.
 * Deal them face-up, hide them, then touch matching pairs.
 *
 * RTTI: dScMgMemory2_c : dScMgSingle3DBase_c. This tree first shipped the
 * class under the coined name MgMemoryMaster; the ROM disagrees. The vptr is
 * ov006:0x0213d4d4, and the word below it is _ZTI14dScMgMemory2_c. The
 * factory is dScMgMemory2_c_classInit (historical alias
 * MgMemoryMaster_Spawn). SIZE 0x5410, from classInit's
 * fBase_c::operator new(0x5410). The out-of-line destructor emits D1
 * 0x020f5564 then D0 0x020f55b8.
 *
 * THE DEALER at 0x4f38: a Dealer (include/Dealer.h, 0x270 bytes), the
 * same record the card, roulette, BSC, Monte Carlo and Memory Match scenes
 * embed there. The compiler builds and destroys it.
 */
#ifndef DSCMGMEMORY2_C_H
#define DSCMGMEMORY2_C_H
#include "dScMgSingle3DBase_c.h"
#include "Dealer.h"

struct dMgMemory2Card_c {
    s32 x;
    s32 y;
    s32 speed;
    s16 angle;
    u16 animTimer;
    u8 value;
    u8 unk_11;
    u8 visible;
    u8 active;
    u8 state;
    u8 frame;
    u8 flyAwayStarted;
    u8 pad_17;
};

struct dMgMemory2Player_c {
    s32 x;
    s32 y;
    s32 speed;
    s16 angle;
    s16 delay;
    u8 active;
    u8 unk_11;
    u8 state;
    u8 pad_13;
};

struct dMgMemory2Cursor_c {
    s32 x;
    s32 y;
    s16 blinkTimer;
    u8 pad_0a[2];
    u8 visible;
    u8 frame;
    u8 enabled;
    u8 pad_0f;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgMemory2Card_c_size_must_be_0x18[sizeof(dMgMemory2Card_c) == 0x18 ? 1 : -1];
typedef char dMgMemory2Player_c_size_must_be_0x14[sizeof(dMgMemory2Player_c) == 0x14 ? 1 : -1];
typedef char dMgMemory2Cursor_c_size_must_be_0x10[sizeof(dMgMemory2Cursor_c) == 0x10 ? 1 : -1];
#endif

struct dScMgMemory2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMemory2_c();

    /* The out-of-line destructor is the key function, so this TU emits
       _ZTV/_ZTI/_ZTS14dScMgMemory2_c as compiler-only output; the manifest
       licenses the cartridge copies at 0x0213d4d4, 0x0213d350 and 0x0213d35c
       as deadstrip data. */
    s32 InitResources();               /* slot  0 -- 0x020f74b4 */
    virtual void StartRound(int arg);  /* slot 18 */
    virtual int CanEndState(int mode); /* slot 19 */
    virtual void OnDealDone();         /* slot 21 */
    s32 Behavior();                    /* slot  6 -- 0x020f7458 */
    s32 Render();                      /* slot  9 -- 0x020f73f4 */

    /* Numeric order follows the five pointer-to-member dispatch tables. */
    enum SceneState { STATE_SETUP = 0, STATE_PLAY, STATE_JUDGE_PAIRS, STATE_RESULT, STATE_EXIT };

    enum CardState {
        CARD_MOVE = 0,
        CARD_IDLE,
        CARD_SELECT,
        CARD_FLIP_UP,
        CARD_WAIT,
        CARD_FLIP_DOWN,
        CARD_FLY_AWAY
    };

    enum PlayerState { PLAYER_DROP = 0, PLAYER_MOVE, PLAYER_WAIT };

    enum RoundState {
        ROUND_START = 0,
        ROUND_DEAL_EASY,
        ROUND_DEAL_NORMAL,
        ROUND_DEAL_HARD,
        ROUND_DEAL_FOURTH,
        ROUND_SHOW_CARDS,
        ROUND_HIDE_CARDS,
        ROUND_READY_CARDS,
        ROUND_WAIT_DEAL,
        ROUND_REVEAL
    };

    enum ResultState { RESULT_WAIT = 0, RESULT_REWARD, RESULT_TURN_CARDS, RESULT_FINISH };

    void DrawMessage();
    void DrawCursor();
    void UpdateCursor();
    void ShowCursor();
    void HideCursor();

    void PlayerWait(int player);
    void PlayerMove(int player);
    void PlayerDrop(int player);
    void InitPlayers();

    void CheckFinished();
    void JudgePair();
    void DrawCards();
    void UpdateCards();

    void CardFlyAway(int card);
    void CardFlipDown(int card);
    void CardWait(int card);
    void CardFlipUp(int card);
    void CardSelect(int card);
    void CardIdle(int card);
    void CardMove(int card);

    void ResultFinish();
    void ResultTurnCards();
    void ResultReward();
    void ResultWait();

    void RoundReveal();
    void *RoundWaitDeal();
    void RoundReadyCards();
    void RoundHideCards();
    void RoundShowCards();
    void RoundDealFourth();
    void RoundDealHard();
    void RoundDealNormal();
    void RoundDealEasy();
    void RoundStart();

    void ShuffleCards();
    void ChoosePreviewCards();
    void ResetGame();
    void StateExit();
    void StateResult();
    void StateJudge();
    void StatePlay();
    void StateSetup();
    void SetupDifficulty();

    Dealer mDealer;                  /* 0x4f38 */
    dMgMemory2Card_c mCards[20];     /* 0x51a8 */
    dMgMemory2Player_c mPlayers[3];  /* 0x5388 */
    dMgMemory2Cursor_c mCursor;      /* 0x53c4 */
    s32 mState;                      /* 0x53d4 */
    s32 mSubstate;                   /* 0x53d8 */
    u8 pad_53dc[4];                  /* 0x53dc */
    u16 mRoundTimer;                 /* 0x53e0 */
    u16 mCardTimer;                  /* 0x53e2 */
    u16 mResultTimer;                /* 0x53e4 */
    s16 mDealSignals;                /* 0x53e6 -- incremented by the dealer animation */
    s16 mCardsDealt;                 /* 0x53e8 -- cards activated by round-deal states */
    u16 mTargetPairs;                /* 0x53ea */
    u16 mPreviewTimer;               /* 0x53ec */
    u8 mSelectedValues[2];           /* 0x53ee */
    u8 mSelectedCards[2];            /* 0x53f0 */
    u8 mValueCounts[11];             /* 0x53f2 */
    u8 mPreviewCards[8];             /* 0x53fd */
    u8 mPairsFound;                  /* 0x5405 */
    u8 mSelectedCount;               /* 0x5406 */
    u8 unk_5407;                     /* 0x5407 */
    u8 mMisses;                      /* 0x5408 */
    u8 mMaxMisses;                   /* 0x5409 */
    u8 mDifficulty;                  /* 0x540a */
    u8 mMessageVisible;              /* 0x540b */
    u8 mInputSeen;                   /* 0x540c */
    u8 pad_540d[3];                  /* 0x540d */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgMemory2_c_size_must_be_0x5410[sizeof(dScMgMemory2_c) == 0x5410 ? 1 : -1];
#endif

#endif
