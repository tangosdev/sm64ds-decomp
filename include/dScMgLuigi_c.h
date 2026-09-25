#ifndef DSCMGLUIGI_C_H
#define DSCMGLUIGI_C_H
#include "dScMgBase_c.h"

/* Luigi's Wanted minigame. A poster names one character; pictures of the
   cast move on the touch screen and the player taps the match. A miss
   drops a falling mark and costs time, a hit pops a reward and adds time,
   and a pair of curtains plus an iris wipe separate the rounds. The timer
   runs out into a failed catch. dScMgLuigi_c is the ROM name of the scene.
   Storage below 0x4660 belongs to dScMgBase_c.

   tools/rtti_extract.py confirms it is a leaf: no RTTI record names it as a
   base. The reconstructed factory dScMgLuigi_c_classInit (historical alias
   MgWanted_Spawn) installs this class's vtable for the MG_LUIGI registry
   profile. */

/* One miss mark. Sixteen of them sit at 0x4660. y falls under velY while
   phase is 0; the timer then holds the mark up before active and shown clear. */
struct dScMgLuigi_Penalty {
    Fix12i x;        /* +0x00 picture position, in pixels << 12 */
    Fix12i y;        /* +0x04 */
    Fix12i velY;     /* +0x08 starts at 0x1100 and gains 0x100 a frame */
    s16    timer;    /* +0x0c counts down, then reloads to 0x40 */
    u8     pad_e[2];
    u8     active;   /* +0x10 UpdatePenalties runs while this is set */
    u8     shown;    /* +0x11 DrawPenalties */
    u8     phase;    /* +0x12 0 = falling, then holding */
    u8     pad_13;
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgLuigi_Penalty_size_must_be_0x14[sizeof(dScMgLuigi_Penalty) == 0x14 ? 1 : -1];
#endif

/* One curtain panel. Two of them, left then right, stride 0x18 at 0x47a0.
   x slides toward the middle of the screen; state picks the motion and
   phase picks the step inside it. */
struct dScMgLuigi_Curtain {
    Fix12i x;        /* +0x00 */
    Fix12i y;        /* +0x04 parked at 0x60 */
    Fix12i velX;     /* +0x08 */
    s32    sound;    /* +0x0c handle handed back by Sound_PlayIfNotActive */
    s16    delay;    /* +0x10 waits out before the panel moves */
    u16    count;    /* +0x12 ticks up to the whoosh */
    u8     active;   /* +0x14 CurtainsDone still has work */
    u8     shown;    /* +0x15 DrawCurtains */
    u8     state;    /* +0x16 which motion */
    u8     phase;    /* +0x17 step within the motion */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgLuigi_Curtain_size_must_be_0x18[sizeof(dScMgLuigi_Curtain) == 0x18 ? 1 : -1];
#endif

/* The "got it" pop above the picture that was found. It rises, holds,
   then ticks sparks times, adding one second each, up to 50. */
struct dScMgLuigi_Reward {
    Fix12i x;        /* +0x00 */
    Fix12i y;        /* +0x04 */
    Fix12i velY;     /* +0x08 starts negative and keeps accelerating up */
    u8     pad_c[3];
    u8     timer;    /* +0x0f */
    u8     active;   /* +0x10 UpdateReward */
    u8     phase;    /* +0x11 0 rising, 1 holding, then the spark count */
    u8     sparks;   /* +0x12 how many +1s ticks are left */
    u8     shown;    /* +0x13 DrawReward */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgLuigi_Reward_size_must_be_0x14[sizeof(dScMgLuigi_Reward) == 0x14 ? 1 : -1];
#endif

/* The round-end iris. One record, addressed with the same 0x14 stride as
   the penalty marks; only index 0 is used. radius opens until it covers
   the screen. y and scale are stored with it and not read back here. */
struct dScMgLuigi_Iris {
    Fix12i x;        /* +0x00 center, 128 px */
    Fix12i y;        /* +0x04 center, 96 px */
    Fix12i radius;   /* +0x08 grows by 0x2000 until it passes 0xa0 */
    s32    scale;    /* +0x0c held at 0x1000 */
    u8     active;   /* +0x10 */
    u8     state;    /* +0x11 which iris step is running */
    u8     pad[2];
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgLuigi_Iris_size_must_be_0x14[sizeof(dScMgLuigi_Iris) == 0x14 ? 1 : -1];
#endif

struct dScMgLuigi_c : dScMgBase_c {
    virtual ~dScMgLuigi_c();
    virtual s32 InitResources();          /* slot 0 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot 5 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
    virtual void OnYoshiTryEat(int arg);  /* slot 18 */

