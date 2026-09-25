#ifndef DAOBJKM2_AGARU_C_H
#define DAOBJKM2_AGARU_C_H

#include "types.h"
#include "dBgActor_c.h"

/* Bowser in the Fire Sea rising platform (profile KM2_AGARU; agaru is
 * "to go up"). ROM RTTI daObjKm2_Agaru_c; direct base dBgActor_c.
 * Factory allocates 0x328, and mState closes exactly on it.
 *
 * The vtable was diffed slot by slot against _ZTV10dBgActor_c; only the
 * slots declared below differ, every other slot is the base's own word.
 *
 * daObjKm2_Agaru_c_classInit is reconstructed (RTTI daObjKm2_Agaru_c,
 * KM2_AGARU registry). Retail does not store that spelling. Historical
 * aliases: FireSeaElevator (the class), FireSeaElevator_Spawn.
 */
struct daObjKm2_Agaru_c : dBgActor_c {
    s32 mRestY;       /* 0x320 -- spawn height; the ride tops out 1500.0 above it */
    u16 mWaitTimer;   /* 0x324 -- 20-frame pause before each leg */
    u8  mStoodOn;     /* 0x326 -- set by the mesh callback, cleared every Behavior */
    u8  mState;       /* 0x327 -- 0 idle, 1 rising, 2 sinking */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daObjKm2_Agaru_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm2_Agaru_c_size_must_be_0x328[
    sizeof(daObjKm2_Agaru_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJKM2_AGARU_C_H */
