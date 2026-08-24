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

    /* Where it started; InitResources copies the actor's position in here.
       MEASURED SIZE, do not shrink to the field span: the two factories call
       fBase_c::operator new(0x330), read off the retail instruction, and a span
       is only a lower bound. These three close it exactly. */
    s32 mBasePosX;                    /* 0x324 */
    s32 mBasePosY;                    /* 0x328 */
    s32 mBasePosZ;                    /* 0x32c */
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
    u8  pad_323[0x1];
    s32 mBasePosX;            /* 0x324 */
    s32 mBasePosY;            /* 0x328 */
    s32 mBasePosZ;            /* 0x32c */
};

typedef char SlidingPlatformWf_C_size_must_be_0x330[sizeof(struct SlidingPlatformWf) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* SLIDINGPLATFORMWF_H */
