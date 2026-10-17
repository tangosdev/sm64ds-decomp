//cpp
/* dMgState_c -- the minigame scene's little state machine, and the rest of its
 * translation unit: 52 functions, ov004 .text 0x020b67e8..0x020b8c18.
 *
 * dScMgBase_c keeps one of these by value and drives it once a frame: Behavior
 * counts the state timer down and calls the current state's per-frame handler,
 * Render calls the current state's draw handler, and SetState swaps both out.
 * Every handler is a pointer to a dMgState_c member, so the table below is a
 * table of member pointers rather than of plain function addresses.
 *
 * The four methods sit in the middle of the unit. Below them, 0x020b67e8 to
 * 0x020b8714, are the state handlers and the helpers they share. The .data
 * records at 0x020bc8bc..0x020bca3c hold 46 pointer-to-member constants over
 * 43 of them (the twenty-entry enter table at 0x020bc92c..0x020bca44 among
 * them), so all 43 are written as dMgState_c members under their address
 * names. The remaining five are genuine free helpers that write the shared
 * .data window (Mg_SetNextBanner, 020b67f8, 020b6808, 020b682c and
 * Mg_ResetBannerLayout).
 *
 * Edges. 0x020b67e8 is the first function after Coin_Construct, the last
 * destructor helper __sinit_ov004_020b9ad0 registers for the preceding unit
 * (the 0x14-entry array at sMgCoins). 0x020b8c18 is the start of
 * dMgPsOpt_c. The unit's .data window, 0x020bc850..0x020bca44, is read only
 * by this unit's functions and ends before dMgPsOpt_c's at 0x020bca58; the
 * words at gMgCoinsShowDelta and sMgCoinsDeltaShown belong to the preceding
 * unit, which this unit only writes. The unit emits no static initializer of
 * its own, so none of the four __sinit_ov004_* records belongs to it.
 *
 * mwccarm emits one .text section per function and lays them out in the reverse
 * of source order, so the functions are written here highest-ROM-address first.
 *
 * The cartridge proves the 0x28-byte layout, the twenty-entry enter table, the
 * construction order and the three callback roles. The class and member names
 * are inferred from the dMg* minigame family; see include/dMgState_c.h.
 *
 * Many states share one shape. The enter handler sets mTimer, shows a
 * graphic through Mg_ShowBanner (file index, x, y; the x/y words at
 * sMgScoreX..0x020bc8b8, except 0x020bc8a0, the file-index
 * override, are set by Mg_ResetBannerLayout and patched by
 * Mg_ResetRecordBannerPos and Mg_PlaceBannersOnTopScreen) and installs a
 * behavior. sMgNextBanner replaces the next such file index once;
 * 0x1d there means no override. The behavior waits for mTimer to run out
 * and for the scene's CanEndState(mState) to return nonzero, in several
 * states around a screen fade (SetForwardTime out, then SetBackwardTime
 * once the fader IsAtEnd). It then frees every graphics slot
 * (FreeGfxSlotsById(0x1d)), passes mState to Mg_CallStartRound and
 * returns the machine to idle (mState -1).
 *
 * Notes:
 *   - The .data member-pointer records keep their data_ov004_* names and
 *     are declared as dMgState_c::Callback; the comment on each extern names
 *     the member it points at.
 *   - Base (the scene) and Obj (the fader) are TU-local views; each says
 *     why it is not the shared header's class. Their names, and the W2 type
 *     of data_02086b58, are the spellings config/decl-agreement-baseline.json
 *     records for those externs.
 *   - mState doubles as the id passed to the scene's CanEndState and,
 *     through Mg_CallStartRound, to its StartRound. The `(void *)mState`
 *     spelling follows that helper's declaration.
 *
 * Retired one-function sources (ROM address order):
 *   0x020b67e8  Mg_SetNextBanner
 *   0x020b67f8  Mg_SetPerfectBanner
 *   0x020b6808  Mg_ResetRecordBannerPos
 *   0x020b682c  Mg_PlaceBannersOnTopScreen
 *   0x020b68e8  DrawRecordTable
 *   0x020b6948  DrawScoreDigits
 *   0x020b6ad8  DrawScore
 *   0x020b6b40  Finished_Update
 *   0x020b6c10  Finished_Enter
 *   0x020b6c9c  GameOver_Update
 *   0x020b6d6c  GameOver_Enter
 *   0x020b6ddc  ResultsNewRecord_Update
 *   0x020b6f14  ResultsNewRecord_Enter
 *   0x020b6f88  Results_Update
 *   0x020b7020  Results_Enter
 *   0x020b70b4  BannerEThenRound_Enter
 *   0x020b7124  UpdateFadeThenResume
 *   0x020b724c  DigitsThenRound_Enter
 *   0x020b72d4  UpdateRetryPrompt
 *   0x020b743c  RetryPromptWideB_Enter
 *   0x020b7460  RetryPromptWide_Enter
 *   0x020b746c  ScoreThenRound_Update
 *   0x020b7594  ScoreThenRound_Enter
 *   0x020b75e4  RetryPromptB_Update
 *   0x020b7744  RetryPromptB_Enter
 *   0x020b77b4  RoundClearTop_Update
 *   0x020b7854  RoundClearTop_Enter
 *   0x020b78f4  UpdateWaitThenResume
 *   0x020b798c  Tie_Enter
 *   0x020b79b0  Draw_Enter
 *   0x020b7a18  UpdateAfterHidden
 *   0x020b7b20  LoseResume_Update
 *   0x020b7b90  LoseResume_Enter
 *   0x020b7c04  UpdateGameOverToMenu
 *   0x020b7cd0  Lose_Update
 *   0x020b7e38  Lose_Enter
 *   0x020b7eac  Win_Update
 *   0x020b7f5c  Win_Enter
 *   0x020b7fec  UpdateRetryFadeOut
 *   0x020b8098  RetryPrompt_Update
 *   0x020b81f8  RetryPrompt_Enter
 *   0x020b8284  RoundBanner5_Update
 *   0x020b83ac  RoundBanner5_Enter
 *   0x020b841c  NextRound_Update
 *   0x020b853c  NextRound_Enter
 *   0x020b8560  RoundClear_Update
 *   0x020b8688  RoundClear_Enter
 *   0x020b8a8c  Mg_ResetBannerLayout
 */

