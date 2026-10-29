#ifndef DSCMGPACHINKO2_C_H
#define DSCMGPACHINKO2_C_H
#include "dScMgBase_c.h"

/* The Tamaire minigame scene: scene 0x171, profile MG_TAMAIRE. RTTI derives
 * dScMgPachinko2_c directly from dScMgBase_c, and no RTTI record names it as
 * a base. It overrides vtable slots 0 (InitResources), 6 (Behavior),
 * 9 (Render), 16 and 17 (the destructors) and 18 (StartRound). Offsets
 * below 0x4660 belong to dScMgBase_c. Own offsets that only the unnamed
 * helper functions touch are still padding.
 *
 * The factory dScMgPachinko2_c_classInit (historical alias
 * MgLakituLaunch_Spawn) installs this class's vtable. The ROM proves the
 * class; the classInit spelling follows later EAD lineage.
 */
/* One launched ball. Ball_StateAim drives it from the touch record while
   the pen is down (x/y follow the pen, px/py keep the pen offset) and, on
   release, aims it at (0x80, 0x20): angle = atan2, speed from the distance,
   vx/vy from the sine table. */
struct dScMgPachinko2_Ball {
    s32 x;          /* +0x00 -- Fix12 */
    s32 y;          /* +0x04 */
    s32 vx;         /* +0x08 */
    s32 vy;         /* +0x0c */
    s32 px;         /* +0x10 -- pen offset while held */
    s32 py;         /* +0x14 */
    s32 prevX;      /* +0x18 -- DrawAimLine redraws the guide only when x or */
    s32 prevY;      /* +0x1c    y differs from these, then stores x and y here;
                                ResetRound zeroes them */
    s32 speed;      /* +0x20 */
    s32 gravity;    /* +0x24 -- Ball_StateFlying and _StateScored grow it by
                       0x10 a frame and _StateTumble by 0x20, then add it to
                       vy (or take it off speed on the arc); Ball_HitAt,
                       UpdateNextBall and ResetRound zero it */
    s32 sound;      /* +0x28 -- Sound_PlayIfNotActive handle */
    s32 prevDist;   /* +0x2c */
    u16 rotation;   /* +0x30 -- Ball_DrawAll hands it to Mg_RenderSpriteBothScaled
                       as the angle; _StateFlying and _StateTumble add spin,
                       _StateHop and UpdateNextBall zero it */
    s16 spin;       /* +0x32 -- rotation step; vx / 4 at the throw */
    u16 angle;      /* +0x34 -- heading: Ball_HitAt stores 0xd000 or 0xb000,
                       Ball_StateAim an atan2, and every reader takes it
                       unsigned as the sine-table index (>> 4). Was s16. */
    s16 timer;      /* +0x36 -- anim tick in state 5, countdown from 0x40 in state 7 */
    u8 active;      /* +0x38 -- Ball_UpdateAll runs the ball while set */
    u8 state;       /* +0x39 -- 2 while released, 0 when reset */
    u8 visible;     /* +0x3a -- Ball_DrawAll draws the ball while set */
    u8 spawnClear;  /* +0x3b -- zeroed when a ball spawns; nothing reads it */
    u8 sprite;      /* +0x3c -- 0 = rolling ball, else frame + 1 in sPachinko2BallCountSprites */
    u8 animStep;    /* +0x3d -- 0..3, picks the frame */
    u8 facingRight; /* +0x3e -- moving right; frames +4 */
    u8 pad_3f;      /* +0x3f */
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    dScMgPachinko2_Ball_size_must_be_0x40[sizeof(struct dScMgPachinko2_Ball) == 0x40 ? 1 : -1];
#endif

/* One of the three paddles. State 0..14 dispatches through
   sPachinko2PaddleStateTable; Paddle_Hit parks the current motion in
   home/saved* and later states restore it. */
struct dScMgPachinko2_Paddle {
    s32 x;          /* +0x00 -- Fix12 */
    s32 y;          /* +0x04 */
    s32 vx;         /* +0x08 */
    s32 vy;         /* +0x0c */
    s32 speed;      /* +0x10 */
    u8 unk_14[0x8]; /* +0x14 -- Paddle_SpawnForLanes zeroes both words; nothing reads them */
    s32 homeX;      /* +0x1c */
    s32 homeY;      /* +0x20 */
    s32 savedVx;    /* +0x24 */
    s32 savedVy;    /* +0x28 */
    s32 sound;      /* +0x2c -- Sound_PlayIfNotActive handle */
    u16 angle;      /* +0x30 -- Paddle_StateOrbit steps it by 0x40 and reads
                       the sine table at angle >> 4; _StateDescend stores
                       0x8000 or 0; _StateDropBalls an atan2. Was s16. */
    u16 timer;      /* +0x32 */
    u8 active;      /* +0x34 */
    u8 visible;     /* +0x35 */
    u8 state;       /* +0x36 */
    u8 savedState;  /* +0x37 */
    u8 dir;         /* +0x38 */
    u8 ballsHeld;   /* +0x39 -- balls caught, dropped back one at a time */
    u8 swing;       /* +0x3a */
    u8 step;        /* +0x3b -- sub-step inside the current state */
    u8 hits;        /* +0x3c -- four parks the paddle */
    u8 flip;        /* +0x3d */
    u8 pad_3e[0x2]; /* +0x3e */
};

/* The flash at a paddle catch: four frames from sPachinko2SparkSprites.
   Spark_UpdateAll holds the first until timer reaches eight, then advances
   one frame each update. */
struct dScMgPachinko2_HitFx {
    s32 x; /* +0x00 */
    s32 y; /* +0x04 */
    s32 unk_08; /* Spark_Spawn zeroes this and unk_0c; nothing reads them */
    s32 unk_0c;
    u16 timer; /* +0x10 */
    u8 pad_12[0x2];
    u8 visible; /* +0x14 */
    u8 frame;   /* +0x15 */
    u8 active;  /* +0x16 */
    u8 pad_17;
};

/* The popup a scoring ball throws up: moves while life counts from 0x40
   through 0x30, then holds its position until expiry (Popup_UpdateAll). */
struct dScMgPachinko2_ScorePop {
    s32 x;  /* +0x00 */
    s32 y;  /* +0x04 */
    s32 vy; /* +0x08 */
    s32 unk_0c; /* Popup_Spawn zeroes it; nothing reads it */
    u16 life; /* +0x10 */
    u8 pad_12[0x2];
    u8 active;  /* +0x14 */
    u8 visible; /* +0x15 */
    u8 score;   /* +0x16 -- the number Popup_DrawAll draws through
                   Mg_DrawBigNumber: Ball_StateScored passes the submitted
                   score plus one to Popup_Spawn. Was `sprite`. */
    u8 settled; /* +0x17 -- Popup_UpdateAll moves the popup only while it is
                   zero and sets it when life reaches 0x30; Popup_Spawn
                   clears it. Was `falling`, the opposite of what it means. */
};

/* One of the two cups. State 0..4 dispatches through sPachinko2CupStateTable. */
struct dScMgPachinko2_Cup {
    s32 x;  /* +0x00 -- Fix12 */
    s32 y;  /* +0x04 */
    s32 vx; /* +0x08 */
    s32 unk_0c; /* no function in the TU touches it */
    u16 timer; /* +0x10 */
    u8 pad_12[0x2];
    u8 active;   /* +0x14 */
    u8 state;    /* +0x15 */
    u8 phase;    /* +0x16 */
    u8 frame;    /* +0x17 -- sprite index into sPachinkoPipeSprites */
    u8 animStep; /* +0x18 */
    u8 visible;  /* +0x19 */
    u8 dir;      /* +0x1a */
    u8 armed;    /* +0x1b */
    u8 hops;     /* +0x1c */
    u8 lastMove; /* +0x1d -- 0xff before the first */
    u8 repeats;  /* +0x1e */
    u8 pad_1f;
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    dScMgPachinko2_Paddle_size_must_be_0x40[sizeof(struct dScMgPachinko2_Paddle) == 0x40 ? 1 : -1];
typedef char
    dScMgPachinko2_HitFx_size_must_be_0x18[sizeof(struct dScMgPachinko2_HitFx) == 0x18 ? 1 : -1];
typedef char
    dScMgPachinko2_ScorePop_size_must_be_0x18[sizeof(struct dScMgPachinko2_ScorePop) == 0x18 ? 1
                                                                                             : -1];
typedef char
    dScMgPachinko2_Cup_size_must_be_0x20[sizeof(struct dScMgPachinko2_Cup) == 0x20 ? 1 : -1];
#endif

struct dScMgPachinko2_c : dScMgBase_c {
    virtual ~dScMgPachinko2_c();      /* slots 16 and 17 */
    virtual s32 InitResources();      /* slot 0 */
    virtual s32 Behavior();           /* slot 6 */
    virtual s32 Render();             /* slot 9 */
    virtual void StartRound(int arg); /* slot 18 */

