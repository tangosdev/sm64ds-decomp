#ifndef DSCMINIGM_C_H
#define DSCMINIGM_C_H
#include "dScene_c.h"

/* The minigame-select menu scene. Four character tabs of nine icons
 * on the top screen; the sub screen scrolls by mSubBgScrollX onto the
 * second page. */
struct dScMiniGm_c : dScene_c {
    s32 mSubBgScrollX;           /* 0x050 */
    u8  mPageFlipped;            /* 0x054 */
    u8  pad_055[0x3];
    s32 mGroupBase;              /* 0x058 */
    s32 unk_05c;                 /* 0x05c -- InitResources zeroes; 020c0378 copies into the selected entry */
    s32 unk_060;                 /* 0x060 -- InitResources zeroes */
    s32 mGfxSlot;                /* 0x064 -- 020c16e4 walks 0..9 for char/palette dest */
    s32 mGfxSlots[9];            /* 0x068 -- 020c16e4 writes, 020c1688 reads as palette dest */
    s32 mArrowBobPhase;          /* 0x08c */
    s32 mPrevPageTimer;          /* 0x090 */
    s32 mNextPageTimer;          /* 0x094 */
    s32 mExitTimer;              /* 0x098 */
    s32 mIconBlinkPhase;         /* 0x09c */
    s32 mScrollDelay;            /* 0x0a0 */
    s32 unk_0a4;                 /* 0x0a4 -- InitResources zeroes */
    s32 unk_0a8;                 /* 0x0a8 -- InitResources zeroes */
    u8  mExiting;                /* 0x0ac */
    u8  pad_0ad[0x3];

    /* Inline is load-bearing: out-of-line emits D0 before D1 plus a
       homeless D2. Empty because this class adds only scalars. */
    virtual ~dScMiniGm_c() {}                            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMiniGm_c_size_must_be_0xb0[sizeof(dScMiniGm_c) == 0xb0 ? 1 : -1];
#endif

#endif
