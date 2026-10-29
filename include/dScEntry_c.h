#ifndef DSCENTRY_C_H
#define DSCENTRY_C_H

#include "OamAnimation.h"
#include "dScene_c.h"
#include "dGraph_c.h"
#include "dThIcon_c.h"

struct dEntObj_c;

/* Course-entry / results scene. ov075 hosts ENTRY(6) and RESULT(7); both
 * spawn dScEntry_c. Nested icon_c (dThIcon_c leaf) and graphCallback_c
 * (dGraph_c::callback_c leaf). Nine course/character icons on the sub
 * screen; GraphCallback2 decompresses BG2 then steps every live icon
 * through Render.
 */
struct dScEntry_c : dScene_c {
    struct icon_c : dThIcon_c {
        icon_c();
        ~icon_c();
        virtual void Behavior();
        virtual void Render();
    };

    struct graphCallback_c;

    typedef void (dScEntry_c::*Step)();
    struct StepSet {
        Step enter;
        Step behavior;
        Step render;
    };

    dEntObj_c *mEntObj;             /* 0x050 -- multiplayer player figures */
    Step mEnterStep;                /* 0x054 -- run as soon as a step set is installed */
    Step mBehaviorStep;             /* 0x05c -- Behavior runs it every frame */
    Step mRenderStep;               /* 0x064 -- Render runs it every frame */
    const StepSet *mPendingSteps;   /* 0x06c -- installed after the delay or fade */
    icon_c mIcons[9];               /* 0x070..0x1b4 */
    OamAnimation mOamAnimations[4]; /* 0x1b4..0x264 */
    // Inferred view of workspace shared with selection, blinking and results
    // steps. Waiting text also uses the wave fields outside player sync.
    struct WaitingWork {
        s32 playerWaitTicks;
        s32 wavePhase;
        s32 waveDelay;
        s32 syncTicks;
    };
    s32 mStepDelay; /* 0x264 -- countdown before the current step advances */
    union {
        s32 mStepWork[4]; /* 0x268 -- reused by each step */
        WaitingWork mWaiting;
        s32 mSawLoadSignal;     /* loading: observed the asserted peer signal */
        s32 mPlayerSelectReady; /* selection intro: figures reached their slots */
    };
    s32 mSoundHandle;         /* 0x278 */
    u8 mResultLeaderFlags[4]; /* 0x27c -- active players tied for the nonzero top score */
    u8 mIconCount;            /* 0x280 -- live entries of mIcons */
    u8 mSelectedIcon;         /* 0x281 */
    u8 mStepPhase;            /* 0x282 -- substate within the current step */
    u8 mFadeBrightness;     /* 0x283 -- sub-screen blend brightness, restored after a step change */
    u8 mFadeTick;           /* 0x284 -- steps the fade every other frame */
    u8 mIsFading;           /* 0x285 -- while set, Behavior runs the fade and nothing else */
    u8 pad_286;             /* 0x286 */
    u8 mDisconnectHideMask; /* 0x287 -- main-screen layers hidden on disconnect */

    // Names inferred from the step table and multiplayer/UI consumers.
    void TallyRoundWinners();
    s32 InitResultResources();
    s32 InitEntryResources();
    void LeaveSettings();
    int func_ov075_02116ce4();
    void ResetMatchRecords();
    void UpdateSettingsSync();
    void EnterSettingsSync();
    void RenderSoundSettings();
    void UpdateSoundSettings();
    void EnterSoundSettings();
    void RenderControlSettings();
    void UpdateControlSettings();
    void EnterControlSettings();
    void EnterQuitMessage();
    void UpdateQuit();
    void EnterQuit();
    void UpdateQuitConfirm();
    void EnterQuitConfirm();
    void UpdateRematchSync();
    void EnterRematchSync();
    void RenderWinCounts();
    void UpdateRematchPrompt();
    void EnterRematchPrompt();
    void RenderResults();
    void UpdateResults();
    void EnterResults();
    void func_ov075_02118a78();
    void SetSteps(const StepSet *steps);
    void ChangeStepsAfterDelay(const StepSet *steps, s32 delay);
    void ChangeStepsWithFade(const StepSet *steps);
    void UpdateStepDelay();
    void EnterStepFadeOut();
    void UpdateStepFadeOut();
    // Name and const qualifier are inferred; callers supply a step-set address.
    void HandlePlayerDisconnect(const StepSet *resumeSteps);
    enum MatchLoadPhase {
        MATCH_LOAD_RESOURCES,
        MATCH_LOAD_WAIT_FOR_SIGNAL_CLEAR,
        MATCH_LOAD_READY
    };
    void EnterMatchLoading();
    void UpdateMatchLoading();
    static void LoadCourseResources(int course);
    void StartMatch();
    int CheckMenuConfirm(int player);
    int ArePlayersReady();
    void EnterCourseSelect();
    void UpdateCourseSelect();
    void RenderCourseSelectTitle();
    void EnterPlayerSync();
    void UpdatePlayerSync();
    void RenderWaitingText();
    void EnterPlayerSelectIntro();
    void UpdatePlayerSelectIntro();

    /* Out of line: the key function, defined first in src/game/stages/d_s_entry.cpp. */
    virtual ~dScEntry_c(); /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();        /* slot  0 */
    virtual bool BeforeInitResources(); /* slot  1 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */
};

// Inferred step-set names; storage remains in the overlay's existing BSS.
extern dScEntry_c::StepSet EntryPlayerSelectIntroSteps;
extern dScEntry_c::StepSet EntryCourseSelectSteps;
extern dScEntry_c::StepSet EntryMatchLoadingSteps;
extern dScEntry_c::StepSet EntryDelaySteps;
extern dScEntry_c::StepSet EntryFadeOutSteps;

struct dScEntry_c::graphCallback_c : dGraph_c::callback_c {
    void *compressedBg2Screen; /* 0x04 */
    void *entryScene;          /* 0x08 */
    s32 bg2ScreenBase;         /* 0x0c - BG2 screen block, written to bits 8..12 */
    u8 unk_010[0x1c];          /* 0x10 */

    graphCallback_c();
    virtual int GraphCallback2();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScEntry_c_size_must_be_0x288[sizeof(dScEntry_c) == 0x288 ? 1 : -1];
typedef char dScEntry_c_Step_size_must_be_0x8[sizeof(dScEntry_c::Step) == 0x8 ? 1 : -1];
typedef char dScEntry_c_StepSet_size_must_be_0x18[sizeof(dScEntry_c::StepSet) == 0x18 ? 1 : -1];
typedef char icon_c_size_must_be_0x24[sizeof(dScEntry_c::icon_c) == 0x24 ? 1 : -1];
typedef char dScEntry_oam_animation_size_must_be_0x2c[sizeof(OamAnimation) == 0x2c ? 1 : -1];
typedef char dScEntry_graphCallback_c_size_must_be_0x2c[sizeof(dScEntry_c::graphCallback_c) == 0x2c
                                                            ? 1
                                                            : -1];
#endif

#endif
