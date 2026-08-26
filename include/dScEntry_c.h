#ifndef DSCENTRY_C_H
#define DSCENTRY_C_H

#include "dScene_c.h"

/* The "entry" scene -- whichever course or minigame is currently loaded, keyed
 * off the actor ID it was spawned with (fBase_c::actorID, inherited at 0x0c).
 * One of dScene_c's ten direct children; it adds no virtual of its own and
 * overrides eight of dScene_c's, destructor pair included.
 *
 * Size 0x288 is read straight off the constructor's fBase_c::operator new
 * call. Most of 0x50..0x288 is still opaque storage: it holds a
 * dScEntry_c::icon_c[9] array at 0x70 and a dScEntry_c::graphCallback_c[4]
 * array at 0x1b4, whose own layouts are a later pass. Derivation and the
 * constructor/destructor evidence for those two ranges:
 * notes/scene-provenance.md.
 *
 * Real class-form methods use this header. Method-local offset views remain
 * where naming the full opaque object layout would add unsupported claims.
 */
struct dScEntry_c : dScene_c {
    class graphCallback_c;

    u8  unk_050[0x30];  /* 0x050 -- opaque; icon_c[9] runs 0x070..0x1b4 */
    s32 unk_080;                  /* 0x080 */
    u8  unk_084[0x1c];  /* 0x084 -- opaque; graphCallback_c[4] runs to 0x264 */
    s16 unk_0a0;                  /* 0x0a0 */
    u8  unk_0a2[0x2];  /* 0x0a2 */
    u16 unk_0a4;                  /* 0x0a4 */
    u8  unk_0a6[0x1dd];  /* 0x0a6 -- opaque */
    u8  mFadeBrightness;           /* 0x283 -- driven into the sub engine's MASTER_BRIGHT */
    u8  mFadeTick;                 /* 0x284 -- steps the fade every other frame */
    u8  mIsFading;                 /* 0x285 -- while set, Behavior runs the fade and nothing else */
    u8  pad_286;                   /* 0x286 */
    u8  unk_287;                   /* 0x287 */

    /* Declared first: the destructor is the class's key function. */
    virtual ~dScEntry_c();                              /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                       /* slot  0 */
    virtual bool BeforeInitResources();                 /* slot  1 */
    virtual s32  CleanupResources();                    /* slot  3 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual void OnPendingDestroy();                     /* slot 12 */
};

typedef char dScEntry_c_size_must_be_0x288[sizeof(dScEntry_c) == 0x288 ? 1 : -1];

#endif
