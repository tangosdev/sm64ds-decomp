#ifndef DSCENTRY_C_H
#define DSCENTRY_C_H

#include "OamAnimation.h"
#include "dScene_c.h"
#include "dGraph_c.h"
#include "dThIcon_c.h"

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

    u8 unk_050[0x4];             /* 0x050 */
    Step mEnterStep;             /* 0x054 -- run as soon as a step set is installed */
    Step mBehaviorStep;          /* 0x05c -- Behavior runs it every frame */
    Step mRenderStep;            /* 0x064 -- Render runs it every frame */
    u8 pad_06c[0x4];             /* 0x06c */
    icon_c mIcons[9];            /* 0x070..0x1b4 */
    OamAnimation mOamAnimations[4]; /* 0x1b4..0x264 */
    u8 unk_264[0x1c];               /* 0x264 */
    u8 mIconCount;                  /* 0x280 -- live entries of mIcons */
    u8 pad_281[0x2];                /* 0x281 */
    u8  mFadeBrightness;           /* 0x283 -- driven into the sub engine's MASTER_BRIGHT */
    u8  mFadeTick;                 /* 0x284 -- steps the fade every other frame */
    u8  mIsFading;                 /* 0x285 -- while set, Behavior runs the fade and nothing else */
    u8  pad_286;                   /* 0x286 */
    u8  unk_287;                   /* 0x287 */

    /* Out of line: the key function, defined first in src/game/stages/d_s_entry.cpp. */
    virtual ~dScEntry_c();                              /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();                       /* slot  0 */
    virtual bool BeforeInitResources();                 /* slot  1 */
    virtual s32  CleanupResources();                    /* slot  3 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual void OnPendingDestroy();                     /* slot 12 */
};

struct dScEntry_c::graphCallback_c : dGraph_c::callback_c {
    void *compressedBg2Screen; /* 0x04 */
    void *entryScene;          /* 0x08 */
    s32 bg2Priority;           /* 0x0c */
    u8 unk_010[0x1c];          /* 0x10 */

    graphCallback_c();
    virtual int GraphCallback2();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScEntry_c_size_must_be_0x288[sizeof(dScEntry_c) == 0x288 ? 1 : -1];
typedef char dScEntry_c_Step_size_must_be_0x8[sizeof(dScEntry_c::Step) == 0x8 ? 1 : -1];
typedef char icon_c_size_must_be_0x24[
    sizeof(dScEntry_c::icon_c) == 0x24 ? 1 : -1];
typedef char dScEntry_oam_animation_size_must_be_0x2c[
    sizeof(OamAnimation) == 0x2c ? 1 : -1];
typedef char dScEntry_graphCallback_c_size_must_be_0x2c[
    sizeof(dScEntry_c::graphCallback_c) == 0x2c ? 1 : -1];
#endif

#endif