#include "dMgState_c.h"
#include "dScene_c.h"
#include "dWipe_c.h"
#include "PlayerInput.h"

extern int ApproachLinear(s32 &value, s32 target, s32 step);
/* local extern: including Sound.h renumbers SetState's local static. */
namespace Sound { u32 PlayBank2_2D(u32); }

/* The minigame scene at gMgScene, seen through the two virtuals
   this unit calls and the one field it reads, under dScMgBase_c.h's names.
   Including dScMgBase_c.h instead renumbers SetState's local static
   (sEnterTable$184 becomes $208), and the manifest pins $184. Named
   parameters declared above SetState move that number too (measured: one
   name fewer gives $183), so these two views keep five between them. */
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17();
    virtual void StartRound(int);      /* slot 18, reached via Mg_CallStartRound */
    virtual int CanEndState(int mode);  /* slot 19 */
    virtual void OnNewRecord();             /* slot 20 */
    char pad[0xa4];
    s32 mCredits;                          /* 0x0a8 */
};
/* The installed screen fader at data_0209f5bc (a dFader_c), under
   dFader_c.h's slot names. The ROM sets r2 = 0 at every SetBackwardTime and
   SetForwardTime call here, which the header's one-argument spelling does
   not do, so this view declares the second argument. */
struct Obj {
    virtual void v0(); virtual void v1();
    virtual void AdvanceFade();                        /* slot 2 */
    virtual int SetBackwardTime(u32 frames, int arg);  /* slot 3 */
    virtual int SetForwardTime(u32 frames, int arg);   /* slot 4 */
    virtual int IsAtStart();                           /* slot 5 */
    virtual int IsAtEnd();                             /* slot 6 */
    virtual int IsBetweenStartAndEnd();                /* slot 7 */
};

extern "C" {
extern void FreeGfxSlotsById(int arg);
extern int GetGameLanguage(void);
extern unsigned int func_02012790(unsigned int x);
extern void func_02012dd0(void* c);
extern void Mg_CommitRecords(void);
extern int Mg_GetSubmittedScore(void);
extern int Mg_IsNewRecord(void);
extern void Mg_RestoreMusic(void);
extern void Mg_PlayJingle(void* c);
extern void Mg_StartMusic(void);
extern void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *d);
extern int Mg_GetTopScreenYOffset(void);
extern int Mg_CheckBannerButton(int arg);
extern void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void Mg_DrawBlinkSprite(void *, int, int);
extern void Mg_DrawRecordTable(void *);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
extern void Mg_CallStartRound(void* c, void* arg);
extern void Mg_SetNextBanner(int);
}

struct W2 { int w[2]; };
extern W2 data_02086b58;  /* __ptmf_null's address: the null member pointer */
extern Obj* data_0209f5bc;
extern dWipe_c data_0209f61c;
extern Base* gMgScene;  /* local extern: seen through the TU-local Base view (see Base above) */
extern int sMgTopScreenYOffset;
extern int sMgBannerButtonsArmed;
extern int gMgCoinsShowDelta;
extern int sMgCoinsDeltaShown;

extern int sMgScoreX;
extern int sMgRecordBannerX;
extern int sMgDrawBannerY;
extern int sMgScoreDigitsX;
extern int sMgWinBannerY;
extern int sMgResultsBannerY;
extern int sMgDrawBannerX;
extern int sMgQuitX;
extern int sMgScoreDigitsY;
extern int sMgRetryBannerY;
extern int sMgLoseBannerX;
extern int sMgRecordBannerY;
extern int sMgClearBannerX;
extern int sMgClearBannerY;
extern int sMgResultsBannerX;
extern int sMgWinBannerX;
extern int sMgLoseBannerY;
extern int sMgRetryBannerX;
extern int sMgPlayAgainY;
extern int sMgScoreY;
extern int sMgNextBanner;
extern int sMgQuitY;
extern int sMgPlayAgainX;
extern int sMgTopBannerY;
extern int sMgTopBannerX;
extern int sMgMainBannerY;
extern int sMgMainBannerX;
extern unsigned char *sMgStateTextSpritesByLanguage[];

/* Behavior and render records. Each comment names the member the record
   points at (ov004 relocs.txt). */
