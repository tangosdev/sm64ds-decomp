#ifndef DSCMINIGM_C_H
#define DSCMINIGM_C_H
#include "dScene_c.h"

/* The minigame-select menu scene. Four rows of minigame icons on the top
 * screen; the sub screen scrolls sideways by mSubBgScrollX to reveal the
 * second page. See notes/minigame-provenance.md for how the layout below was
 * recovered. */
struct dScMiniGm_c : dScene_c {
    s32 mSubBgScrollX;           /* 0x050 */
    u8  mPageFlipped;            /* 0x054 */
    u8  pad_055[0x3];
    s32 mGroupBase;              /* 0x058 */
    s32 unk_05c;                 /* 0x05c */
    s32 unk_060;                 /* 0x060 */
    s32 unk_064;                 /* 0x064 */
    u8  pad_068[0x24];  /* 0x068 -- opaque; untouched by any slot fn */
    s32 mArrowBobPhase;          /* 0x08c */
    s32 mPrevPageTimer;          /* 0x090 */
    s32 mNextPageTimer;          /* 0x094 */
    s32 mExitTimer;              /* 0x098 */
    s32 mIconBlinkPhase;         /* 0x09c */
    s32 mScrollDelay;            /* 0x0a0 */
    s32 unk_0a4;                 /* 0x0a4 */
    s32 unk_0a8;                 /* 0x0a8 */
    u8  mExiting;                /* 0x0ac */
    u8  pad_0ad[0x3];

    /* Declared first -- key function; see the family convention discussed
       in dScene_c.h. Never defined as a real method in any TU: both D1 and
       D0 are plain functions carrying their literal mangled name
       (src/_ZN11dScMiniGm_cD1Ev.c, src/_ZN11dScMiniGm_cD0Ev.c). */
    virtual ~dScMiniGm_c();                              /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScMiniGm_c_size_must_be_0xb0[sizeof(dScMiniGm_c) == 0xb0 ? 1 : -1];

#endif
