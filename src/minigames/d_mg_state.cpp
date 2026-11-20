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
 * 0x020b8714, are the state handlers and the helpers they share. The 49
 * pointer-to-member constants at 0x020bc8bc..0x020bca44 (the twenty that
 * SetState copies into its enter table among them) point at 43 of them, so all
 * 43 are written as dMgState_c members under their address names. The
 * remaining five are genuine free helpers that write the shared .data window
 * (Mg_SetNextBanner, 020b67f8, 020b6808, 020b682c and Mg_ResetBannerLayout).
 *
 * Edges. 0x020b67e8 is the first function after Sprite::Sprite, the last
 * helper the preceding unit's static initializer (0x020b9ad0) passes for
 * its 0x14-entry array sMgCoins. 0x020b8c18 is the start of
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
 * (FreeGfxSlotsById(0x1d)), passes mState to the scene's CallStartRound and
 * returns the machine to idle (mState -1).
 *
 * Notes:
 *   - Every handler is installed as &dMgState_c::X. mwccarm emits each such
 *     use as its own eight-byte {fn, 0} .data constant, so a handler installed
 *     from two places has two; those 49 constants are the run at
 *     0x020bc8bc..0x020bca44.
 *   - The scene is gMgScene, a dScMgBase_c; the states call its StartRound,
 *     CanEndState and OnNewRecord (slots 18-20) and read its mCredits.
 *     W2 preserves the two-word storage view of __ptmf_null.
 *   - mState doubles as the id passed to the scene's CanEndState and,
 *     through CallStartRound, to its StartRound.
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
#include "dScMgBase_c.h"
#include "dScene_c.h"
#include "dWipe_c.h"
#include "OamAttr.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "math/Approach.h"

extern "C" {
extern void FreeGfxSlotsById(int arg);
extern int GetGameLanguage(void);
extern void Mg_CommitRecords(void);
extern int Mg_GetSubmittedScore(void);
extern int Mg_IsNewRecord(void);
extern void Mg_RestoreMusic(void);
extern void Mg_PlayJingle(void *c);
extern void Mg_StartMusic(void);
extern void Mg_RenderSubSpriteMtx(OamAttr *a, int b, int c, struct Matrix2x2 *d);
extern int Mg_GetTopScreenYOffset(void);
extern int Mg_CheckBannerButton(int arg);
extern void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
}
extern void func_02012dd0(void *c);
extern void Mg_SetNextBanner(int);

extern void (dMgState_c::*__ptmf_null)(); /* the null member pointer */
extern dFdBrightness_c *gSceneFader;
extern int sMgTopScreenYOffset;
extern int sMgBannerButtonsArmed;
extern int gMgCoinsShowDelta;
extern int sMgCoinsDeltaShown;

int sMgScoreX = 0x80;
int sMgRecordBannerX = 0x80;
int sMgDrawBannerY = 0x58;
int sMgScoreDigitsX = 0x80;
int sMgWinBannerY = 0x60;
int sMgResultsBannerY = -0x60;
int sMgDrawBannerX = 0x80;
int sMgQuitX = 0x80;
int sMgScoreDigitsY = 0x80;
int sMgRetryBannerY = 0x28;
int sMgLoseBannerX = 0x80;
int sMgRecordBannerY = 0x60;
int sMgClearBannerX = 0x80;
int sMgClearBannerY = 0x60;
int sMgResultsBannerX = 0x80;
int sMgWinBannerX = 0x80;
int sMgLoseBannerY = 0x60;
int sMgRetryBannerX = 0x80;
int sMgPlayAgainY = 0x6c;
int sMgScoreY = 0x60;
int sMgNextBanner = 0x1d;
int sMgQuitY = 0x9c;
int sMgPlayAgainX = 0x80;
int sMgTopBannerY = 0x60;
int sMgTopBannerX = 0x80;
int sMgMainBannerY = 0x40;
int sMgMainBannerX = 0x80;
extern OamAttr *sMgTextSpritesEnglish[];
extern OamAttr *sMgTextSpritesSpanish[];
extern OamAttr *sMgTextSpritesFrench[];
extern OamAttr *sMgTextSpritesGerman[];
extern OamAttr *sMgTextSpritesItalian[];
OamAttr **sMgStateTextSpritesByLanguage[] = {
    sMgTextSpritesEnglish, sMgTextSpritesSpanish, sMgTextSpritesFrench,
    sMgTextSpritesGerman,  sMgTextSpritesItalian,
};