extern dMgState_c::Callback pmf_dMgState_c_NextRound_Update;    /* NextRound_Update */
extern dMgState_c::Callback pmf_dMgState_c_RoundBanner5_Update; /* RoundBanner5_Update */
extern dMgState_c::Callback pmf_dMgState_c_DrawScore;  /* DrawScore */
extern dMgState_c::Callback pmf_dMgState_c_RetryPrompt_Update;  /* RetryPrompt_Update */
extern dMgState_c::Callback pmf_dMgState_c_UpdateRetryFadeOut;  /* UpdateRetryFadeOut */
extern dMgState_c::Callback pmf_dMgState_c_Results_Update;      /* Results_Update */
extern dMgState_c::Callback pmf_dMgState_c_Win_Update;          /* Win_Update */
extern dMgState_c::Callback pmf_dMgState_c_Lose_Update;         /* Lose_Update */
extern dMgState_c::Callback pmf_dMgState_c_UpdateRetryPrompt;  /* UpdateRetryPrompt */
extern dMgState_c::Callback pmf_dMgState_c_UpdateGameOverToMenu;  /* UpdateGameOverToMenu */
extern dMgState_c::Callback pmf_dMgState_c_LoseResume_Update;     /* LoseResume_Update */
extern dMgState_c::Callback pmf_dMgState_c_UpdateRetryFadeOut_2;  /* UpdateRetryFadeOut */
extern dMgState_c::Callback pmf_dMgState_c_DrawRecordTable;  /* DrawRecordTable */
extern dMgState_c::Callback pmf_dMgState_c_GameOver_Update;  /* GameOver_Update */
extern dMgState_c::Callback pmf_dMgState_c_UpdateAfterHidden;  /* UpdateAfterHidden */
extern dMgState_c::Callback pmf_dMgState_c_UpdateGameOverToMenu_2;  /* UpdateGameOverToMenu */
extern dMgState_c::Callback pmf_dMgState_c_UpdateFadeThenResume;  /* UpdateFadeThenResume */
extern dMgState_c::Callback pmf_dMgState_c_UpdateWaitThenResume;  /* UpdateWaitThenResume */
extern dMgState_c::Callback pmf_dMgState_c_UpdateWaitThenResume_2;  /* UpdateWaitThenResume */
extern dMgState_c::Callback pmf_dMgState_c_DrawScoreDigits;  /* DrawScoreDigits */
extern dMgState_c::Callback pmf_dMgState_c_ResultsNewRecord_Update; /* ResultsNewRecord_Update */
extern dMgState_c::Callback pmf_dMgState_c_RoundClearTop_Update;    /* RoundClearTop_Update */
extern dMgState_c::Callback pmf_dMgState_c_RetryPromptB_Update;     /* RetryPromptB_Update */
extern dMgState_c::Callback pmf_dMgState_c_UpdateFadeThenResume_2;  /* UpdateFadeThenResume */
extern dMgState_c::Callback pmf_dMgState_c_UpdateRetryFadeOut_3;  /* UpdateRetryFadeOut */
extern dMgState_c::Callback pmf_dMgState_c_RoundClear_Update;     /* RoundClear_Update */
extern dMgState_c::Callback pmf_dMgState_c_ScoreThenRound_Update; /* ScoreThenRound_Update */
extern dMgState_c::Callback pmf_dMgState_c_DrawRecordTable_2;  /* DrawRecordTable */
extern dMgState_c::Callback pmf_dMgState_c_Finished_Update;    /* Finished_Update */

/* The per-state enter handlers, in state order. These are pointer-to-member
 * constants that mwccarm laid down in ov004's .data; the states they belong to
 * are not yet named, so they are still carried by address. */
extern dMgState_c::Callback pmf_dMgState_c_RoundClear_Enter;
extern dMgState_c::Callback pmf_dMgState_c_NextRound_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RoundBanner5_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RetryPrompt_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Win_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Lose_Enter;
extern dMgState_c::Callback pmf_dMgState_c_LoseResume_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Draw_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Tie_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RoundClearTop_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RetryPromptB_Enter;
extern dMgState_c::Callback pmf_dMgState_c_ScoreThenRound_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RetryPromptWide_Enter;
extern dMgState_c::Callback pmf_dMgState_c_RetryPromptWideB_Enter;
extern dMgState_c::Callback pmf_dMgState_c_DigitsThenRound_Enter;
extern dMgState_c::Callback pmf_dMgState_c_BannerEThenRound_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Results_Enter;
extern dMgState_c::Callback pmf_dMgState_c_ResultsNewRecord_Enter;
extern dMgState_c::Callback pmf_dMgState_c_GameOver_Enter;
extern dMgState_c::Callback pmf_dMgState_c_Finished_Enter;

