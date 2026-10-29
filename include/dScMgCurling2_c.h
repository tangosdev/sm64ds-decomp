/* class dScMgCurling2_c : dScMgBase_c. Same shape as dScMgCurling_c --
 * confirmed leaf, D1 writes only its own vtable then calls dScMgBase_c's
 * D2 directly. See include/dScMgCurling_c.h for the shared reasoning
 * (inherited-field misattribution below 0x4660, no size assertion). */
#ifndef DSCMGCURLING2_C_H
#define DSCMGCURLING2_C_H
#include "dScMgBase_c.h"

/* One curling stone, 0x30 bytes, eleven of them at 0x4660 (dScMgCurling_c's
 * stone is the same record at 0x2c; this one carries four more bytes). */
struct dScMgCurling2_stone {
    s32 x;         /* 0x00 */
    s32 y;         /* 0x04 */
    s32 speed;     /* 0x08 */
    s32 prevX;     /* 0x0c, snapshot of x before this frame's step */
    s32 prevY;     /* 0x10 */
    s32 grabX;     /* 0x14, stone position minus stylus position, in Fix12 */
    s32 grabY;     /* 0x18 */
    s32 snd;       /* 0x1c, rolling-sound slot for func_02012468 */
    u16 timer;     /* 0x20, counts down while the stone is active */
    u16 spinVel;   /* 0x22, sprite spin rate, written from speed and angle */
    u16 spinAngle; /* 0x24, accumulates spinVel */
    u16 angle;     /* 0x26, travel heading */
    u8 state;      /* 0x28 */
    u8 active;     /* 0x29 */
    u8 visible;    /* 0x2a, draw gate */
    u8 fast;       /* 0x2b, speed >= 0x3800 after a hit */
    u8 unk2c;      /* 0x2c, write-only: NextStone and ClearStoneFlags zero it, nothing reads it */
    u8 target;     /* 0x2d, a house stone: alt sprite, x100 score */
    u8 unk2e[0x2]; /* 0x2e, untouched */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    dScMgCurling2_stone_size_must_be_0x30[sizeof(struct dScMgCurling2_stone) == 0x30 ? 1 : -1];
#endif

/* Falling piece, 0x24 bytes, 0x32 of them at 0x48c0. Seeded across the top
 * of the screen and recycled once y passes 0xc8. */
struct dScMgCurling2_piece {
    s32 x;           /* 0x00, 20.12 */
    s32 y;           /* 0x04, 20.12 */
    s32 xInc;        /* 0x08 */
    s32 yInc;        /* 0x0c */
    s32 yTarget;     /* 0x10, the value yInc ramps toward */
    u16 countdown;   /* 0x14 */
    u16 countdown2;  /* 0x16 */
    u16 countdown3;  /* 0x18 */
    u8 pad1a[2];     /* 0x1a */
    u8 updateEnable; /* 0x1c */
    u8 modeIndex;    /* 0x1d, into sCurling2StepModeTable */
    u8 xIndex;       /* 0x1e, into sCurling2StepXTable or _021419b8 */
    u8 yIndex;       /* 0x1f, into sCurling2StepYTable */
    u8 drawEnable;   /* 0x20 */
    u8 sprite0;      /* 0x21, into sCurlingBitSprites */
    u8 sprite1;      /* 0x22 */
    u8 pad23;        /* 0x23 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    dScMgCurling2_piece_size_must_be_0x24[sizeof(struct dScMgCurling2_piece) == 0x24 ? 1 : -1];
#endif

/* Number spawned between two stones when they collide, 0x18 bytes,
 * 0x3c of them at 0x4fe0. */
struct dScMgCurling2_value {
    s32 x;        /* 0x00, 20.12 */
    s32 y;        /* 0x04, 20.12 */
    s32 xInc;     /* 0x08, zeroed at spawn and never read */
    s32 yInc;     /* 0x0c, decays by 0x40 a frame */
    u16 lifetime; /* 0x10 */
    u16 value;    /* 0x12, drawn as the sprite's number */
    u8 live;      /* 0x14 */
    u8 mode;      /* 0x15, 1 or 2 */
    u8 pad16[2];  /* 0x16 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    dScMgCurling2_value_size_must_be_0x18[sizeof(struct dScMgCurling2_value) == 0x18 ? 1 : -1];
#endif

/* Per-throw score marker, 0x10 bytes, five of them at 0x4870 -- one per
 * thrown stone. The caption is drawn once the countdown expires. */
struct dScMgCurling2_mark {
    s32 x;         /* 0x00, 20.12 */
    s32 y;         /* 0x04 */
    u16 value;     /* 0x08, the caption */
    u16 countdown; /* 0x0a, frames until the caption shows */
    u8 enable;     /* 0x0c, the countdown runs while set */
    u8 drawn;      /* 0x0d, set on expiry; DrawMarks gates on it */
    u8 unk0e[0x2]; /* 0x0e, untouched */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    dScMgCurling2_mark_size_must_be_0x10[sizeof(struct dScMgCurling2_mark) == 0x10 ? 1 : -1];
#endif

struct dScMgCurling2_c : dScMgBase_c {
    virtual ~dScMgCurling2_c();

