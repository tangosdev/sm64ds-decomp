/* Memory Master uses the ROM class dScMgMemory2_c, confirmed by RTTI:
 * dScMgMemory2_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The THIRTEENTH and last direct child of that base;
 * with this file the family is complete.
 *
 * SAME COINED-NAME STORY AS ITS SIBLING -- see include/dScMgMemory_c.h's
 * own banner for the full version. Short form: this class shipped as
 * `_ZN14MgMemoryMasterD1Ev`/`D0Ev` and `_ZTV14MgMemoryMaster`, and the ROM
 * disagrees. The vptr value is ov006:0x0213d4d4; the RTTI pointer one word
 * below it is `_ZTI14dScMgMemory2_c`, whose `_ZTS` string reads
 * "14dScMgMemory2_c". The factory now spells `dScMgMemory2_c_classInit`
 * (historical alias MgMemoryMaster_Spawn), already
 * attributed. "MgMemoryMaster" and "dScMgMemory2_c" are both 14 characters,
 * so the rename is length-neutral and no mangled prefix changes.
 *
 * SIZE 0x5410, from dScMgMemory2_c_classInit's own `_ZN7fBase_cnwEj(0x5410)`.
 *
 * The reconstructed scene TU proves twenty 0x18-byte cards, three 0x14-byte
 * player markers, a cursor/HUD object, and the state-machine scalars below.
 * Address-only handler spellings are descriptive and disclosed as coined in
 * symbols/actor_renames.tsv.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN14dScMgMemory2_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMgMemory2_c. The reconstructed factory
 * dScMgMemory2_c_classInit (historical alias MgMemoryMaster_Spawn) installs this class's
 * cartridge vtable for the MG_MEMORY_J registry profile.
 */
#ifndef DSCMGMEMORY2_C_H
#define DSCMGMEMORY2_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c1c64(char *t); /* decl_common.h's own signature */

struct dMgMemory2SharedState_c {
    ~dMgMemory2SharedState_c() { func_ov006_020c1c64((char *)this); }
    u8 pad_000[0x1e6];
    s16 ready;
    u8 pad_1e8[0x88];
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
    s16 angle;
    u8 pad_0a[2];
    u8 visible;
    u8 frame;
    u8 enabled;
    u8 pad_0f;
};

typedef char dMgMemory2SharedState_c_size_must_be_0x270[sizeof(dMgMemory2SharedState_c) == 0x270 ? 1 : -1];
typedef char dMgMemory2Card_c_size_must_be_0x18[sizeof(dMgMemory2Card_c) == 0x18 ? 1 : -1];
typedef char dMgMemory2Player_c_size_must_be_0x14[sizeof(dMgMemory2Player_c) == 0x14 ? 1 : -1];
typedef char dMgMemory2Cursor_c_size_must_be_0x10[sizeof(dMgMemory2Cursor_c) == 0x10 ? 1 : -1];

struct dScMgMemory2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMemory2_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgSingle3DBase_c's. Spelled WITHOUT the `virtual`
       keyword, the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h
       spell theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so each reuses an existing slot and adds no
       field, and the 0x5410 assert below still holds. The destructor above is
       declared first and out of line, so it stays this class's KEY FUNCTION and
       none of these translation units emits _ZTV14dScMgMemory2_c. */
    s32 InitResources();   /* slot  0 -- src/_ZN14dScMgMemory2_c13InitResourcesEv.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    virtual void OnGroundPounded();                    /* slot 21 */
    s32 Behavior();        /* slot  6 -- src/_ZN14dScMgMemory2_c8BehaviorEv.cpp */
    s32 Render();          /* slot  9 -- src/_ZN14dScMgMemory2_c6RenderEv.cpp */

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

typedef char dScMgMemory2_c_size_must_be_0x5410[sizeof(dScMgMemory2_c) == 0x5410 ? 1 : -1];

#endif