// @symbol Mg_ResetBannerLayout
extern "C" void Mg_ResetBannerLayout(void) {
    sMgClearBannerX = 0x80;
    sMgRecordBannerX = 0x80;
    sMgClearBannerY = 0x60;
    sMgResultsBannerX = 0x80;
    sMgMainBannerX = 0x80;
    sMgRecordBannerY = 0x60;
    sMgWinBannerX = 0x80;
    sMgWinBannerY = 0x60;
    sMgLoseBannerX = 0x80;
    sMgLoseBannerY = 0x60;
    sMgResultsBannerY = -0x60;
    sMgMainBannerY = 0x40;
    sMgDrawBannerY = 0x58;
    sMgRetryBannerY = 0x28;
    sMgPlayAgainY = 0x6c;
    sMgQuitY = 0x9c;
    sMgNextBanner = 0x1d;
    sMgBannerButtonsArmed = 1;
    sMgDrawBannerX = 0x80;
    sMgTopBannerX = 0x80;
    sMgTopBannerY = 0x60;
    sMgRetryBannerX = 0x80;
    sMgPlayAgainX = 0x80;
    sMgQuitX = 0x80;
    sMgScoreX = 0x80;
    sMgScoreY = 0x60;
    sMgScoreDigitsX = 0x80;
    sMgScoreDigitsY = 0x80;
}

// @symbol _ZN10dMgState_cC1Ev
dMgState_c::dMgState_c() : mState(-1), mTimer(0), unk_020(0), mSuspended(0) {}

// @symbol _ZN10dMgState_c8SetStateEi
void dMgState_c::SetState(s32 state)
{
    static Callback sEnterTable[20] = {
        pmf_dMgState_c_RoundClear_Enter,       /*  0: RoundClear_Enter */
        pmf_dMgState_c_NextRound_Enter,        /*  1: NextRound_Enter */
        pmf_dMgState_c_RoundBanner5_Enter,     /*  2: RoundBanner5_Enter */
        pmf_dMgState_c_RetryPrompt_Enter,      /*  3: RetryPrompt_Enter */
        pmf_dMgState_c_Win_Enter,              /*  4: Win_Enter */
        pmf_dMgState_c_Lose_Enter,             /*  5: Lose_Enter */
        pmf_dMgState_c_LoseResume_Enter,       /*  6: LoseResume_Enter */
        pmf_dMgState_c_Draw_Enter,             /*  7: Draw_Enter */
        pmf_dMgState_c_Tie_Enter,              /*  8: Tie_Enter */
        pmf_dMgState_c_RoundClearTop_Enter,    /*  9: RoundClearTop_Enter */
        pmf_dMgState_c_RetryPromptB_Enter,     /* 10: RetryPromptB_Enter */
        pmf_dMgState_c_ScoreThenRound_Enter,   /* 11: ScoreThenRound_Enter */
        pmf_dMgState_c_RetryPromptWide_Enter,  /* 12: RetryPromptWide_Enter */
        pmf_dMgState_c_RetryPromptWideB_Enter, /* 13: RetryPromptWideB_Enter */
        pmf_dMgState_c_DigitsThenRound_Enter,  /* 14: DigitsThenRound_Enter */
        pmf_dMgState_c_BannerEThenRound_Enter, /* 15: BannerEThenRound_Enter */
        pmf_dMgState_c_Results_Enter,          /* 16: Results_Enter */
        pmf_dMgState_c_ResultsNewRecord_Enter, /* 17: ResultsNewRecord_Enter */
        pmf_dMgState_c_GameOver_Enter,         /* 18: GameOver_Enter */
        pmf_dMgState_c_Finished_Enter,         /* 19: Finished_Enter */
    };

    mState = state;
    mEnter = sEnterTable[mState];
    mRender = 0;
    if (mEnter)
        (this->*mEnter)();
}

// @symbol _ZN10dMgState_c8BehaviorEv
void dMgState_c::Behavior()
{
    if (mState == -1)
        return;
    ApproachLinear(mTimer, 0, 1);
    if (mBehavior == 0)
        return;
    (this->*mBehavior)();
}

// @symbol _ZN10dMgState_c6RenderEv
void dMgState_c::Render()
{
    if (mState == -1)
        return;
    if (mRender == 0)
        return;
    (this->*mRender)();
}

// @symbol _ZN10dMgState_c16RoundClear_EnterEv
void dMgState_c::RoundClear_Enter() {
    int prevId, id;
    mTimer = 0xb4;
    prevId = sMgNextBanner;
    id = 3;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgClearBannerX, sMgClearBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)4);
    mBehavior = pmf_dMgState_c_RoundClear_Update;
}