    virtual s32 InitResources();      /* slot 0 */
    virtual s32 Behavior();           /* slot 6 */
    virtual s32 Render();             /* slot 9 */
    virtual void StartRound(int arg); /* slot 18 */

    /* Slot 18 left unnamed -- same reasoning as dScMgCurling_c.h. */

    /* NON-VIRTUAL MEMBERS REACHED THROUGH THE OV006 POINTER-TO-MEMBER TABLES.
     * These seventeen are members, not free functions: ov006 .data carries an
     * 8-byte {code pointer, zero adjustment} record for sixteen of them, and
     * __sinit_ov006_02130758 copies those records into the BSS arrays that
     * dScMgCurling2_c's own code dispatches through.  A zero-adjustment
     * pointer-to-member record IS the proof of member-ness; the ROM does not
     * carry the original identifiers, so the NAMES BELOW ARE COINED.
     *
     * Each name is derived from the byte the handler drives and from its index
     * in its own table, both read out of the cartridge:
     *
     *   sCurling2StepModeTable (+0x1d, the mode byte)
     *     [0] PickStepMode   [1] StepXOnly   [2] StepXAndY
     *   sCurling2StepXTable (+0x1e, X increment, 8 a frame, +/-0x300)
     *     [0] StepXPick  [1] StepXPushPos  [2] StepXPushNeg  [3] StepXSettle
     *   sCurling2StepXFastTable (+0x1e, X increment, 0x20 a frame, +/-0x400)
     *     [0] StepXPickFast [1] StepXPushPosFast
     *     [2] StepXPushNegFast [3] StepXSettleFast
     *   sCurling2StepYTable (+0x1f, Y increment)
     *     [0] StepYRestart   [1] StepYRampUp   [2] StepYRampDown
     *   sCurling2DragTable (the stylus drag)
     *     [0] DragBegin      [1] DragUpdate
     *
     * The index each handler writes back agrees with that layout in every case
     * -- the two pushers hand to 3, the settler hands to 0, the Y chain runs
     * 0 -> 1 -> 2 -> 0 -- and sCurling2StepXPicks/_0212e4f8/_0212e4fc, the ROM
     * tables the two pickers and PickStepMode read from, hold exactly {1, 2}.
     * Those handlers step one dScMgCurling2_piece (the 0x48c0 array). The
     * method names are still coined; the ROM does not carry them.
     *
     * Members a still-shard caller spells as func_ov006_* stay that way.
     * Renaming them means editing those callers or include/decl_common.h. */
    void PickStepMode(int entry);
    void StepXOnly(int entry);
    void StepXAndY(int entry);

    void StepXPick(int entry);
    void StepXPushPos(int entry);
    void StepXPushNeg(int entry);
    void StepXSettle(int entry);

    void StepXPickFast(int entry);
    void StepXPushPosFast(int entry);
    void StepXPushNegFast(int entry);
    void StepXSettleFast(int entry);

    void StepYRestart(int entry);
    void StepYRampUp(int entry);
    void StepYRampDown(int entry);

    void DragBegin();
    void DragUpdate();

    void SpawnValue(int stone, int other);

    /* The rest of the class's members, proven the same way: the five
     * Behavior-state handlers ride sCurling2BehaviorTable on mState, the four
     * stone-state handlers ride sCurling2StoneTable on mStone[i].state, and
     * the remaining helpers take the object as their first argument. These
     * names are coined too; the ROM does not carry them. */
    void BeginRound(); /* Behavior state 0 */
    void Play();       /* Behavior state 1 */
    void NextThrow();  /* Behavior state 2 */
    void EndRound();   /* Behavior state 3 */
    void Idle();       /* Behavior state 4 */

