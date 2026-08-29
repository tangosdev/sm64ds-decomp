#ifndef DSCENTRY_C_H
#define DSCENTRY_C_H

#include "dScene_c.h"
#include "dGraph_c.h"
#include "dThIcon_c.h"

/* The "entry" scene -- whichever course or minigame is currently loaded, keyed
 * off the actor ID it was spawned with (fBase_c::actorID, inherited at 0x0c).
 * One of dScene_c's ten direct children; it adds no virtual of its own and
 * overrides eight of dScene_c's, destructor pair included.
 *
 * Size 0x288 is read straight off the factories' fBase_c::operator new call.
 * RTTI and the paired array construction/destruction prove the icon_c[9]
 * member at 0x70 derives from dThIcon_c. Its constructor, destructor and two
 * virtual slots are now native C++, so the compiler owns that array's reverse
 * destruction. The separate 0x1b4 array has no type evidence beyond four
 * trivial 0x2c-byte elements and remains explicitly opaque.
 *
 * RTTI also proves graphCallback_c derives from dGraph_c::callback_c, but it
 * is not the 0x1b4 array: its constructor operates on the separate 0x2c-byte
 * global at ov075:0x0211d71c. Keeping both types distinct corrects the older
 * coincidental-size inference in notes/scene-provenance.md.
 *
 * Real class-form methods use this header. Method-local offset views remain
 * where naming the full opaque object layout would add unsupported claims.
 */
struct dScEntry_c : dScene_c {
    struct icon_c : dThIcon_c {
        icon_c();
        ~icon_c();
        virtual void Behavior();
        virtual void Render();
    };

    struct graphCallback_c;

    u8  unk_050[0x20];           /* 0x050 */
    icon_c mIcons[9];            /* 0x070..0x1b4 */
    u8  mUnkArray[4][0x2c];      /* 0x1b4..0x264 -- distinct trivial type */
    u8  unk_264[0x1f];           /* 0x264 */
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

struct dScEntry_c::graphCallback_c : dGraph_c::callback_c {
    void *compressedBg2Screen; /* 0x04 */
    void *entryScene;          /* 0x08 */
    s32 bg2Priority;           /* 0x0c */
    u8 unk_010[0x1c];          /* 0x10 */

    graphCallback_c();
    virtual int GraphCallback2();
};

typedef char dScEntry_c_size_must_be_0x288[sizeof(dScEntry_c) == 0x288 ? 1 : -1];
typedef char icon_c_size_must_be_0x24[
    sizeof(dScEntry_c::icon_c) == 0x24 ? 1 : -1];
typedef char dScEntry_graphCallback_c_size_must_be_0x2c[
    sizeof(dScEntry_c::graphCallback_c) == 0x2c ? 1 : -1];

#endif
