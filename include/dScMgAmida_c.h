#ifndef DSCMGAMIDA_C_H
#define DSCMGAMIDA_C_H
#include "dScMgBase_c.h"

/* The destructor tears down four arrays with __destroy_arr; both
   per-element destructors are no-ops. */
extern "C" void __destroy_arr(void *arr, int count, int elemSize, void *dtor);
extern "C" void func_ov006_020d116c(void);
extern "C" void NullDestructor_0203d47c(void);

/* One of the 0x80 falling pieces. Behavior adds vel into pos once a tick
   and counts timer up until active clears; Render draws pos >> 12. */
struct dScMgAmida_c_Piece {
    s32 posX;    /* +0x00 */
    s32 posY;    /* +0x04 */
    s32 velX;    /* +0x08 */
    s32 velY;    /* +0x0c */
    s32 timer;   /* +0x10 */
    u8  active;  /* +0x14 */
};
typedef char dScMgAmida_c_Piece_size_must_be_0x18[sizeof(dScMgAmida_c_Piece) == 0x18 ? 1 : -1];

/* The Amida (ghost-leg lottery) minigame scene -- a dScMgBase_c leaf.
   Field evidence and the full vtable census live in
   notes/minigame-provenance.md.

   TWO CONSTRAINTS HERE ARE MEASURED, NOT STYLE:

   1. Unk36 (slot 36) is declared below as a real virtual, but its three
      call sites in InitResources/Behavior/Render MUST keep the
      pre-migration vtable-shim dispatch. Calling it as a plain
      `this->Unk36()` compiles Render 0xc bytes larger (0x2ac vs 0x2a0),
      because mwcc lands its own slot for Unk36 right after dScMgBase_c's
      compiler-visible virtuals -- dScMgBase_c leaves slots 18-35
      undeclared -- not on true ROM slot 36. That one delta cascaded into
      ~1400 unrelated-looking mismatches; traced via final_link.o.xMAP.

   2. AfterCleanupResources returns early when vfSuccess != 2, which skips
      the base-class call as well. That is what the ROM does. Keep it.

   Slot 35 (func_ov006_020d1170) is deliberately not declared, renamed or
   touched: nothing in this class calls it. */
struct dScMgAmida_c : dScMgBase_c {
    virtual ~dScMgAmida_c();
    virtual s32  InitResources();                       /* slot  0 */
    virtual void AfterCleanupResources(u32 vfSuccess);   /* slot  5 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual int  Unk36();                                /* slot 36 */

    u8  unk_4660[4][8];      /* 0x4660 -- only ever passed around whole */
    u8  pad_4680[0x50];
    s32 mState;               /* 0x46d0 -- Behavior's state switch: 0 setup,
                                 1 playing, 2 result wait, 3 finale */
    u8  mFinished;            /* 0x46d4 -- set when mRoundCount reaches 5;
                                 never cleared except by a full reset */
    u8  unk_46d5;             /* 0x46d5 */
    u8  pad_46d6[0x2];
    u8  pad_46d8[0x28];       /* 0x46d8 -- slot 34's digit-drawing state */
    s32 mLineEndY;            /* 0x4700 -- 0x78 or 0x98; the y2 argument of
                                 the four vertical ghost-leg line draws */
    u8  pad_4704[0x8];        /* 0x4704 -- slot 34's flag bytes */
    u8 *unk_470c;             /* 0x470c -- 0x100 rows x 0x158 bytes, cleared
                                 row-wise; slot 34 indexes it as y*0x158 */
    u8 *unk_4710;             /* 0x4710 -- second buffer of the same shape */
    s32 unk_4714[4];          /* 0x4714 */
    s32 mLanePos[4][2];       /* 0x4724 -- Fix12 {x,y} of the four lane
                                 markers; seeded to x=0x20+0x40*i, y=0xb0 */
    s32 mLaneVel[4][2];       /* 0x4744 -- Fix12 {dx,dy} added into mLanePos
                                 each tick; dy loses 0x100 per tick (gravity) */
    s32 unk_4764;             /* 0x4764 */
    dScMgAmida_c_Piece mPieces[0x80]; /* 0x4768 */
    s32 mScrollSpeed;          /* 0x5368 -- per-tick scroll step, from the
                                 pattern table and clamped to 0x64 */
    s32 mScrollAccum;          /* 0x536c -- mScrollSpeed accumulates here;
                                 the high bits become the step count, low 4 kept */
    u8  pad_5370[0x4];
    s32 mRoundCount;           /* 0x5374 -- zeroed on reset; below 5 the scene
                                 replays, at 5 it finishes */
    u8  pad_5378[0x24];
    s32 mLaneAnimTimer[4];     /* 0x539c -- per lane; wraps on the per-lane
                                 period in data_ov006_0213b880 */
    s32 mLaneAnimFrame[4];     /* 0x53ac -- per lane, cycles 0..0xd into the
                                 sprite table data_ov006_0213a458 */
    u16 mBgScrollPhase;        /* 0x53bc -- += 0xc0 a frame; indexes the sine
                                 table for the sub-screen BG2 offset */
    u8  pad_53be[0x2];
    s32 mResultWaitTimer;       /* 0x53c0 -- 0x3c frames of state 2 */
    s32 mStartBannerTimer;      /* 0x53c4 -- 0x3c; on expiry gfx slot 0xd (the
                                 start banner) is freed and the prompt blinks */
    u8  pad_53c8[0x8];
    s32 mEndDelayTimer;         /* 0x53d0 -- 0xb4; Render holds the play field
                                 until it drains, then draws the finale */
    s32 mPatternIndex;          /* 0x53d4 -- selects the ghost-leg pattern;
                                 indexes five 0x1c-stride tables in ov006 */
    u8  pad_53d8[0x4];
    u8  unk_53dc;                /* 0x53dc */
    u8  unk_53dd;                /* 0x53dd */
    u8  pad_53de[0x2];
    s32 mRoundTimer;             /* 0x53e0 -- counts down; expiry ends the
                                 round and picks replay or finish */
    s32 unk_53e4;                /* 0x53e4 */
    s32 mScore;                  /* 0x53e8 -- mLevel * 5, clamped to 9999;
                                 pushed to the HUD every Behavior tick */
    u8  pad_53ec[0x10];          /* tail padding to the 0x53fc allocation */
};

typedef char dScMgAmida_c_size_must_be_0x53fc[sizeof(dScMgAmida_c) == 0x53fc ? 1 : -1];

#endif