// @symbol _ZN10dMgState_c17RoundClear_UpdateEv
void dMgState_c::RoundClear_Update() {
    Base *scene = gMgScene;
    if (data_0209f5bc->IsAtEnd()) {
        data_0209f5bc->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        Mg_CallStartRound(scene, (void *)mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (data_0209f5bc->IsBetweenStartAndEnd())
            return;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        data_0209f5bc->SetForwardTime(0x1e, 0);
    }
}

// @symbol _ZN10dMgState_c15NextRound_EnterEv
void dMgState_c::NextRound_Enter() {
    mTimer = 15;
    mBehavior = pmf_dMgState_c_NextRound_Update;
}

// @symbol _ZN10dMgState_c16NextRound_UpdateEv
void dMgState_c::NextRound_Update() {
    if (data_0209f5bc->IsAtEnd() != 0) {
        Base *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        data_0209f5bc->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_CallStartRound(scene, (void *)mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (data_0209f5bc->IsBetweenStartAndEnd() != 0)
            return;
        data_0209f5bc->SetForwardTime(0x1e, 0);
    }
}

// @symbol _ZN10dMgState_c18RoundBanner5_EnterEv
void dMgState_c::RoundBanner5_Enter() {
    mTimer = 0xb4;
    Mg_ShowBanner(5, sMgRecordBannerX, sMgRecordBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)1);
    mBehavior = pmf_dMgState_c_RoundBanner5_Update;
}

// @symbol _ZN10dMgState_c19RoundBanner5_UpdateEv
void dMgState_c::RoundBanner5_Update() {
    if (data_0209f5bc->IsAtEnd()) {
        Base *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        data_0209f5bc->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        Mg_CallStartRound(scene, (void *)mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (data_0209f5bc->IsBetweenStartAndEnd() == 0)
            data_0209f5bc->SetForwardTime(0x1e, 0);
    }
}

// @symbol _ZN10dMgState_c17RetryPrompt_EnterEv
void dMgState_c::RetryPrompt_Enter() {
    int prevId, id;
    mTimer = 0x3c;
    prevId = sMgNextBanner;
    id = 8;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = pmf_dMgState_c_RetryPrompt_Update;
}

// @symbol _ZN10dMgState_c18RetryPrompt_UpdateEv
void dMgState_c::RetryPrompt_Update() {
    Obj *obj;
    if (Mg_CheckBannerButton(0)) {
        obj = data_0209f5bc;
        obj->SetForwardTime(0x1e, 0);
        mBehavior = pmf_dMgState_c_UpdateRetryFadeOut;
        return;
    }
    if (Mg_CheckBannerButton(2) || Mg_CheckBannerButton(1)) {
        obj = data_0209f5bc;
        if (obj->IsBetweenStartAndEnd())
            return;
        dScene_c::SetFaders(&data_0209f61c);
        data_0209f61c.func_0202ec9c(2);
        dScene_c::StartSceneFade(5, 0, 0);
        func_02012dd0((void *)0x3c);
        return;
    }
    if (mTimer != 0)
        return;
    Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
    Mg_ShowBanner(2, sMgQuitX, sMgQuitY, -1, -1, 0xd);
}

// @symbol _ZN10dMgState_c18UpdateRetryFadeOutEv
void dMgState_c::UpdateRetryFadeOut() {
  if (data_0209f5bc->IsAtEnd() == 0)
    return;
  void* scene = gMgScene;
  if (scene == 0)
    return;
  data_0209f5bc->SetBackwardTime(0x1e, 0);
  FreeGfxSlotsById(0x1d);
  Mg_RestoreMusic();
  Mg_StartMusic();
  Mg_CallStartRound(scene, (void *)mState);
  mTimer = 0;
  mState = -1;
  unk_020 = 0;
  mSuspended = 0;
}

// @symbol _ZN10dMgState_c9Win_EnterEv
void dMgState_c::Win_Enter() {
    int prevId, id;
    mTimer = 0x78;
    prevId = sMgNextBanner;
    id = 9;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgWinBannerX, sMgWinBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x136);
    mBehavior = pmf_dMgState_c_Win_Update;
}

// @symbol _ZN10dMgState_c10Win_UpdateEv
void dMgState_c::Win_Update() {
    if (mTimer > 0x64)
        return;
    if (mTimer != 0)
        return;
    Base *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->CanEndState(mState) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    Mg_RestoreMusic();
    Mg_StartMusic();
    Mg_CallStartRound(scene, (void *)mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

// @symbol _ZN10dMgState_c10Lose_EnterEv
void dMgState_c::Lose_Enter() {
    mTimer = 0x78;
    Mg_ShowBanner(0xa, sMgLoseBannerX, sMgLoseBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x137);
    mBehavior = pmf_dMgState_c_Lose_Update;
}

// @symbol _ZN10dMgState_c11Lose_UpdateEv
void dMgState_c::Lose_Update() {
    int state = mTimer;
    if (state > 0x64)
        return;
    Base *p = gMgScene;
    int a8 = (p != 0) ? p->mCredits : 0;
    if (a8 == 0) {
        if (state != 0)
            return;
        Base *q = gMgScene;
        if (q == 0)
            return;
        if (q->CanEndState(mState) == 0)
            return;
        gMgCoinsShowDelta = 0;
        sMgCoinsDeltaShown = 0;
        Mg_RestoreMusic();
        FreeGfxSlotsById(0x1d);
        mTimer = 0xb4;
        mBehavior = pmf_dMgState_c_UpdateGameOverToMenu;
    } else {
        if (state != 0)
            return;
        Base *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        Mg_CallStartRound(scene, (void *)mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    }
}

// @symbol _ZN10dMgState_c20UpdateGameOverToMenuEv
void dMgState_c::UpdateGameOverToMenu() {
    Obj* obj;
    int v = mTimer;
    if (v == 0xb3) {
        Mg_ShowBanner(8, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
        Mg_PlayJingle((void*)3);
        return;
    }
    if (v != 0) return;
    obj = data_0209f5bc;
    if (obj->IsBetweenStartAndEnd() != 0) return;
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.func_0202ec9c(2);
    dScene_c::StartSceneFade(5, 0, 0);
    func_02012dd0((void*)0x3c);
}

// @symbol _ZN10dMgState_c16LoseResume_EnterEv
void dMgState_c::LoseResume_Enter() {
    mTimer = 0x78;
    Mg_ShowBanner(0xa, sMgLoseBannerX, sMgLoseBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x137);
    mBehavior = pmf_dMgState_c_LoseResume_Update;
}

// @symbol _ZN10dMgState_c17LoseResume_UpdateEv
void dMgState_c::LoseResume_Update() {
    int v = mTimer;
    if (v > 0x64)
        return;
    if (v != 0)
        return;
    FreeGfxSlotsById(0x1d);
    mBehavior = pmf_dMgState_c_UpdateAfterHidden;
    mSuspended = 1;
    gMgCoinsShowDelta = 0;
    sMgCoinsDeltaShown = 0;
}

// @symbol _ZN10dMgState_c17UpdateAfterHiddenEv
void dMgState_c::UpdateAfterHidden() {
    Base *g = gMgScene;
    int cond = g ? g->mCredits : 0;
    if (cond == 0) {
        g = gMgScene;
        if (!g) return;
        if (!g->CanEndState(mState)) return;
        Mg_RestoreMusic();
        mTimer = 0xb4;
        mBehavior = pmf_dMgState_c_UpdateGameOverToMenu_2;
        return;
    }
    g = gMgScene;
    if (!g) return;
    if (!g->CanEndState(mState)) return;
    Mg_RestoreMusic();
    Mg_StartMusic();
    Mg_CallStartRound(g, (void *)mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

// @symbol _ZN10dMgState_c10Draw_EnterEv
void dMgState_c::Draw_Enter() {
    mTimer = 0x78;
    Mg_ShowBanner(0xb, sMgDrawBannerX, sMgDrawBannerY, -1, -1, 0xd);
    mBehavior = pmf_dMgState_c_UpdateWaitThenResume;
}

// @symbol _ZN10dMgState_c9Tie_EnterEv
void dMgState_c::Tie_Enter() {
    mTimer = 0;
    mBehavior = pmf_dMgState_c_UpdateWaitThenResume_2;
}

// @symbol _ZN10dMgState_c20UpdateWaitThenResumeEv
void dMgState_c::UpdateWaitThenResume() {
  Base* scene;
  if (mTimer != 0) return;
  scene = gMgScene;
  if (scene == 0) return;
  if (scene->CanEndState(mState) == 0) return;
  FreeGfxSlotsById(0x1d);
  Mg_CallStartRound(scene, (void *)mState);
  mTimer = 0;
  mState = -1;
  unk_020 = 0;
  mSuspended = 0;
}

// @symbol _ZN10dMgState_c19RoundClearTop_EnterEv
void dMgState_c::RoundClearTop_Enter() {
    int id;
    mTimer = 0xb4;
    Mg_PlayJingle((void*)4);
    id = 3;
    if (sMgNextBanner != 0x1d) { id = sMgNextBanner; sMgNextBanner = 0x1d; }
    Mg_ShowBanner(id, sMgTopBannerX, sMgTopBannerY - (sMgTopScreenYOffset + 0xc0), -1, -1, 0xa);
    mBehavior = pmf_dMgState_c_RoundClearTop_Update;
}

// @symbol _ZN10dMgState_c20RoundClearTop_UpdateEv
void dMgState_c::RoundClearTop_Update() {
    if (mTimer != 0)
        return;
    Base *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->CanEndState(mState) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    Mg_RestoreMusic();
    Mg_StartMusic();
    Mg_CallStartRound(scene, (void *)mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

// @symbol _ZN10dMgState_c18RetryPromptB_EnterEv
void dMgState_c::RetryPromptB_Enter() {
    mTimer = 0xb4;
    Mg_PlayJingle((void *)3);
    Mg_ShowBanner(8, sMgRetryBannerX, sMgRetryBannerY, -1, -1, 0xb);
    mBehavior = pmf_dMgState_c_RetryPromptB_Update;
}

// @symbol _ZN10dMgState_c19RetryPromptB_UpdateEv
void dMgState_c::RetryPromptB_Update() {
    Obj *obj;
    if (Mg_CheckBannerButton(0)) {
        obj = data_0209f5bc;
        obj->SetForwardTime(0x1e, 0);
        mBehavior = pmf_dMgState_c_UpdateRetryFadeOut_3;
        return;
    }
    if (Mg_CheckBannerButton(2) || Mg_CheckBannerButton(1)) {
        obj = data_0209f5bc;
        if (obj->IsBetweenStartAndEnd())
            return;
        dScene_c::SetFaders(&data_0209f61c);
        data_0209f61c.func_0202ec9c(2);
        dScene_c::StartSceneFade(5, 0, 0);
        func_02012dd0((void *)0x3c);
        return;
    }
    if (mTimer != 0)
        return;
    Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
    Mg_ShowBanner(1, sMgQuitX, sMgQuitY, -1, -1, 0xd);
}

// @symbol _ZN10dMgState_c20ScoreThenRound_EnterEv
void dMgState_c::ScoreThenRound_Enter() {
    mTimer = 0xb4;
    Mg_PlayJingle((void *)1);
    mBehavior = pmf_dMgState_c_ScoreThenRound_Update;
    mRender = pmf_dMgState_c_DrawScore;
}

// @symbol _ZN10dMgState_c21ScoreThenRound_UpdateEv
void dMgState_c::ScoreThenRound_Update() {
    if (data_0209f5bc->IsAtEnd()) {
        Base *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        data_0209f5bc->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        Mg_CallStartRound(scene, (void *)mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (data_0209f5bc->IsBetweenStartAndEnd() == 0)
            data_0209f5bc->SetForwardTime(0x1e, 0);
    }
}

// @symbol _ZN10dMgState_c21RetryPromptWide_EnterEv
void dMgState_c::RetryPromptWide_Enter() {
    RetryPromptWideB_Enter();
}

// @symbol _ZN10dMgState_c22RetryPromptWideB_EnterEv
void dMgState_c::RetryPromptWideB_Enter() {
    mTimer = 10;
    mBehavior = pmf_dMgState_c_UpdateRetryPrompt;
}

// @symbol _ZN10dMgState_c17UpdateRetryPromptEv
void dMgState_c::UpdateRetryPrompt() {
  if (mTimer == 0) {
    if (Mg_CheckBannerButton(0) != 0) {
      data_0209f5bc->SetForwardTime(0x1e, 0);
      mBehavior = pmf_dMgState_c_UpdateRetryFadeOut_2;
      return;
    }
    if (Mg_CheckBannerButton(2) == 0) {
      if (Mg_CheckBannerButton(1) == 0)
        return;
    }
    if (data_0209f5bc->IsBetweenStartAndEnd() != 0)
      return;
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.func_0202ec9c(2);
    dScene_c::StartSceneFade(5, 0, 0);
    func_02012dd0((void*)0x3c);
    return;
  }
  Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
  Mg_ShowBanner(2, sMgQuitX, sMgQuitY, -1, -1, 0xd);
  mTimer = 0;
}

// @symbol _ZN10dMgState_c21DigitsThenRound_EnterEv
void dMgState_c::DigitsThenRound_Enter() {
    mTimer = 0xb4;
    Mg_ShowBanner(5, sMgRecordBannerX, sMgRecordBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)1);
    mBehavior = pmf_dMgState_c_UpdateFadeThenResume;
    mRender = pmf_dMgState_c_DrawScoreDigits;
}

// @symbol _ZN10dMgState_c20UpdateFadeThenResumeEv
void dMgState_c::UpdateFadeThenResume() {
  if (data_0209f5bc->IsAtEnd()) {
    Base* scene = gMgScene;
    if (scene == 0)
      return;
    if (scene->CanEndState(mState) == 0)
      return;
    data_0209f5bc->SetBackwardTime(0x1e, 0);
    Mg_RestoreMusic();
    Mg_StartMusic();
    Mg_CallStartRound(scene, (void *)mState);
    FreeGfxSlotsById(0x1d);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
  } else {
    if (mTimer != 0)
      return;
    if (data_0209f5bc->IsBetweenStartAndEnd() == 0)
      data_0209f5bc->SetForwardTime(0x1e, 0);
  }
}

// @symbol _ZN10dMgState_c22BannerEThenRound_EnterEv
void dMgState_c::BannerEThenRound_Enter() {
    mTimer = 0x3c;
    Mg_ShowBanner(0xe, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = pmf_dMgState_c_UpdateFadeThenResume_2;
}

// @symbol _ZN10dMgState_c13Results_EnterEv
void dMgState_c::Results_Enter() {
    if (Mg_IsNewRecord()) {
        ResultsNewRecord_Enter();
        return;
    }
    Mg_ShowBanner(7, 0x80, 0x14, -1, -1, 0xd);
    mTimer = 0x78;
    mBehavior = pmf_dMgState_c_Results_Update;
    mRender = pmf_dMgState_c_DrawRecordTable;
    Mg_CommitRecords();
}

// @symbol _ZN10dMgState_c14Results_UpdateEv
void dMgState_c::Results_Update() {
    int ok;
    unsigned char idx;
    int i;
    if (mTimer != 0)
        return;
    idx = gActivePlayerSlot;
    ok = 0;
    i = idx * 4;
    if (gTouchHeld[i] != 0) {
        if (gTouchEdge[i] != 0)
            ok = 1;
    }
    if (ok == 0)
        return;
    func_02012790(0x62);
    FreeGfxSlotsById(7);
    *(W2 *)&mRender = data_02086b58;
    RetryPromptWide_Enter();
}

// @symbol _ZN10dMgState_c22ResultsNewRecord_EnterEv
void dMgState_c::ResultsNewRecord_Enter() {
    Mg_ShowBanner(7, 0x80, 0x14, -1, -1, 0xd);
    mTimer = 0x12c;
    mBehavior = pmf_dMgState_c_ResultsNewRecord_Update;
    mRender = pmf_dMgState_c_DrawRecordTable_2;
    Mg_CommitRecords();
}

// @symbol _ZN10dMgState_c23ResultsNewRecord_UpdateEv
void dMgState_c::ResultsNewRecord_Update() {
    int idx;
    int flag;
    Base *scene;
    int v;
    int s;
    int id;

    if (mTimer == 0) {
        idx = gActivePlayerSlot;
        flag = 0;
        if (gTouchHeld[idx * 4] != 0) {
            if (gTouchEdge[idx * 4] != 0)
                flag = 1;
        }
        if (flag != 0) {
            FreeGfxSlotsById(7);
            func_02012790(0x62);
            *(W2 *)&mRender = data_02086b58;
            RetryPromptWide_Enter();
        }
    }

    if (mTimer != 0xb4)
        return;

    scene = gMgScene;
    if (scene) {
        scene->OnNewRecord();
    }

    id = 5;
    v = sMgNextBanner;
    if (v != 0x1d) {
        sMgNextBanner = 0x1d;
        id = v;
    }

    s = sMgResultsBannerY;
    if (s < 0)
        s -= Mg_GetTopScreenYOffset();

    Mg_ShowBanner(id, sMgResultsBannerX, s, -1, -1, 0xd);
    Mg_PlayJingle((void*)1);
}

// @symbol _ZN10dMgState_c14GameOver_EnterEv
void dMgState_c::GameOver_Enter() {
    mTimer = 0xb4;
    Mg_ShowBanner(8, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = pmf_dMgState_c_GameOver_Update;
}

// @symbol _ZN10dMgState_c15GameOver_UpdateEv
void dMgState_c::GameOver_Update() {
    int x = mTimer;
    Base *g = gMgScene;
    if (x != 0) {
        unsigned char idx = gActivePlayerSlot;
        int off = idx * 4;
        int ok = 0;
        if (gTouchHeld[off]) {
            if (gTouchEdge[off]) ok = 1;
        }
        if (ok == 0)
            return;
    }
    if (g->CanEndState(mState) == 0)
        return;
    *(W2 *)&mRender = data_02086b58;
    FreeGfxSlotsById(0x1d);
    if (Mg_IsNewRecord() != 0) {
        ResultsNewRecord_Enter();
        return;
    }
    Results_Enter();
}

// @symbol _ZN10dMgState_c14Finished_EnterEv
void dMgState_c::Finished_Enter() {
    int prevId, id;
    mTimer = 0xb4;
    prevId = sMgNextBanner;
    id = 3;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgClearBannerX, sMgClearBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)4);
    mBehavior = pmf_dMgState_c_Finished_Update;
}

// @symbol _ZN10dMgState_c15Finished_UpdateEv
void dMgState_c::Finished_Update() {
    int x = mTimer;
    Base *g = gMgScene;
    if (x != 0) {
        unsigned char idx = gActivePlayerSlot;
        int off = idx * 4;
        int ok = 0;
        if (gTouchHeld[off]) {
            if (gTouchEdge[off]) ok = 1;
        }
        if (ok == 0)
            return;
    }
    if (g->CanEndState(mState) == 0)
        return;
    *(W2 *)&mRender = data_02086b58;
    FreeGfxSlotsById(0x1d);
    if (Mg_IsNewRecord() != 0) {
        ResultsNewRecord_Enter();
        return;
    }
    Results_Enter();
}

// @symbol _ZN10dMgState_c9DrawScoreEv
void dMgState_c::DrawScore() {
    int r;
    if (gMgScene == 0) return;
    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreX, sMgScoreY, r, -1, -1, 0, 0);
}

// @symbol _ZN10dMgState_c15DrawScoreDigitsEv
void dMgState_c::DrawScoreDigits() {
    unsigned int r;
    int idx;

    if (gMgScene == 0) return;

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 4, sMgScoreDigitsY, r / 100, -1, -1, 1, 0);

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 0x1c, sMgScoreDigitsY, r % 100 / 10, -1, -1, 1, 0);

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 0x2c, sMgScoreDigitsY, r % 10, -1, -1, 1, 0);

    idx = GetGameLanguage();
    Mg_RenderSubSpriteMtx((void *)*(int *)(sMgStateTextSpritesByLanguage[idx] + 0xc),
                          sMgScoreDigitsX - 0x24, sMgScoreDigitsY, 0);

    idx = GetGameLanguage();
    Mg_RenderSubSpriteMtx((void *)*(int *)(sMgStateTextSpritesByLanguage[idx] + 0x1c),
                          sMgScoreDigitsX + 0x10, sMgScoreDigitsY, 0);
}

// @symbol _ZN10dMgState_c15DrawRecordTableEv
void dMgState_c::DrawRecordTable() {
    void *g = gMgScene;
    if (g == 0) return;
    Mg_DrawRecordTable(g);
    if (mTimer > 0) return;
    Mg_DrawBlinkSprite(g, 0xe0, 0xa0);
}

// @symbol Mg_PlaceBannersOnTopScreen
extern "C" void Mg_PlaceBannersOnTopScreen(void)
{
    int v1 = -0x1c - Mg_GetTopScreenYOffset();
    sMgWinBannerX = 0x80;
    sMgWinBannerY = v1;
    {
        int v2 = -0x1c - Mg_GetTopScreenYOffset();
        sMgLoseBannerX = 0x80;
        sMgLoseBannerY = v2;
    }
    {
        int v3 = -0x1c - Mg_GetTopScreenYOffset();
        sMgDrawBannerX = 0x80;
        sMgDrawBannerY = v3;
        sMgResultsBannerX = 0x80;
        sMgResultsBannerY = -0x1c;
        sMgMainBannerX = 0x80;
        sMgMainBannerY = 0x60;
    }
}

// @symbol Mg_ResetRecordBannerPos
extern "C" void Mg_ResetRecordBannerPos(void)
{
    sMgResultsBannerX = 0x80;
    sMgResultsBannerY = -0x1c;
}

// @symbol Mg_SetPerfectBanner
extern "C" void Mg_SetPerfectBanner(void) {
    Mg_SetNextBanner(6);
}

// @symbol Mg_SetNextBanner
extern "C" void Mg_SetNextBanner(int v) { sMgNextBanner = v; }