    /* cups: the two 0x20-byte drop targets at 0x5620 */
    void CupSlot_DrawAll();
    /* render both cups */
    void CupSlot_ResetAll();
    /* reset cup slots */
    void Cup_PickTarget(int k);
    /* launched-ball vs cup k check */
    void Cup_StateDecide(int k);
    /* cup state 4: randomize */
    void Cup_StateHopRight(int k);
    /* cup state 3 */
    void Cup_StateHopLeft(int k);
    /* cup state 2 */
    void Cup_StateWait(int k);
    /* cup state 1 */
    void Cup_StateInit(int k);
    /* cup state 0: idle reset */
    void Cup_DrawAll();
    /* cup PMF dispatch, sPachinko2CupStateTable */
    void Cup_ShowAll();
    /* init both cups */
    void Cup_HideAll();
    /* clear cup flags */

    /* scene state */
    void ChooseLaneConfig();                       /* pick lane config -> mLaneConfig */
    void DrawScore();                              /* round-end jingle gate */
    void DrawTopRecord();                          /* round-end sfx gate */
    void StartRoundEnd(int a);                     /* enter state 3, fade */
    void CheckRoundOver();                         /* all-paddles-done check -> state 3 */
    void Paddle_SpawnForLanes();                   /* arm paddles for the round */
    void CheckCountdownEnd();                      /* enter state 2 */
    void DrawCountdown();                          /* countdown tick gate */
    void UpdateCountdown();                        /* countdown ticker */
    void ClearAimLine();                           /* cancel launch if ball left pen */
    void PlotSubBg2Pixels(int a1, int a2, int a3); /* BG2 tile stamp (a0 unused) */
    void DrawAimLine();                            /* draw the guide lines */
    void StartCountdown();                         /* arm 30-tick countdown */
    void UpdateNextBall();                         /* launch queued ball */
    void ResetRound();                             /* reset the whole round */

