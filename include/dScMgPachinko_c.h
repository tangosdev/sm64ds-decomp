#ifndef DSCMGPACHINKO_C_H
#define DSCMGPACHINKO_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper). rtti_vtables.py --own also
   reports slots 36/38, but both are spurious: the true vtable ends at 36
   words (address-gap math against the neighbouring g_profile_MG_TAMAIRE
   table, and relocs.txt cross-references confirm it belongs to a wholly
   different actor's spawn descriptor, not this class) -- excluded, along
   with Paddle_StateOrbit and dScMgPachinko2_c_classInit, from this migration.
   Fields below dScMgBase_c's own 0x4660 are INHERITED, not this class's
   own -- accessed via raw offsets on a char* cast of `this` (0xa4, 0xc0,
   0xc3, 0xc4 all touched here, all already dScMgBase_c's own, same as
   dScMgPachinko2_c's).
 *
 * SM64DS RTTI names the implementation dScMgPachinko_c. The reconstructed factory
 * dScMgPachinko_c_classInit (historical alias MgBobOmbSquad_Spawn) installs this class's
 * cartridge vtable for the MG_PACHINKO registry profile.
 */
/* One pachinko ball on the board, 0x38 bytes, thirty of them at 0x4660. */
struct dScMgPachinko_ball {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 velX;           /* 0x08 Ball_StateDive/_StateSpinAway add it to
                           x; _AimAtTarget rewrites it as speed * cos */
    s32 velY;           /* 0x0c added to y; _StateDive's atan2(velY, velX)
                           gives `angle` */
    s32 offX;           /* 0x10 Ball_DrawAll draws at x + offX;
                           _StateSpinAway sets it to cos(rotation) * spinRadius */
    s32 offY;           /* 0x14 the same for y, with sin */
    s32 spinRadius;     /* 0x18 _StateSpinAway grows it by 0x800 up to 0x30000
                           and scales the offset by it */
    s32 sound;          /* 0x1c, sound handle (_StateSpinAway's
                           Sound_PlayIfNotActive) */
    s32 scale;          /* 0x20 Ball_DrawAll's sprite scale (20.12);
                           seeded 0x1000 */
    u16 rotation;       /* 0x24 the angle Ball_DrawAll draws at:
                           _StateSpinAway spins it 0x800 a frame, _StateDive
                           sets it to angle - 0x4000 and _StateGlide turns it
                           there 0x100 a frame */
    u16 angle;          /* 0x26 */
    u16 timer;          /* 0x28 StateWaiting holds the ball until it runs
                           out; a hit forces state 6 with 16 and StatePop
                           pops the ball when it reaches zero */
    u8  unk2a[0x2];     /* 0x2a */
    u8  active;         /* 0x2c */
    u8 visible;         /* 0x2d Ball_DrawAll draws only balls with
                           it set; StateWaiting sets it, StateSpinAway and
                           the clears drop it with active */
    u8  unk2e;          /* 0x2e */
    u8  state;          /* 0x2f 0 rests; a hit below 5 jumps to 6 */
    u8 started;         /* 0x30 Ball_StateFall/_StateDrift seed the
                           velocity on the frame it is 0 and bump it */
    u8 driftDir;        /* 0x31 _StateDrift's sway: 0 eases velX to a stop,
                           1 speeds it up rightwards, 2 leftwards; a wall
                           bounce swaps 1 and 2 */
    u8 pad32;           /* 0x32 */
    u8 sprite;          /* 0x33 index into sPachinkoBallSprites, which
                           Ball_DrawAll draws */
    u8 target;          /* 0x34 the mTarget index _AimAtTarget homes it on */
    u8 fallPlayed;      /* 0x35 Ball_CheckFall plays sound 0x184 once,
                           on the frame it sets this */
    u8 level;           /* 0x36 mWaveLevel when SpawnWave spawned it;
                           _AimAtTarget passes it to the sound call */
    u8  unk37;          /* 0x37 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_ball_size_must_be_0x38[sizeof(struct dScMgPachinko_ball) == 0x38 ? 1 : -1];
#endif

/* One ball being aimed with the stylus, 0x38 bytes, at 0x4ed8. */
struct dScMgPachinko_shot {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 velX;           /* 0x08 */
    s32 velY;           /* 0x0c */
    s32 offX;           /* 0x10, grab offset from the stylus */
    s32 offY;           /* 0x14 */
    s32 prevX;          /* 0x18 x the aim line was last drawn from */
    s32 prevY;          /* 0x1c */
    s32 speed;          /* 0x20 */
    s32 gravity;        /* 0x24 StateFlying grows it by 0x10 a frame and adds
                           it to velY; StateFallAway adds it to speed; zeroed
                           when UpdateNextBall serves the shot */
    s32 sound;          /* 0x28, sound handle */
    s32 dist;           /* 0x2c, last frame's distance from the launcher */
    u16 angle;          /* 0x30 */
    u16 timer;          /* 0x32 */
    u8  active;         /* 0x34 clear: this shot is gone */
    u8  state;          /* 0x35 at least 2 before it can hit; 2 sits on the launcher */
    u8 visible;         /* 0x36 BobOmb_DrawAll draws only shots with
                           it set; set when served, cleared with active on a
                           hit and by BobOmb_ClearAll */
    u8 combo;           /* 0x37 balls this shot has popped:
                           Ball_CheckHitByBobOmb picks the pop sound
                           and score by it (capped at 5), then bumps it;
                           zeroed on grab and when served */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_shot_size_must_be_0x38[sizeof(struct dScMgPachinko_shot) == 0x38 ? 1 : -1];
#endif

/* A score popup: 30 of them at 0x4cf0 (Popup_SpawnScore). */
struct dScMgPachinko_popup {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u16 points;         /* 0x08 Popup_SpawnScore stores the score here
                           and submits it; Popup_DrawAll draws it */
    u16 timer;          /* 0x0a SpawnScore seeds 0x30, Popup_UpdateAll
                           counts it down, DrawAll draws only while nonzero */
};

/* One of the three swinging rows at 0x4e58 (Deco_UpdateAll swings
   angle between -0x1000 and 0x1000; dir picks the way). */
struct dScMgPachinko_row {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u8  pad08[0x8];
    u16 angle;          /* 0x10 -- swings +-0x1000; compared signed */
    s16 unk12;          /* 0x12 */
    u8  active;         /* 0x14 */
    u8  visible;        /* 0x15 */
    u8  dir;            /* 0x16 */
    u8  pad17;
};

/* The two pipes at 0x4ea0. */
struct dScMgPachinko_pipe {
    s32 x;              /* 0x00 Pipe_DrawAll draws the sprite at
                           (x, y) >> 12; _StateRise and _StateSink slide it
                           by vel between the sPachinkoPipe*Stops columns */
    s32 y;              /* 0x04 Pipe_ResetAll seeds 0x68000 */
    s32 vel;            /* 0x08 */
    s32 velY;           /* 0x0c the y twin of vel, as in the flyer and bg
                           records; pipes only slide in x, so _ResetAll and
                           ResetRound zero it and nothing reads it */
    u16 timer;          /* 0x10 */
    u8  pad12;
    u8 state;           /* 0x13 Pipe_UpdateAll's index into
                           sPachinkoPipeStateTable (0 Pick, 1 Rise, 2 Sink,
                           3 Idle): _StatePick stores the rolled kind, Rise
                           ends in 2, Sink in 1, Idle in 0; _SetAll sets 3 */
    u8 phase;           /* 0x14 Rise/Sink: 0 while sliding, 1 once at the
                           stop for the settle animation; Idle: nonzero holds
                           the pipe there (_SetAll's argument, 1 at time up) */
    u8 sprite;          /* 0x15 index into sPachinkoPipeSprites, which
                           Pipe_DrawAll draws */
    u8  frame;          /* 0x16 */
    u8 active;          /* 0x17 Pipe_UpdateAll and _SetAll skip a pipe
                           without it; _ResetAll sets it */
    u8 visible;         /* 0x18 Pipe_DrawAll draws only with it set;
                           _StatePick sets it, _ResetAll clears it */
    u8 sinking;         /* 0x19 Pipe_StatePick sets it when it picks
                           Sink (2), with vel 0x1000, and clears it
                           otherwise; _ResetAll clears it */
    u8  laps;           /* 0x1a */
    u8  pad1b;
};

/* A hit effect: the burst sprite Fx_Spawn starts at a hit, worth
   `points` once its animation ends. */
struct dScMgPachinko_fx {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    u16 timer;      /* 0x08 -- frames on the current frame; steps at 6 */
    u16 points;     /* 0x0a -- popup score spawned when it ends; 0 for none */
    u8  active;     /* 0x0c */
    u8  frame;      /* 0x0d */
    u8  visible;    /* 0x0e */
    u8  kind;       /* 0x0f -- 2 uses the long (six-frame) animation */
    u8  target;     /* 0x10 */
    u8  pad11[0x3];
};

/* The flyer that crosses the top of the board: Lakitu on his cloud (seen in
   melonDS). Its state handlers dispatch on `state`, guarded by `active`
   (Flyer_StateWaiting..). */
struct dScMgPachinko_flyer {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    s32 vx;         /* 0x08 -- 20.12 */
    s32 vy;         /* 0x0c -- 20.12; StateWaiting seeds 0x2000, StateDescend adds it to y */
    s32 sound;      /* 0x10 -- looping sound handle */
    u16 delay;      /* 0x14 */
    u8  active;     /* 0x16 */
    u8  visible;    /* 0x17 */
    u8  state;      /* 0x18 */
    u8  facing;     /* 0x19 -- flips at each edge; picks the sprite */
    u8  passes;     /* 0x1a -- crossings left */
    u8  pad1b;
};

/* One of the four targets (Target_ShowAll / HideAll): the four
   flowers along the bottom (seen in melonDS; y 0xa0000, x from a table). */
struct dScMgPachinko_target {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    u8  active;     /* 0x08 */
    u8  visible;    /* 0x09 */
    u8  sprite;     /* 0x0a -- index into the target sprite table */
    u8  pad0b;
};

/* The scrolling background (Bg_StateWaiting..), dispatched on `state`. */
struct dScMgPachinko_bg {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    s32 vx;         /* 0x08 */
    s32 vy;         /* 0x0c */
    u16 timer;      /* 0x10 */
    u8  active;     /* 0x12 */
    u8  state;      /* 0x13 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dScMgPachinko_popup_size_must_be_0xc[sizeof(dScMgPachinko_popup) == 0xc ? 1 : -1];
typedef char dScMgPachinko_row_size_must_be_0x18[sizeof(dScMgPachinko_row) == 0x18 ? 1 : -1];
typedef char dScMgPachinko_pipe_size_must_be_0x1c[sizeof(dScMgPachinko_pipe) == 0x1c ? 1 : -1];
typedef char dScMgPachinko_fx_size_must_be_0x14[sizeof(dScMgPachinko_fx) == 0x14 ? 1 : -1];
typedef char dScMgPachinko_flyer_size_must_be_0x1c[sizeof(dScMgPachinko_flyer) == 0x1c ? 1 : -1];
typedef char dScMgPachinko_target_size_must_be_0xc[sizeof(dScMgPachinko_target) == 0xc ? 1 : -1];
typedef char dScMgPachinko_bg_size_must_be_0x14[sizeof(dScMgPachinko_bg) == 0x14 ? 1 : -1];
#endif

struct dScMgPachinko_c : dScMgBase_c {
    virtual ~dScMgPachinko_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void StartRound(int arg);               /* slot 18 */

    /* Members in ROM order, 0x020fa7b8..0x020feba8, between D0 and StartRound.
       The *_State* handlers are the targets of the five pointer-to-member
       state tables. */
    void Deco_DrawAll();
    void Deco_UpdateAll();
    void Deco_ShowAll();
    void Deco_HideAll();
    void Bg_StateSinkOut(int);
    void Bg_StateScroll(int);
    void Bg_StateRiseIn(int);
    void Bg_StateWaiting(int);
    void Bg_Update();
    void Bg_Start();
    void Bg_Stop();
    void Target_DrawAll();
    void Target_ShowAll();
    void Target_HideAll();
    void Flyer_Draw();
    void Flyer_CheckHits(int);
    void Flyer_StateLeave(int);
    void Flyer_StatePause(int);
    void Flyer_StateCross(int);
    void Flyer_StateDescend(int);
    void Flyer_StateWaiting(int);
    void Flyer_Update();
    void Target_Choose();
    void Fx_DrawAll();
    void Fx_UpdateAll();
    void Fx_Spawn(int, int, int, int, int);
    void CheckTargetsLeft();
    void DrawScore();
    void DrawTopRecord();
    void Popup_DrawAll();
    void Popup_UpdateAll();
    void Popup_SpawnScore(int, unsigned short);
    void ClearAimLine();
    void PlotSubBg2Pixels(int, int, int);
    void DrawAimLine();
    void Pipe_DrawAll();
    void Pipe_SetAll(int);
    int Pipe_StateIdle(int);
    void Pipe_StateSink(int);
    void Pipe_StateRise(int);
    void Pipe_StatePick(int);
    void Pipe_UpdateAll();
    void Pipe_ResetAll();
    void Ball_DrawAll();
    void Ball_CountNearby(int);
    void Ball_CheckPipes(int);
    void Ball_AimAtTarget(int);
    void Ball_CheckFall(int);
    void Ball_StatePop(int);
    void Ball_StateSpinAway(int);
    void Ball_StateGlide(int);
    void Ball_StateDive(int);
    void Ball_StateDrift(int);
    void Ball_StateFall(int);
    void Ball_StateWaiting(int);
    void Ball_UpdateAll();
    void Ball_CheckHitByBobOmb(int);
    void SpawnWave();
    void BobOmb_ClearAll();
    void BobOmb_DrawAll();
    void BobOmb_UpdateAll();
    void CountLaunch();
    void BobOmb_StateWaitGrab(int);
    void BobOmb_StateAim(int);
    void BobOmb_StateFlying(int);
    void BobOmb_StateFallAway(int);
    void BobOmb_StateRemove(int);
    void UpdateNextBall();
    void ResetRound();

    dScMgPachinko_ball mBall[30];    /* 0x4660, stride 0x38, ends 0x4cf0 */
    dScMgPachinko_popup mPopup[30];  /* 0x4cf0 */
    dScMgPachinko_row   mRow[3];     /* 0x4e58 */
    dScMgPachinko_pipe  mPipe[2];    /* 0x4ea0 */
    /* 48: Flyer_CheckHits, BobOmb_ClearAll/_DrawAll/_UpdateAll,
       UpdateNextBall and ResetRound all walk 0x30 records,
       which end exactly at mFx (0x4ed8 + 48 * 0x38 = 0x5958). */
    dScMgPachinko_shot mShot[48];     /* 0x4ed8, stride 0x38 */
    dScMgPachinko_fx     mFx[30];     /* 0x5958 */
    /* mFlyer and mBg are one-element arrays: their state handlers take an
       index and step it (i * 0x1c, i * 0x14) the way the array handlers do,
       and both dispatchers pass 0. [inference from codegen] */
    dScMgPachinko_flyer  mFlyer[1];   /* 0x5bb0 */
    dScMgPachinko_target mTarget[4];  /* 0x5bcc */
    dScMgPachinko_bg     mBg[1];      /* 0x5bfc */
    s32 mPhase;                       /* 0x5c10 -- Behavior: 0 setup, 1 play, 2 results wait */
    s32 mLineErr;                     /* 0x5c14 -- DrawAimLine's Bresenham
                                         error term: seeded with half the major
                                         delta, plus the minor delta each step */
    u16 mEndTimer;                    /* 0x5c18 -- 0x40 frames from time up to the results */
    u8 pad_5c1a[0x2];                 /* 0x5c1a */
    u16 mNextBallDelay; /* 0x5c1c -- frames to the next penned Bob-omb (throw speed >> 10) */
    u16 mUnused5c1e;    /* 0x5c1e -- ResetRound zeroes it; nothing reads it */
    u16 mBallsServed;   /* 0x5c20 -- Bob-ombs UpdateNextBall has put on
                           the launcher this round; every one after the first
                           plays sound 0x18d */
    u16 mUnused5c22;    /* 0x5c22 -- ResetRound zeroes it; nothing reads it */
    u16 mWaveTimer;     /* 0x5c24 -- frames to the next spawn; 0x50 - 4 per level, >= 0x20 */
    u16 mWaveSpawns;    /* 0x5c26 -- enemies spawned; every tenth raises mWaveLevel */
    u16 mWaveLevel;     /* 0x5c28 -- 0..0x27: enemy speed, spawn rate and wave size */
    u16 mHitPause;      /* 0x5c2a -- 0x20 frames of only effects after a shot hits a flyer */
    /* trailing extent the ROM's `new dScMgPachinko_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_5c2c[0x3];
    u8 mAimShot; /* 0x5c2f -- 1 + the mShot index whose aim line is drawn, 0 for
                    none: set when UpdateNextBall serves a shot, read
                    by DrawAimLine, cleared by ClearAimLine */
    u8 mUnused5c30; /* 0x5c30 -- ResetRound and CheckTargetsLeft (time up) clear it;
                    nothing reads it */
    u8 mUnused5c31; /* 0x5c31 -- cleared with mUnused5c30; nothing reads it */
    u8 mPlayStarted; /* 0x5c32 -- latched from 0 to 1 by CountLaunch on the
                        first play-phase frame Behavior runs it (Behavior skips it
                        while mHitPause is set); ResetRound
                        clears it. No reader outside that latch (medium confidence). */
    u8 pad_5c33;
    u8 mFlyerOut; /* 0x5c34 -- nonzero while the flyer is up: Target_Choose
                     returns early on it and bumps it when it launches the
                     flyer; the flyer's leave and hit paths clear it */
    u8 pad_5c35[0x3];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_c_size_must_be_0x5c38[sizeof(struct dScMgPachinko_c) == 0x5c38 ? 1 : -1];
#endif

#endif
