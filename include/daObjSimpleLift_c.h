#ifndef DAOBJSIMPLELIFT_C_H
#define DAOBJSIMPLELIFT_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* A dBgActor_c: model at 0xd4, moving mesh collider at 0x124, clsn matrix
 * at 0x2ec, all inherited. Layout evidence: notes/platform-provenance.md.
 *
 * SM64DS proves this class as daObjSimpleLift_c through RTTI, allocation
 * size and vtable identity: the vtable at ov091:0x02135080 carries
 * _ZTI17daObjSimpleLift_c (0x02134f40) in its typeinfo word, and that
 * record's type-name string at 0x02134f4c reads "17daObjSimpleLift_c".
 * The factory and profile spellings below are reconstructed source-style
 * names -- evidence-bounded proposals, not recovered SM64DS symbols.
 *
 * daObjSimpleLift_c_classInit_BK_TRANSBAR at 0x021327e8 (historical alias
 * SlidingPlatformWf_Spawn) allocates 0x330 and installs this class's
 * cartridge vtable. It backs the BK_TRANSBAR registry profile, whose
 * descriptor at 0x02135008 is reconstructed as g_profile_BK_TRANSBAR. */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjSimpleLift_c : dBgActor_c {
    u8 mPauseTimer;                       /* 0x31e */
    u8  pad_31f[0x1];
    s16 mMoveTimer;                      /* 0x320 */
    u8 mVariant;                       /* 0x322 */

    /* --- vtable --- */
    virtual ~daObjSimpleLift_c();

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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSimpleLift_c_size_must_be_0x330[sizeof(daObjSimpleLift_c) == 0x330 ? 1 : -1];
#endif

#else

/* The same object spelled flat, for any C translation unit that needs the
   layout. The destructor this branch was originally written for is no longer
   one of them: both variants are emitted from the promoted C++ TU. The header
   has a single consumer today, src/actors/daObjSimpleLift_c.cpp, and it takes
   the C++ branch above. */
struct daObjSimpleLift_c {
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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSimpleLift_c_C_size_must_be_0x330[sizeof(struct daObjSimpleLift_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJSIMPLELIFT_C_H */