    /* sprite bursts: the two 16 x 0x18 records at 0x5320 and 0x54a0 */
    void Popup_DrawAll();                     /* render records at 0x54a0 */
    void Popup_UpdateAll();                   /* step records at 0x54a0 */
    void Popup_Spawn(int r1, int r2, int r3); /* spawn at 0x54a0 */
    void Spark_DrawAll();                     /* render records at 0x5320 */
    void Spark_UpdateAll();                   /* step records at 0x5320 */
    void Spark_Spawn(int a2, int a3);         /* spawn at 0x5320 */

    /* paddles: the three 0x40-byte records at 0x5260 */
    void Paddle_DrawAll();                /* render paddles */
    void Paddle_HideAll();                /* reset paddle timers */
    void Paddle_Stop(int idx);            /* park paddle idx */
    void Paddle_CheckBallHits(int idx);   /* paddle idx vs balls */
    void Paddle_UpdateAll();              /* paddle PMF dispatch, sPachinko2PaddleStateTable */
    void Paddle_ClearAll();               /* clear paddle flags */
    void Paddle_StateFlyAway(int i);      /* paddle state 14 */
    void Paddle_StateDropBalls(int i);    /* paddle state 13 */
    void Paddle_StateGather(int i);       /* paddle state 12: seek lane target */
    void Paddle_StateRiseHome(int i);     /* paddle state 11 */
    void Paddle_StateSwing(int i);        /* paddle state 10 */
    void Paddle_StateFallToGround(int i); /* paddle state 9 */
    void Paddle_StateKnockedDown(int i);  /* paddle state 8 */
    void Paddle_Hit(int i);               /* collect -> next state */
    void Paddle_StatePause(int i);        /* paddle state 7 */
    void Paddle_StateCross(int i);        /* paddle state 6: horizontal patrol */
    void Paddle_StateWeave(int i);        /* paddle state 5 */
    void Paddle_StateOrbit(int i);        /* paddle state 4: circle lane */
    void Paddle_StateBoxLoop(int i);      /* paddle state 3 */
    void Paddle_StateSweep(int i);        /* paddle state 2 */
    void Paddle_StateDescend(int i);      /* paddle state 1: rise then split */
    void Paddle_StateSpawn(int i);        /* paddle state 0: enter */