    void StoneWait(int i);  /* stone state 0 */
    void StoneSlide(int i); /* stone state 1 */
    void StoneRest(int i);  /* stone state 2 */
    void StoneSteer(int i); /* stone state 3 */

    void StoneSpin(int i);
    void NextStone();
    void SeedStones();
    void ClearStoneFlags();
    void Scroll();
    void ResetScroll();
    void ResetGame();

    void DrawValues();
    void AgeValues();
    void ClearValues();
    void DrawPieces();
    void StepPieces();
    void SeedPieces();
    void DrawMarks();
    void AgeMarks();
    void DrawCursor();
    void DrawCounter();
    void DrawStones();
    void SeparateStones(int idx);

    dScMgCurling2_stone mStone[11];   /* 0x4660, stride 0x30 */
    dScMgCurling2_mark mMark[5];      /* 0x4870 */
    dScMgCurling2_piece mPiece[0x32]; /* 0x48c0 */
    s32 bg2x;                         /* 0x4fc8, BG2 scroll, 20.12 */
    s32 bg2y;                         /* 0x4fcc */
    s32 scrollIdx;                    /* 0x4fd0, scroll mode 0..3; 0xff repicks */
    s32 bg0x;                         /* 0x4fd4, BG0 scroll */
    s32 bg0y;                         /* 0x4fd8 */
    u8 pad_4fdc[0x4];                 /* 0x4fdc, untouched */
    dScMgCurling2_value mValue[0x3c]; /* 0x4fe0 */
    s32 mState;                       /* 0x5580, Behavior state, indexes sCurling2BehaviorTable */
    s32 mDragX;                       /* 0x5584, drag x (fx32) */
    s32 mDragY;                       /* 0x5588, drag y */
    s32 mGrabX;                       /* 0x558c, drag x + grab x latch */
    s32 mSwingReferenceY;             /* 0x5590, drag y at the last direction change */
    s32 mGrabOffsetX;                 /* 0x5594, drag x offset from the stylus */
    s32 mGrabOffsetY;                 /* 0x5598, drag y offset */
    s32 mThrowSpeed;                  /* 0x559c, throw power, capped at 0xc000 */
    s32 unk_55a0;                     /* 0x55a0, write-only: DragBegin zeroes it */
    s32 unk_55a4;                     /* 0x55a4, write-only: DragBegin zeroes it */
    s32 mLastSwingDeltaY;             /* 0x55a8, last drag dy */
    s32 mTopRecord;                   /* 0x55ac, cached Mg_GetTopRecord result */
    u16 unk_55b0;                     /* 0x55b0, write-only: ResetGame zeroes it */
    u16 mThrowAngle;                  /* 0x55b2, throw angle, clamped to 0x9800..0xe800 */
    u16 mSpawnTimer;                  /* 0x55b4, counts down to the next stone */
    u16 mStateTimer;                  /* 0x55b6, state-entry countdown */
    u8 mDragState;                    /* 0x55b8, drag-state index into sCurling2DragTable */
    u8 mCursorVisible;                /* 0x55b9, enables DrawCursor */
    u8 thrown;                        /* 0x55ba, stones thrown this round (0..5) */
    u8 spawning;                      /* 0x55bb, a stone is queued to spawn */
    u8 gameOver;                      /* 0x55bc, set once the last stone is thrown */
    u8 sndCooldown;                   /* 0x55bd, scrape-sound throttle */
    u8 mSwingState;                   /* 0x55be, drag phase */
    u8 combo;                         /* 0x55bf, collision-streak counter for SpawnValue */
    u8 unk_55c0;                      /* 0x55c0, write-only: StoneWait zeroes it */
    u8 unk_55c1;                      /* 0x55c1, write-only: StoneWait zeroes it */
    u8 unk_55c2;                      /* 0x55c2, untouched */
    u8 mCounterVisible;               /* 0x55c3, enables DrawCounter */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCurling2_c_size_must_be_0x55c4[sizeof(dScMgCurling2_c) == 0x55c4 ? 1 : -1];
#endif

#endif