    /* Non-virtual members, in ROM address order. Each is a member because a
       zero-adjustment pointer-to-member record in ov006 .data names it (the
       run at 0x0213cd8c..0x0213ce4c), or because it is only ever reached
       with this object in r0 from another member of the run. The names are
       coined: the cartridge carries only the address. See
       symbols/actor_renames.tsv for the evidence, one row per symbol. */
    void BuildIrisTable(int i);
    void IrisStop(int idx);
    void IrisGrow(int idx);
    void IrisHold(int idx);
    void UpdateIris();
    void StartIris();
    void DrawReward();
    void UpdateReward();
    void StartReward(int i);
    void DrawCurtains();
    void CurtainStateSlow(int i);
    void CurtainStateFast(int i);
    void CurtainStateHold(int i);
    void CurtainStateBounce(int i);
    void CurtainUpdate(int i);
    void CurtainStart(int i);
    int  CurtainsDone();
    void ResetCurtains();
    void DrawTimer();
    void TickTimer();
    void DrawPictures();
    void DrawPenalties();
    void UpdatePenalties();
    void AddPenalty(int idx);
    void DrawWantedIcon();
    void TickPictureFlash(int idx);
    void MovePictureBounce(int i);
    void MovePictureSway(int idx);
    void MovePictureDriftRandom(int idx);
    void MovePictureDriftByRow(int idx);
    void MovePictureDriftByColumn(int idx);
    void MovePictureDriftByLevel(int idx);
    void MovePictureDriftFixed(int idx);
    void WrapPicture(int i);
    void StopPicture(int idx);
    void RestartPicture(int idx);
    void UpdatePictures();
    void BeginCatch(int found);
    void CheckTouch();
    void PlaceBoardLayout();
    void PlaceNextPicture();
    void ChooseTarget();
    void ChooseBoard();
    void ResetBoard();
    void StateCatch();
    void StatePlay();
    void StatePlace();
    void StateSetup();

    dScMgLuigi_Penalty mPenalty[16]; /* 0x4660 */
    dScMgLuigi_Curtain mCurtain[2];  /* 0x47a0 */
    dScMgLuigi_Reward  mReward;      /* 0x47d0 */
    dScMgLuigi_Iris    mIris[1];     /* 0x47e4 -- only [0] is used */

    /* 120 pictures. Positions and velocities are 20.12 and abut mState.
       mBlink counts down after a miss and toggles mShown. mKind picks the
       mover (9 while the miss is still playing). mActive is set once the
       slot has been placed. */
    Fix12i mPosX[120];       /* 0x47f8 -- wraps at 0x110 */
    Fix12i mPosY[120];       /* 0x49d8 -- wraps at 0xd0 */
    Fix12i mVelX[120];       /* 0x4bb8 */
    Fix12i mVelY[120];       /* 0x4d98 */
    s32 mState;              /* 0x4f78 -- 0 setup, 1 place, 2 play, 3 catch */
    u16 mMovePhase[120];     /* 0x4f7c -- sway: 0 slowing, 1 speeding right,
                                2 speeding left. Drift stores an angle. */
    u16 mBlink[120];         /* 0x506c -- miss flash; (t >> 2) & 1 hides */
    u16 mLevelPhase[4];      /* 0x515c -- one random angle per speed */
    u16 mIconOn;             /* 0x5164 -- draw the caught picture while set */
    s16 mIconX;              /* 0x5166 */
    s16 mIconY;              /* 0x5168 */
    s16 mCatchWait;          /* 0x516a -- frames to hold the catch */
    s16 unk_516c;            /* 0x516c -- set to 10 as play starts; no read here */
    s16 unk_516e;            /* 0x516e -- cleared as play starts; no read here */
    u8  pad_5170[2];
    s16 mTime;               /* 0x5172 -- seconds left; misses take 10, a
                                reward spark gives 1 back, capped at 50 */
    u16 mBoard;              /* 0x5174 -- which layout and mover set */
    u16 mPrevBoard;          /* 0x5176 -- last board, so the next one differs */
    u8  mGrid[13][9];        /* 0x5178 -- cells already used while placing */
    u8  mColDir[8];          /* 0x51ed -- 0/1, drift left or right per column */
    u8  mRowDir[6];          /* 0x51f5 -- 0/1, drift up or down per row */
    u8  mTimeTick;           /* 0x51fb -- counts to 60, then mTime drops */
    u8  pad_51fc;
    u8  mKind[120];          /* 0x51fd -- mover id; 9 means a fresh miss */
    u8  mStarted[120];       /* 0x5275 -- 0 until the slot's first tick */
    u8  mActive[120];        /* 0x52ed -- slot has a picture */
    u8  mCharacter[120];    /* 0x5365 -- which character, and how fast */
    u8  mShown[120];         /* 0x53dd -- DrawPictures while 1 */
    u8  mFilled;             /* 0x5455 -- the board is full */
    u8  mTarget;             /* 0x5456 -- 1-based index of the picture to find;
                                scatter placement also counts slots with it */
    u8  mClears;             /* 0x5457 -- found pictures; not read back here */
    u8  mBoardPending;        /* 0x5458 -- 1 from ResetBoard until StatePlace has placed the board; no read here */
    u8  mFound;              /* 0x5459 -- 1 if the catch was the right picture */
    u8  mWanted;             /* 0x545a -- character the poster is asking for */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgLuigi_c_size_must_be_0x545c[sizeof(dScMgLuigi_c) == 0x545c ? 1 : -1];
#endif

#endif