void Mg_ResetBannerLayout(void) {
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

dMgState_c::dMgState_c() : mState(-1), mTimer(0), unk_020(0), mSuspended(0) {}

void dMgState_c::SetState(s32 state) {
    static Callback sEnterTable[20] = {
        &dMgState_c::RoundClear_Enter,       /*  0 */
        &dMgState_c::NextRound_Enter,        /*  1 */
        &dMgState_c::RoundBanner5_Enter,     /*  2 */
        &dMgState_c::RetryPrompt_Enter,      /*  3 */
        &dMgState_c::Win_Enter,              /*  4 */
        &dMgState_c::Lose_Enter,             /*  5 */
        &dMgState_c::LoseResume_Enter,       /*  6 */
        &dMgState_c::Draw_Enter,             /*  7 */
        &dMgState_c::Tie_Enter,              /*  8 */
        &dMgState_c::RoundClearTop_Enter,    /*  9 */
        &dMgState_c::RetryPromptB_Enter,     /* 10 */
        &dMgState_c::ScoreThenRound_Enter,   /* 11 */
        &dMgState_c::RetryPromptWide_Enter,  /* 12 */
        &dMgState_c::RetryPromptWideB_Enter, /* 13 */
        &dMgState_c::DigitsThenRound_Enter,  /* 14 */
        &dMgState_c::BannerEThenRound_Enter, /* 15 */
        &dMgState_c::Results_Enter,          /* 16 */
        &dMgState_c::ResultsNewRecord_Enter, /* 17 */
        &dMgState_c::GameOver_Enter,         /* 18 */
        &dMgState_c::Finished_Enter,         /* 19 */
    };

    mState = state;
    mEnter = sEnterTable[mState];
    mRender = 0;
    if (mEnter)
        (this->*mEnter)();
}

void dMgState_c::Behavior() {
    if (mState == -1)
        return;
    ApproachLinear(mTimer, 0, 1);
    if (mBehavior == 0)
        return;
    (this->*mBehavior)();
}

void dMgState_c::Render() {
    if (mState == -1)
        return;
    if (mRender == 0)
        return;
    (this->*mRender)();
}

void dMgState_c::RoundClear_Enter() {
    int prevId, id;
    mTimer = 180;
    prevId = sMgNextBanner;
    id = 3;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgClearBannerX, sMgClearBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)4);
    mBehavior = &dMgState_c::RoundClear_Update;
}

