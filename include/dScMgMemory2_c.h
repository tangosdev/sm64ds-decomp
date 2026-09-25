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
 * mShared is what func_ov006_020c1d80 builds and func_ov006_020c1c64 tears
 * down. The real BlendModelAnim / Model / ShadowModel stay byte arrays:
 * a member with a destructor would run twice.
 */
#ifndef DSCMGMEMORY2_C_H
#define DSCMGMEMORY2_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c1c64(char *t); /* decl_common.h's own signature */

extern "C" int func_ov006_020c1d80(char *sharedState);

struct dMgMemory2SharedState_c {
    dMgMemory2SharedState_c() { func_ov006_020c1d80((char *)this); }
    ~dMgMemory2SharedState_c() { func_ov006_020c1c64((char *)this); }

    u8  pad_000[0x1a];        /* 0x000 */
    s16 unk_01a;              /* 0x01a -- func_ov006_020c1d80 stores 0 */
    u8  blendModelAnim[0x70]; /* 0x01c -- BlendModelAnim */
    u8  model[0x50];          /* 0x08c -- Model */
    /* func_ov006_020c0a48: file handles, a second BlendModelAnim at +0x18,
       ShadowModel at +0x88. */
    u8  at_0dc[0xf8];         /* 0x0dc */
    u8  pad_1d4[0x8];         /* 0x1d4 */
    s16 unk_1dc;              /* 0x1dc -- func_ov006_020c1d80 stores 0 */
    u8  pad_1de[0x8];         /* 0x1de */
    s16 ready;                /* 0x1e6 */
    u8  filePtr[0x78];        /* 0x1e8 -- 15 SharedFilePtr, through +0x258 */
    s32 unk_260;              /* 0x260 */
    s32 unk_264;              /* 0x264 */
    s32 unk_268;              /* 0x268 */
    s32 unk_26c;              /* 0x26c */
};

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
typedef char dMgMemory2SharedState_c_size_must_be_0x270[sizeof(dMgMemory2SharedState_c) == 0x270 ? 1 : -1];
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
    s32 InitResources();   /* slot  0 -- 0x020f74b4 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual void OnGroundPounded();                    /* slot 21 */
    s32 Behavior();        /* slot  6 -- 0x020f7458 */
    s32 Render();          /* slot  9 -- 0x020f73f4 */

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

    dMgMemory2SharedState_c mShared; /* 0x4f38 */
    dMgMemory2Card_c mCards[20];     /* 0x51a8 */
    dMgMemory2Player_c mPlayers[3];  /* 0x5388 */
    dMgMemory2Cursor_c mCursor;      /* 0x53c4 */
    s32 mState;                      /* 0x53d4 */
    s32 mSubstate;                   /* 0x53d8 */
    u8 pad_53dc[4];                  /* 0x53dc */
    u16 mRoundTimer;                 /* 0x53e0 */
    u16 mCardTimer;                  /* 0x53e2 */
    u16 mResultTimer;                /* 0x53e4 */
    s16 mReadyCount;                 /* 0x53e6 */
    s16 mDealCount;                  /* 0x53e8 */
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
