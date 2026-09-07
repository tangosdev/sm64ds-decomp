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
      call sites in InitResources/Behavior/Render still use the
      pre-migration vtable-shim dispatch, and THE REASON THEY HAD TO IS
      GONE. Calling it as a plain `this->Unk36()` used to compile Render
      0xc bytes larger (0x2ac vs 0x2a0) because mwcc landed its own slot
      for Unk36 right after dScMgBase_c's compiler-visible virtuals --
      dScMgBase_c left slots 18-35 undeclared -- rather than on true ROM
      slot 36. That one delta cascaded into ~1400 unrelated-looking
      mismatches; traced via final_link.o.xMAP. The base declares all
      eighteen now and Unk36 sits on 36, so the shim may well be
      removable. That is a MEASUREMENT nobody has taken, not a conclusion:
      the shims are kept here because they match today, and replacing
      them is its own commit with its own rombuild behind it. Do not
      remove them on the strength of this paragraph.

   2. AfterCleanupResources returns early when vfSuccess != 2, which skips
      the base-class call as well. That is what the ROM does. Keep it.

   Slot 35 is declared and renamed now -- `Virtual8C`, this class's override
   at ov006:0x020d1170.  Nothing in THIS class calls it, which is why it was
   left alone for so long; the thirteen call sites are in four other leaves
   (dScMgCoin_c, dScMgPanel_c, dScMgSound_c, dScMgSnowball_c), each asking the
   question of itself.  This class narrows the base's `(param1 & 0xff) != 0`
   to `== 1`. */
struct dScMgAmida_c : dScMgBase_c {
    /* A touch-screen cell, x then y.  ALL THREE SPECIAL MEMBERS ARE
       LOAD-BEARING, measured on the pen handler func_ov006_020d1ba0 under
       2004/b56:
         - the empty destructor keeps a local of this type on the stack (the
           dScMgFlower_c::Vec2 rule) and, because the by-value temporaries
           below then need destroying after the call, makes mwcc materialise
           `func(...) == n` into a bool before the branch -- the ROM's
           moveq/movne/cmp triple at every func_ov006_020d2580 call;
         - the user copy constructor makes a by-value Point argument travel
           by reference to a caller-side temporary that is filled field by
           field, x before y and the first argument before the second (the
           implicit copy does the second argument first), and the fields
           are re-read through the reference rather than reused from the
           guard that just compared them;
         - the default constructor exists only because declaring the copy
           constructor removes the implicit one. */
    struct Point {
        s32 x;
        s32 y;
        Point() {}
        Point(const Point &o) : x(o.x), y(o.y) {}
        ~Point() {}
    };

    virtual ~dScMgAmida_c();
    virtual s32  InitResources();                       /* slot  0 */
    virtual void AfterCleanupResources(u32 vfSuccess);   /* slot  5 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    /* Overrides dScMgBase_c's slot 18, so the base fixes its index.  Unk36
       below is NOT an override: it is this class's own brand-new slot.
       dScMgBase_c declares all eighteen of 18-35 now, so mwcc lands Unk36 on
       36, which is where the ROM puts it.  That was the last DIFFERS in the
       minigame family and it is closed -- not by anything local to this
       header, but by the base finally spelling the slots underneath it.
       Keep Unk36 LAST all the same: any further virtual added to dScMgBase_c
       would take 36 and push it out again. */
    virtual void OnYoshiTryEat(int arg);                 /* slot 18 */
    virtual int  Virtual7C();                            /* slot 31 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */
    virtual int  Virtual8C();                          /* slot 35 */
    virtual int  Unk36();                                /* slot 36 */

    s32 unk_4660[4][2];      /* 0x4660 */
    s8  unk_4680[4];          /* 0x4680 */
    s32 unk_4684[4];          /* 0x4684 */
    s32 unk_4694[4];          /* 0x4694 */
    s32 unk_46a4[4];          /* 0x46a4 */
    s8  unk_46b4[4];          /* 0x46b4 */
    s32 unk_46b8[4];          /* 0x46b8 */
    s32 unk_46c8;             /* 0x46c8 */
    s32 unk_46cc;             /* 0x46cc */
    s32 mState;               /* 0x46d0 -- Behavior's state switch: 0 setup,
                                 1 playing, 2 result wait, 3 finale */
    u8  mFinished;            /* 0x46d4 -- set when mRoundCount reaches 5;
                                 never cleared except by a full reset */
    u8  unk_46d5;             /* 0x46d5 */
    u8  pad_46d6[0x2];
    /* The rung the player is drawing, as func_ov006_020d1ba0 (the pen
       handler) keeps it: mLineStart is the end snapped to the lane the pen
       started on, mLineEnd the end on the lane it crossed to.  x is the lane axis (0x20 + 0x40 * lane) and doubles as the row
       index of the two 0x158-stride grids below; -1 while idle. */
    Point mLineStart;         /* 0x46d8 */
    Point mLineEnd;           /* 0x46e0 */
    Point mPenStart;          /* 0x46e8 -- cell the pen came down on; -1 while up */
    Point mPen;               /* 0x46f0 -- cell the pen was on last tick; -1 while up */
    Point mPrev;              /* 0x46f8 -- slot 34's previous brush cell */
    s32 mLineEndY;            /* 0x4700 -- 0x78 or 0x98; the y2 argument of
                                 the four vertical ghost-leg line draws */
    u8  mLineStartSet;        /* 0x4704 -- mLineX1/Y1 accepted by the probe */
    u8  mLineEndSet;          /* 0x4705 -- mLineX2/Y2 accepted by the probe */
    u8  mLineCommit;          /* 0x4706 -- set while the accepted rung is painted */
    u8  unk_4707;             /* 0x4707 */
    u8  mProbeMode;           /* 0x4708 -- slot 34 tests the grid instead of painting */
    u8  mProbeHit;            /* 0x4709 -- slot 34 found an occupied cell */
    u8  mLineCount;           /* 0x470a -- rungs drawn; 0xff stops the pen */
    u8  pad_470b;             /* 0x470b */
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
    u8  mPenTapped;             /* 0x53c8 -- the pen has not moved since it came down */
    u8  pad_53c9[0x3];
    s32 mPenSoundHandle;        /* 0x53cc -- the pen-drag sound, re-armed per tick */
    s32 mEndDelayTimer;         /* 0x53d0 -- 0xb4; Render holds the play field
                                 until it drains, then draws the finale */
    s32 mPatternIndex;          /* 0x53d4 -- selects the ghost-leg pattern;
                                 indexes five 0x1c-stride tables in ov006 */
    u8  pad_53d8[0x4];
    u8  unk_53dc;                /* 0x53dc */
    u8  unk_53dd;                /* 0x53dd */
    u8  unk_53de;                /* 0x53de -- input lock: the pen handler bails while set */
    u8  pad_53df;
    s32 mRoundTimer;             /* 0x53e0 -- counts down; expiry ends the
                                 round and picks replay or finish */
    s32 unk_53e4;                /* 0x53e4 */
    s32 mScore;                  /* 0x53e8 -- mLevel * 5, clamped to 9999;
                                 pushed to the HUD every Behavior tick */
    u8  pad_53ec[0x10];          /* tail padding to the 0x53fc allocation */
};

typedef char dScMgAmida_c_Point_size_must_be_0x8[sizeof(dScMgAmida_c::Point) == 0x8 ? 1 : -1];
typedef char dScMgAmida_c_size_must_be_0x53fc[sizeof(dScMgAmida_c) == 0x53fc ? 1 : -1];

#endif