    /* balls: mBalls[0x30] active */
    void Ball_HitAt(int x, int z, int d); /* spawn ball */
    void Ball_Bounce(int i);              /* ball i hits the floor */
    void Ball_ClearAll();                 /* clear ball flags */
    void Ball_DrawAll();                  /* render balls */
    void Ball_UpdateAll();                /* ball PMF dispatch, sPachinko2BallStateTable */
    void Ball_StateWaitGrab(int arg1);    /* ball state 0: idle in tray */
    void Ball_StateAim(int i);            /* ball state 1: pen drag */
    void Ball_StateFlying(int i);         /* ball state 2: launched flight */
    void Ball_StateTumble(int i);         /* ball state 3 */
    void Ball_StateHop(int i);            /* ball state 4 */
    void Ball_StateWalkOff(int i);        /* ball state 5 */
    void Ball_StateRemove(int i);         /* ball state 6: despawn */
    void Ball_StateScored(int i);         /* ball state 7 */

    dScMgPachinko2_Ball mBalls[0x30];         /* 0x4660 */
    dScMgPachinko2_Paddle mPaddles[3];        /* 0x5260 */
    dScMgPachinko2_HitFx mHitFx[0x10];        /* 0x5320 */
    dScMgPachinko2_ScorePop mScorePops[0x10]; /* 0x54a0 */
    dScMgPachinko2_Cup mCups[2];              /* 0x5620 */
    s32 mPhase;         /* 0x5660 -- 0 setup, 1 play, 2 time up, 3 results wait */
    s32 mGuideStep;     /* 0x5664 -- DrawAimLine */
    s32 mLaneConfig;    /* 0x5668 -- 0..5, ChooseLaneConfig */
    u16 mNextBallDelay; /* 0x566c -- frames until UpdateNextBall pens a new ball */
    u16 mEndTimer;      /* 0x566e -- 0x40 frames from time up to the results */
    u16 mBallDealt;     /* 0x5670 -- [medium] first ball dealt; later ones sound 0x19d */
    u16 unk_5672;       /* 0x5672 -- ResetRound zeroes it; nothing reads it */
    u16 mCountdown;     /* 0x5674 -- seconds; UpdateCountdown */
    u8 mHeldBall;       /* 0x5676 -- 1 + the ball in the pen, 0 for none */
    u8 mResult;         /* 0x5677 */
    u8 mCountdownArmed; /* 0x5678 */
    u8 mCountdownTick;  /* 0x5679 -- frames into the current second */
    u8 mCountdownShown; /* 0x567a */
    /* the factory allocates 0x567c bytes; see tools/opnew_sizes.py */
    u8 pad_567b;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    dScMgPachinko2_c_size_must_be_0x567c[sizeof(struct dScMgPachinko2_c) == 0x567c ? 1 : -1];
#endif

#endif
