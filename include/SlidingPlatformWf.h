#ifndef SLIDINGPLATFORMWF_H
#define SLIDINGPLATFORMWF_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* A dBgActor_c: model at 0xd4, moving mesh collider at 0x124, clsn matrix at
 * 0x2ec, all inherited. Layout evidence: notes/platform-provenance.md. */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct SlidingPlatformWf : dBgActor_c {
    u8 mPauseTimer;                       /* 0x31e */
    u8  pad_31f[0x1];
    s16 mMoveTimer;                      /* 0x320 */
    u8 mVariant;                       /* 0x322 */

    /* --- vtable --- */
    virtual ~SlidingPlatformWf();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Tail padding. The field span stops short of the real size: SlidingPlatformBdw_Spawn and SlidingPlatformBfsRectangle_Spawn
       call fBase_c::operator new(0x330), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_324[0xc];      /* 0x324, to the ROM's 0x330 */
};

typedef char SlidingPlatformWf_size_must_be_0x330[sizeof(SlidingPlatformWf) == 0x330 ? 1 : -1];

#else

/* The same object spelled flat, for the compiler-generated destructor, which
   lives in a C translation unit and can never be migrated. */
struct SlidingPlatformWf {
    u8  pad_000[0xd4];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x32];
    u8  mPauseTimer;            /* 0x31e */
    u8  pad_31f[0x1];
    s16 mMoveTimer;            /* 0x320 */
    u8  mVariant;            /* 0x322 */
};

#endif /* __cplusplus */

#endif /* SLIDINGPLATFORMWF_H */
