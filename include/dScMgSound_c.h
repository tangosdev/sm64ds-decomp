/* Boom-box minigame (MG_SOUND). Each round deals note slots on the
 * touch screen; a touch plays the note and pops a bouncing icon.
 *
 * RTTI: dScMgSound_c : dScMgSingle3DBase_c (tools/rtti_extract.py). SIZE
 * 0x562c, from classInit's fBase_c::operator new(0x562c).
 * dScMgSound_c_classInit is reconstructed (historical alias
 * MgBoomBox_Spawn) and installs the vtable for the MG_SOUND registry
 * profile. Retail does not store that spelling.
 *
 * The destructor is defined inline and declared first. mwccarm 2004/b56
 * emits D1 then D0 for an in-class body and D2/D0/D1 for an out-of-line
 * one, and ov006 puts D1 at 0x02119904 below D0 at 0x02119958.
 */
#ifndef DSCMGSOUND_C_H
#define DSCMGSOUND_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c3288(char *t);

/* One HUD sprite. Two of them, not an array: nothing indexes between them. */
struct dMgSoundSprite_c {
    s32 x;            /* +0x00 20.12 screen x */
    s32 y;            /* +0x04 20.12 screen y */
    u16 timer;        /* +0x08 ticks in the current frame */
    u8  pad_0a[2];
    u8  active;       /* +0x0c */
    u8  visible;      /* +0x0d */
    u8  frame;        /* +0x0e */
    u8  pad_0f;
};

/* Touch-screen note. Ten of these, stride 0x14, at 0x50e8. */
struct dMgSoundNote_c {
    s32 x;            /* +0x00 */
    s32 y;            /* +0x04 */
    u16 timer;        /* +0x08 ticks in the current frame */
    u8  pad_0a[2];
    u8  state;        /* +0x0c behavior index */
    u8  alive;        /* +0x0d still in play */
    u8  shown;        /* +0x0e drawn */
    u8  frame;        /* +0x0f animation frame */
    u8  note;         /* +0x10 pitch */
    u8  checked;      /* +0x11 neighbor test already done */
    u8  pad_12[2];
};

/* Icon popped when a note finishes. Thirty of these, stride 0x24, at 0x51b0. */
struct dMgSoundPop_c {
    s32 x;            /* +0x00 */
    s32 y;            /* +0x04 */
    s32 velX;         /* +0x08 */
    s32 velY;         /* +0x0c */
    s32 startY;       /* +0x10 y it was spawned at */
    s32 scale;        /* +0x14 fix12, multiplied into the draw matrix */
    u16 timer;        /* +0x18 */
    u16 pad_1a;
    u8  active;       /* +0x1c gates the behavior */
    u8  visible;      /* +0x1d gates the draw */
    u8  sprite;       /* +0x1e sprite-table index */
    u8  unk_1f;       /* +0x1f cleared by func_ov006_0211c478 */
    u8  state;        /* +0x20 behavior index */
    u8  phase;        /* +0x21 step inside that behavior */
    u8  kind;         /* +0x22 motion-table index */
    u8  dir;          /* +0x23 velX oscillation */
};

/* Storage for the component accesses observed at scene offset 0x4f38.
 * The constructor clears +0x1a8 and the animation callbacks test it before
 * requesting sound. The type and field names are descriptive; 0x1ac is the
 * observed minimum extent, not a recovered original class boundary. */
struct dMgSoundComponentStorage {
    u8 raw_000[0x1a8];
    s32 mSuppressSound;       /* +0x1a8 -- zero permits animation sounds */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgSoundSprite_c_size_must_be_0x10[sizeof(dMgSoundSprite_c) == 0x10 ? 1 : -1];
typedef char dMgSoundNote_c_size_must_be_0x14[sizeof(dMgSoundNote_c) == 0x14 ? 1 : -1];
typedef char dMgSoundPop_c_size_must_be_0x24[sizeof(dMgSoundPop_c) == 0x24 ? 1 : -1];
typedef char dMgSoundComponentStorage_size_must_be_0x1ac[
    sizeof(dMgSoundComponentStorage) == 0x1ac ? 1 : -1];
#endif

struct dScMgSound_c : dScMgSingle3DBase_c {
    virtual ~dScMgSound_c() { func_ov006_020c3288((char *)&mTable); }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */

    dMgSoundComponentStorage mTable; /* 0x4f38; mSuppressSound at 0x50e0 */
    u8  pad_50e4[4];
    dMgSoundNote_c mNotes[10];       /* 0x50e8 */
    dMgSoundPop_c mPops[30];         /* 0x51b0 */
    dMgSoundSprite_c mSpriteA;       /* 0x55e8 frame limit 2 */
    dMgSoundSprite_c mSpriteB;       /* 0x55f8 frame limit 3 */
    s32 mState;                      /* 0x5608 */
    u32 mBank;                       /* 0x560c passed to func_02012174 */
    u16 mQueue[2];                   /* 0x5610 (slot << 8) | (note + 1) */
    u16 mQueueTimer;                 /* 0x5614 */
    u16 mResultTimer;                /* 0x5616 */
    u16 mIntroTimer;                 /* 0x5618 */
    u8  mPairUses[5];                /* 0x561a notes dealt from each pair */
    u8  mPairNotes[5];               /* 0x561f pitch of each pair; pattern 3
                                        fills [0..2] with consecutive lanes,
                                        each previous + 1, mod 3 */
    u8  mTouchCount;                 /* 0x5624 */
    u8  mQueueLen;                   /* 0x5625 write index into mQueue */
    u8  mTries;                      /* 0x5626 counts down from 3 */
    u8  mPattern;                    /* 0x5627 0..4; indexes 0212ee10/ee18 */
    u8  mPrevPattern;                /* 0x5628 */
    u8  pad_5629[3];

    s32 InitResources();             /* slot 0 */
    s32 Behavior();                  /* slot 6 */
    s32 Render();                    /* slot 9 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dScMgSound_c_size_must_be_0x562c[sizeof(dScMgSound_c) == 0x562c ? 1 : -1];
#endif

#endif
