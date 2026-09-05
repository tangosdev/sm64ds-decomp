/* Memory Match uses the ROM class dScMgMemory_c. RTTI at
 * ov006:0x0213d090 names the class and its vtable begins at 0x0213d1b8.
 * The English minigame name is descriptive evidence, not a substitute for
 * that ROM-proven identity.
 *
 * SIZE 0x5340, measured by the unique MG_MEMORY factory at 0x020f5504.
 * The complete scene TU proves twelve 0x18-byte cards, three 0x14-byte
 * player markers, a cursor/HUD object, and the state-machine scalars below.
 * Address-only handler spellings are descriptive and disclosed as coined in
 * symbols/actor_renames.tsv. */
#ifndef DSCMGMEMORY_C_H
#define DSCMGMEMORY_C_H

#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c1c64(char *state);

struct dMgMemorySharedState_c {
    ~dMgMemorySharedState_c() { func_ov006_020c1c64((char *)this); }
    u8 pad_000[0x1e6];
    s16 ready;       /* 0x1e6 -- set when a new Memory Match round starts */
    u8 pad_1e8[0x88];
};

struct dMgMemoryCard_c {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 speed;          /* 0x08 */
    s16 angle;          /* 0x0c */
    u16 animTimer;      /* 0x0e */
    u8 value;           /* 0x10 */
    u8 unk_11;          /* 0x11 */
    u8 visible;         /* 0x12 */
    u8 active;          /* 0x13 */
    u8 state;           /* 0x14 */
    u8 frame;           /* 0x15 */
    u8 flyAwayStarted;  /* 0x16 */
    u8 pad_17;          /* 0x17 */
};

struct dMgMemoryPlayer_c {
    s32 x;          /* 0x00 */
    s32 y;          /* 0x04 */
    s32 speed;      /* 0x08 */
    s16 angle;      /* 0x0c */
    s16 delay;      /* 0x0e */
    u8 active;      /* 0x10 */
    u8 unk_11;      /* 0x11 */
    u8 state;       /* 0x12 */
    u8 pad_13;      /* 0x13 */
};

struct dMgMemoryCursor_c {
    s32 x;          /* 0x00 */
    s32 y;          /* 0x04 */
    s16 angle;      /* 0x08 */
    u8 pad_0a[2];   /* 0x0a */
    u8 visible;     /* 0x0c */
    u8 frame;       /* 0x0d */
    u8 enabled;     /* 0x0e */
    u8 pad_0f;      /* 0x0f */
};

typedef char dMgMemorySharedState_c_size_must_be_0x270[sizeof(dMgMemorySharedState_c) == 0x270 ? 1 : -1];
typedef char dMgMemoryCard_c_size_must_be_0x18[sizeof(dMgMemoryCard_c) == 0x18 ? 1 : -1];
typedef char dMgMemoryPlayer_c_size_must_be_0x14[sizeof(dMgMemoryPlayer_c) == 0x14 ? 1 : -1];
typedef char dMgMemoryCursor_c_size_must_be_0x10[sizeof(dMgMemoryCursor_c) == 0x10 ? 1 : -1];

struct dScMgMemory_c : dScMgSingle3DBase_c {
    virtual ~dScMgMemory_c();

    s32 InitResources();
    s32 Behavior();
    s32 Render();
    virtual void OnYoshiTryEat(int arg);    /* slot 18 */
    virtual int OnTurnIntoEgg(int mode);    /* slot 19 */
    virtual void OnGroundPounded();         /* slot 21 */

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

    void ResultTurnCards();
    void ResultReward();
    void ResultWait();
    void ResultFinish();

    void RoundReveal();
    void* RoundWaitDeal();
    void RoundDealHard();
    void RoundDealNormal();
    void RoundDealEasy();
    void RoundStart();

    void ShuffleCards();
    void ResetGame();
    void StateExit();
    void StateResult();
    void StateJudge();
    void StatePlay();
    void StateSetup();
    void SetupDifficulty();

    dMgMemorySharedState_c mShared; /* 0x4f38 */
    dMgMemoryCard_c mCards[12];     /* 0x51a8 */
    dMgMemoryPlayer_c mPlayers[3];  /* 0x52c8 */
    dMgMemoryCursor_c mCursor;      /* 0x5304 */
    s32 mState;                     /* 0x5314 */
    s32 mSubstate;                  /* 0x5318 */
    u8 pad_531c[4];                 /* 0x531c */
    u16 mRoundTimer;                /* 0x5320 */
    u16 mCardTimer;                 /* 0x5322 */
    u16 mResultTimer;               /* 0x5324 */
    s16 mReadyCount;                /* 0x5326 -- signed comparisons in deal states */
    s16 mDealCount;                 /* 0x5328 -- signed comparisons in deal states */
    u16 mTargetPairs;               /* 0x532a */
    u8 mSelectedValues[2];          /* 0x532c */
    u8 mSelectedCards[2];           /* 0x532e */
    u8 mValueCounts[7];             /* 0x5330 */
    u8 mPairsFound;                 /* 0x5337 */
    u8 mSelectedCount;              /* 0x5338 */
    u8 unk_5339;                    /* 0x5339 */
    u8 mMisses;                     /* 0x533a */
    u8 mMaxMisses;                  /* 0x533b */
    u8 mDifficulty;                 /* 0x533c */
    u8 mMessageVisible;             /* 0x533d */
    u8 mInputSeen;                  /* 0x533e */
    u8 pad_533f;                    /* 0x533f */
};

typedef char dScMgMemory_c_size_must_be_0x5340[sizeof(dScMgMemory_c) == 0x5340 ? 1 : -1];

#endif
