#ifndef DMGSTATE_C_H
#define DMGSTATE_C_H

#include "types.h"

/* The 20 shared states Mg_SetSharedState enters (dMgState_c::SetState).
 * Names describe each enter handler's mechanics, which are certain: which
 * banner and jingle it shows and what runs after it. The on-screen meaning
 * of most is unobserved (notes/minigames/names.md, "the shared banner
 * states"; notes/minigames/runtime-checks.md). Confidence in the meaning
 * word: [high] where the mechanics leave no doubt, [medium] from who
 * enters it, [low] where no caller was found. */
enum MgSharedState {
    MG_STATE_ROUND_CLEAR = 0,          /* [high, seen] banner 3 "CLEAR" (6 "PERFECT"), jingle 4 */
    MG_STATE_NEXT_ROUND = 1,           /* [high, seen] silent fade, then StartRound(1) */
    MG_STATE_ROUND_BANNER5 = 2,        /* [low] banner 5, jingle 1; no caller found */
    MG_STATE_RETRY_PROMPT = 3,         /* [high, seen] PLAY AGAIN / QUIT under the banner */
    MG_STATE_WIN = 4,                  /* [high, seen] banner 9; Memory's cleared board pays */
    MG_STATE_LOSE = 5,                 /* [high, seen] lost bet; game over at 0 credits */
    MG_STATE_LOSE_RESUME = 6,          /* [low] banner 0xa, then hands back to the game */
    MG_STATE_DRAW = 7,                 /* [high, seen] banner 0xb; Card's equal hands, refund */
    MG_STATE_TIE = 8,                  /* [high, seen] silent; Roulette payout == stake */
    MG_STATE_ROUND_CLEAR_TOP = 9,      /* [low] state 0 on the top screen; no caller found */
    MG_STATE_RETRY_PROMPT_B = 10,      /* [low] retry / quit with banner style 0xb */
    MG_STATE_SCORE_THEN_ROUND = 11,    /* [low] draws the score, then StartRound(11) */
    MG_STATE_RETRY_PROMPT_WIDE = 12,   /* [high, seen] PLAY AGAIN / QUIT side by side; Flower */
    MG_STATE_RETRY_PROMPT_WIDE_B = 13, /* [low] as 12 */
    MG_STATE_DIGITS_THEN_ROUND = 14,   /* [low] draws score digits, then StartRound(14) */
    MG_STATE_BANNER_E_THEN_ROUND = 15, /* [low] banner 0xe, jingle 3 */
    MG_STATE_RESULTS = 16,             /* [high, seen] banner 7 "HIGH SCORE", record table */
    MG_STATE_RESULTS_NEW_RECORD = 17,  /* [high, seen] 16 + OnNewRecord, banner 5, jingle 1 */
    MG_STATE_GAME_OVER = 18,           /* [high, seen] banner 8 "GAME OVER", jingle 3 */
    MG_STATE_FINISHED = 19             /* [high, seen] banner 3 (Snowball: 4 "GOAL"), jingle 4 */
};

/* The shared minigame state machine, embedded in dScMgBase_c at 0xcc. It
 * has no vtable and the ROM has no RTTI for it, so the class name and its
 * member names are coined. The matched methods fix the layout: SetState
 * takes mEnter from a 20-entry table and calls it, Behavior counts mTimer
 * toward 0 and calls mBehavior, Render calls mRender, and mState -1 (the
 * constructor's value) makes Behavior and Render do nothing. */
struct dMgState_c {
    typedef void (dMgState_c::*Callback)();

    Callback mEnter;
    Callback mBehavior;
    Callback mRender;
    s32 mState;
    s32 mTimer;
    s32 unk_020;
    s32 mSuspended; /* state 6 sets it, Mg_ResumeSharedState clears it; the base
                       steps and draws the machine only while it is 0 */

    dMgState_c();
    void Render();
    void Behavior();
    void SetState(s32 state);

    /* State handlers: pointer-to-member constants at 0x020bc8bc..0x020bca3c
       (the enter/behavior/render records) name these as targets. */
    void DrawRecordTable();
    void DrawScoreDigits();
    void DrawScore();
    void Finished_Update();
    void Finished_Enter();
    void GameOver_Update();
    void GameOver_Enter();
    void ResultsNewRecord_Update();
    void ResultsNewRecord_Enter();
    void Results_Update();
    void Results_Enter();
    void BannerEThenRound_Enter();
    void UpdateFadeThenResume();
    void DigitsThenRound_Enter();
    void UpdateRetryPrompt();
    void RetryPromptWideB_Enter();
    void RetryPromptWide_Enter();
    void ScoreThenRound_Update();
    void ScoreThenRound_Enter();
    void RetryPromptB_Update();
    void RetryPromptB_Enter();
    void RoundClearTop_Update();
    void RoundClearTop_Enter();
    void UpdateWaitThenResume();
    void Tie_Enter();
    void Draw_Enter();
    void UpdateAfterHidden();
    void LoseResume_Update();
    void LoseResume_Enter();
    void UpdateGameOverToMenu();
    void Lose_Update();
    void Lose_Enter();
    void Win_Update();
    void Win_Enter();
    void UpdateRetryFadeOut();
    void RetryPrompt_Update();
    void RetryPrompt_Enter();
    void RoundBanner5_Update();
    void RoundBanner5_Enter();
    void NextRound_Update();
    void NextRound_Enter();
    void RoundClear_Update();
    void RoundClear_Enter();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgState_c_size_must_be_0x28[
    sizeof(dMgState_c) == 0x28 ? 1 : -1];
#endif

#endif
