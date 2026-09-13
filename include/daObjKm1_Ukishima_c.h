#ifndef DAOBJKM1_UKISHIMA_C_H
#define DAOBJKM1_UKISHIMA_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* BitDW floating island (profile KM1_UKISHIMA / DIAMOND_LIFT 135).
 * ROM RTTI daObjKm1_Ukishima_c; direct base dBgActor_c (_ZTI+8 at
 * ov043 0x02112270 points at _ZTI10dBgActor_c). Factory allocates
 * 0x320 = sizeof(dBgActor_c); mTurnTimer lives in the base's tail
 * padding at 0x31e (same placement as daObjRc_Guruguru_c::mAngVelY).
 *
 * Vtable was diffed slot by slot against _ZTV10dBgActor_c. Only the
 * slots declared below differ; every other slot holds the base's own
 * word and is inherited.
 */
struct daObjKm1_Ukishima_c : dBgActor_c {
    /* InitResources seeds 0x3c; Behavior counts it down and turns the
       island a quarter turn each time it reaches zero. */
    u8 mTurnTimer;            /* 0x31e */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjKm1_Ukishima_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    /* size_t == unsigned long here; unsigned int is illegal. Forwards
       fBase_c::operator new until #2570 merges. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjKm1_Ukishima_c_size_must_be_0x320[sizeof(daObjKm1_Ukishima_c) == 0x320 ? 1 : -1];

#endif /* DAOBJKM1_UKISHIMA_C_H */