void dMgState_c::RoundClear_Update() {
    dScMgBase_c *scene = gMgScene;
    if (gSceneFader->IsAtEnd()) {
        gSceneFader->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        scene->CallStartRound(mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (gSceneFader->IsBetweenStartAndEnd())
            return;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        gSceneFader->SetForwardTime(0x1e, 0);
    }
}

void dMgState_c::NextRound_Enter() {
    mTimer = 15;
    mBehavior = &dMgState_c::NextRound_Update;
}

void dMgState_c::NextRound_Update() {
    if (gSceneFader->IsAtEnd() != 0) {
        dScMgBase_c *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        gSceneFader->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        scene->CallStartRound(mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (gSceneFader->IsBetweenStartAndEnd() != 0)
            return;
        gSceneFader->SetForwardTime(0x1e, 0);
    }
}

void dMgState_c::RoundBanner5_Enter() {
    mTimer = 180;
    Mg_ShowBanner(5, sMgRecordBannerX, sMgRecordBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)1);
    mBehavior = &dMgState_c::RoundBanner5_Update;
}

void dMgState_c::RoundBanner5_Update() {
    if (gSceneFader->IsAtEnd()) {
        dScMgBase_c *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        gSceneFader->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        scene->CallStartRound(mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (gSceneFader->IsBetweenStartAndEnd() == 0)
            gSceneFader->SetForwardTime(0x1e, 0);
    }
}

void dMgState_c::RetryPrompt_Enter() {
    int prevId, id;
    mTimer = 60;
    prevId = sMgNextBanner;
    id = 8;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = &dMgState_c::RetryPrompt_Update;
}

void dMgState_c ::RetryPrompt_Update() {
    dFdBrightness_c *fader;
    if (Mg_CheckBannerButton(0)) {
        fader = gSceneFader;
        fader->SetForwardTime(0x1e, 0);
        mBehavior = &dMgState_c::UpdateRetryFadeOut;
        return;
    }
    if (Mg_CheckBannerButton(2) || Mg_CheckBannerButton(1)) {
        fader = gSceneFader;
        if (fader->IsBetweenStartAndEnd())
            return;
        dScene_c::SetFaders(&gSceneWipe);
        gSceneWipe.SetType(2);
        dScene_c::StartSceneFade(5, 0, 0);
        func_02012dd0((void *)0x3c);
        return;
    }
    if (mTimer != 0)
        return;
    Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
    Mg_ShowBanner(2, sMgQuitX, sMgQuitY, -1, -1, 0xd);
}

void dMgState_c::UpdateRetryFadeOut() {
    if (gSceneFader->IsAtEnd() == 0)
        return;
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    gSceneFader->SetBackwardTime(0x1e, 0);
    FreeGfxSlotsById(0x1d);
    Mg_RestoreMusic();
    Mg_StartMusic();
    scene->CallStartRound(mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

void dMgState_c::Win_Enter() {
    int prevId, id;
    mTimer = 120;
    prevId = sMgNextBanner;
    id = 9;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgWinBannerX, sMgWinBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x136);
    mBehavior = &dMgState_c::Win_Update;
}

void dMgState_c::Win_Update() {
    if (mTimer > 100)
        return;
    if (mTimer != 0)
        return;
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->CanEndState(mState) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    Mg_RestoreMusic();
    Mg_StartMusic();
    scene->CallStartRound(mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

void dMgState_c::Lose_Enter() {
    mTimer = 120;
    Mg_ShowBanner(0xa, sMgLoseBannerX, sMgLoseBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x137);
    mBehavior = &dMgState_c::Lose_Update;
}

void dMgState_c::Lose_Update() {
    int state = mTimer;
    if (state > 0x64)
        return;
    dScMgBase_c *p = gMgScene;
    int a8 = (p != 0) ? p->mCredits : 0;
    if (a8 == 0) {
        if (state != 0)
            return;
        dScMgBase_c *q = gMgScene;
        if (q == 0)
            return;
        if (q->CanEndState(mState) == 0)
            return;
        gMgCoinsShowDelta = 0;
        sMgCoinsDeltaShown = 0;
        Mg_RestoreMusic();
        FreeGfxSlotsById(0x1d);
        mTimer = 180;
        mBehavior = &dMgState_c::UpdateGameOverToMenu;
    } else {
        if (state != 0)
            return;
        dScMgBase_c *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        scene->CallStartRound(mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    }
}

void dMgState_c::UpdateGameOverToMenu() {
    dFdBrightness_c *fader;
    int v = mTimer;
    if (v == 0xb3) {
        Mg_ShowBanner(8, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
        Mg_PlayJingle((void *)3);
        return;
    }
    if (v != 0)
        return;
    fader = gSceneFader;
    if (fader->IsBetweenStartAndEnd() != 0)
        return;
    dScene_c::SetFaders(&gSceneWipe);
    gSceneWipe.SetType(2);
    dScene_c::StartSceneFade(5, 0, 0);
    func_02012dd0((void *)0x3c);
}

void dMgState_c::LoseResume_Enter() {
    mTimer = 120;
    Mg_ShowBanner(0xa, sMgLoseBannerX, sMgLoseBannerY, -1, -1, 0xd);
    Sound::PlayBank2_2D(0x137);
    mBehavior = &dMgState_c::LoseResume_Update;
}

void dMgState_c::LoseResume_Update() {
    int v = mTimer;
    if (v > 0x64)
        return;
    if (v != 0)
        return;
    FreeGfxSlotsById(0x1d);
    mBehavior = &dMgState_c::UpdateAfterHidden;
    mSuspended = 1;
    gMgCoinsShowDelta = 0;
    sMgCoinsDeltaShown = 0;
}

void dMgState_c::UpdateAfterHidden() {
    dScMgBase_c *g = gMgScene;
    int cond = g ? g->mCredits : 0;
    if (cond == 0) {
        g = gMgScene;
        if (!g)
            return;
        if (!g->CanEndState(mState))
            return;
        Mg_RestoreMusic();
        mTimer = 180;
        mBehavior = &dMgState_c::UpdateGameOverToMenu;
        return;
    }
    g = gMgScene;
    if (!g)
        return;
    if (!g->CanEndState(mState))
        return;
    Mg_RestoreMusic();
    Mg_StartMusic();
    g->CallStartRound(mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

void dMgState_c::Draw_Enter() {
    mTimer = 120;
    Mg_ShowBanner(0xb, sMgDrawBannerX, sMgDrawBannerY, -1, -1, 0xd);
    mBehavior = &dMgState_c::UpdateWaitThenResume;
}

void dMgState_c::Tie_Enter() {
    mTimer = 0;
    mBehavior = &dMgState_c::UpdateWaitThenResume;
}

void dMgState_c::UpdateWaitThenResume() {
    dScMgBase_c *scene;
    if (mTimer != 0)
        return;
    scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->CanEndState(mState) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    scene->CallStartRound(mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

void dMgState_c::RoundClearTop_Enter() {
    int id;
    mTimer = 180;
    Mg_PlayJingle((void *)4);
    id = 3;
    if (sMgNextBanner != 0x1d) {
        id = sMgNextBanner;
        sMgNextBanner = 0x1d;
    }
    Mg_ShowBanner(id, sMgTopBannerX, sMgTopBannerY - (sMgTopScreenYOffset + 0xc0), -1, -1, 0xa);
    mBehavior = &dMgState_c::RoundClearTop_Update;
}

void dMgState_c::RoundClearTop_Update() {
    if (mTimer != 0)
        return;
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    if (scene->CanEndState(mState) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    Mg_RestoreMusic();
    Mg_StartMusic();
    scene->CallStartRound(mState);
    mTimer = 0;
    mState = -1;
    unk_020 = 0;
    mSuspended = 0;
}

void dMgState_c::RetryPromptB_Enter() {
    mTimer = 180;
    Mg_PlayJingle((void *)3);
    Mg_ShowBanner(8, sMgRetryBannerX, sMgRetryBannerY, -1, -1, 0xb);
    mBehavior = &dMgState_c::RetryPromptB_Update;
}

void dMgState_c ::RetryPromptB_Update() {
    dFdBrightness_c *fader;
    if (Mg_CheckBannerButton(0)) {
        fader = gSceneFader;
        fader->SetForwardTime(0x1e, 0);
        mBehavior = &dMgState_c::UpdateRetryFadeOut;
        return;
    }
    if (Mg_CheckBannerButton(2) || Mg_CheckBannerButton(1)) {
        fader = gSceneFader;
        if (fader->IsBetweenStartAndEnd())
            return;
        dScene_c::SetFaders(&gSceneWipe);
        gSceneWipe.SetType(2);
        dScene_c::StartSceneFade(5, 0, 0);
        func_02012dd0((void *)0x3c);
        return;
    }
    if (mTimer != 0)
        return;
    Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
    Mg_ShowBanner(1, sMgQuitX, sMgQuitY, -1, -1, 0xd);
}

void dMgState_c::ScoreThenRound_Enter() {
    mTimer = 180;
    Mg_PlayJingle((void *)1);
    mBehavior = &dMgState_c::ScoreThenRound_Update;
    mRender = &dMgState_c::DrawScore;
}

void dMgState_c::ScoreThenRound_Update() {
    if (gSceneFader->IsAtEnd()) {
        dScMgBase_c *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        gSceneFader->SetBackwardTime(0x1e, 0);
        FreeGfxSlotsById(0x1d);
        Mg_RestoreMusic();
        Mg_StartMusic();
        scene->CallStartRound(mState);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (gSceneFader->IsBetweenStartAndEnd() == 0)
            gSceneFader->SetForwardTime(0x1e, 0);
    }
}

void dMgState_c::RetryPromptWide_Enter() {
    RetryPromptWideB_Enter();
}

void dMgState_c::RetryPromptWideB_Enter() {
    mTimer = 10;
    mBehavior = &dMgState_c::UpdateRetryPrompt;
}

void dMgState_c::UpdateRetryPrompt() {
    if (mTimer == 0) {
        if (Mg_CheckBannerButton(0) != 0) {
            gSceneFader->SetForwardTime(0x1e, 0);
            mBehavior = &dMgState_c::UpdateRetryFadeOut;
            return;
        }
        if (Mg_CheckBannerButton(2) == 0) {
            if (Mg_CheckBannerButton(1) == 0)
                return;
        }
        if (gSceneFader->IsBetweenStartAndEnd() != 0)
            return;
        dScene_c::SetFaders(&gSceneWipe);
        gSceneWipe.SetType(2);
        dScene_c::StartSceneFade(5, 0, 0);
        func_02012dd0((void *)0x3c);
        return;
    }
    Mg_ShowBanner(0, sMgPlayAgainX, sMgPlayAgainY, -1, -1, 0xd);
    Mg_ShowBanner(2, sMgQuitX, sMgQuitY, -1, -1, 0xd);
    mTimer = 0;
}

void dMgState_c::DigitsThenRound_Enter() {
    mTimer = 180;
    Mg_ShowBanner(5, sMgRecordBannerX, sMgRecordBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)1);
    mBehavior = &dMgState_c::UpdateFadeThenResume;
    mRender = &dMgState_c::DrawScoreDigits;
}

void dMgState_c::UpdateFadeThenResume() {
    if (gSceneFader->IsAtEnd()) {
        dScMgBase_c *scene = gMgScene;
        if (scene == 0)
            return;
        if (scene->CanEndState(mState) == 0)
            return;
        gSceneFader->SetBackwardTime(0x1e, 0);
        Mg_RestoreMusic();
        Mg_StartMusic();
        scene->CallStartRound(mState);
        FreeGfxSlotsById(0x1d);
        mTimer = 0;
        mState = -1;
        unk_020 = 0;
        mSuspended = 0;
    } else {
        if (mTimer != 0)
            return;
        if (gSceneFader->IsBetweenStartAndEnd() == 0)
            gSceneFader->SetForwardTime(0x1e, 0);
    }
}

void dMgState_c::BannerEThenRound_Enter() {
    mTimer = 60;
    Mg_ShowBanner(0xe, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = &dMgState_c::UpdateFadeThenResume;
}

void dMgState_c::Results_Enter() {
    if (Mg_IsNewRecord()) {
        ResultsNewRecord_Enter();
        return;
    }
    Mg_ShowBanner(7, 0x80, 0x14, -1, -1, 0xd);
    mTimer = 120;
    mBehavior = &dMgState_c::Results_Update;
    mRender = &dMgState_c::DrawRecordTable;
    Mg_CommitRecords();
}

void dMgState_c::Results_Update() {
    int ok;
    if (mTimer != 0)
        return;
    ok = 0;
    if (gTouchInfo[gActivePlayerSlot].held != 0) {
        if (gTouchInfo[gActivePlayerSlot].edge != 0)
            ok = 1;
    }
    if (ok == 0)
        return;
    func_02012790(0x62);
    FreeGfxSlotsById(7);
    mRender = __ptmf_null;
    RetryPromptWide_Enter();
}

void dMgState_c::ResultsNewRecord_Enter() {
    Mg_ShowBanner(7, 0x80, 0x14, -1, -1, 0xd);
    mTimer = 300;
    mBehavior = &dMgState_c::ResultsNewRecord_Update;
    mRender = &dMgState_c::DrawRecordTable;
    Mg_CommitRecords();
}

void dMgState_c::ResultsNewRecord_Update() {
    int idx;
    int flag;
    dScMgBase_c *scene;
    int v;
    int s;
    int id;

    if (mTimer == 0) {
        idx = gActivePlayerSlot;
        flag = 0;
        if (gTouchInfo[idx].held != 0) {
            if (gTouchInfo[idx].edge != 0)
                flag = 1;
        }
        if (flag != 0) {
            FreeGfxSlotsById(7);
            func_02012790(0x62);
            mRender = __ptmf_null;
            RetryPromptWide_Enter();
        }
    }

    if (mTimer != 180)
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
    Mg_PlayJingle((void *)1);
}

void dMgState_c::GameOver_Enter() {
    mTimer = 180;
    Mg_ShowBanner(8, sMgMainBannerX, sMgMainBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)3);
    mBehavior = &dMgState_c::GameOver_Update;
}

void dMgState_c::GameOver_Update() {
    int x = mTimer;
    dScMgBase_c *g = gMgScene;
    if (x != 0) {
        int ok = 0;
        if (gTouchInfo[gActivePlayerSlot].held) {
            if (gTouchInfo[gActivePlayerSlot].edge)
                ok = 1;
        }
        if (ok == 0)
            return;
    }
    if (g->CanEndState(mState) == 0)
        return;
    mRender = __ptmf_null;
    FreeGfxSlotsById(0x1d);
    if (Mg_IsNewRecord() != 0) {
        ResultsNewRecord_Enter();
        return;
    }
    Results_Enter();
}

void dMgState_c::Finished_Enter() {
    int prevId, id;
    mTimer = 180;
    prevId = sMgNextBanner;
    id = 3;
    if (prevId != 0x1d) {
        sMgNextBanner = 0x1d;
        id = prevId;
    }
    Mg_ShowBanner(id, sMgClearBannerX, sMgClearBannerY, -1, -1, 0xd);
    Mg_PlayJingle((void *)4);
    mBehavior = &dMgState_c::Finished_Update;
}

void dMgState_c::Finished_Update() {
    int x = mTimer;
    dScMgBase_c *g = gMgScene;
    if (x != 0) {
        int ok = 0;
        if (gTouchInfo[gActivePlayerSlot].held) {
            if (gTouchInfo[gActivePlayerSlot].edge)
                ok = 1;
        }
        if (ok == 0)
            return;
    }
    if (g->CanEndState(mState) == 0)
        return;
    mRender = __ptmf_null;
    FreeGfxSlotsById(0x1d);
    if (Mg_IsNewRecord() != 0) {
        ResultsNewRecord_Enter();
        return;
    }
    Results_Enter();
}

void dMgState_c::DrawScore() {
    int r;
    if (gMgScene == 0)
        return;
    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreX, sMgScoreY, r, -1, -1, 0, 0);
}

void dMgState_c::DrawScoreDigits() {
    unsigned int r;
    int idx;

    if (gMgScene == 0)
        return;

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 4, sMgScoreDigitsY, r / 100, -1, -1, 1, 0);

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 0x1c, sMgScoreDigitsY, r % 100 / 10, -1, -1, 1, 0);

    r = Mg_GetSubmittedScore();
    Mg_DrawBigNumber(sMgScoreDigitsX + 0x2c, sMgScoreDigitsY, r % 10, -1, -1, 1, 0);

    idx = GetGameLanguage();
    Mg_RenderSubSpriteMtx(sMgStateTextSpritesByLanguage[idx][3], sMgScoreDigitsX - 0x24,
                          sMgScoreDigitsY, 0);

    idx = GetGameLanguage();
    Mg_RenderSubSpriteMtx(sMgStateTextSpritesByLanguage[idx][7], sMgScoreDigitsX + 0x10,
                          sMgScoreDigitsY, 0);
}

void dMgState_c::DrawRecordTable() {
    dScMgBase_c *scene = gMgScene;
    if (scene == 0)
        return;
    scene->DrawRecordTable();
    if (mTimer > 0)
        return;
    scene->DrawBlinkSprite(0xe0, 0xa0);
}

void Mg_PlaceBannersOnTopScreen(void) {
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

void Mg_ResetRecordBannerPos(void) {
    sMgResultsBannerX = 0x80;
    sMgResultsBannerY = -0x1c;
}

void Mg_SetPerfectBanner(void) {
    Mg_SetNextBanner(6);
}

void Mg_SetNextBanner(int v) {
    sMgNextBanner = v;
}
