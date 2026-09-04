#ifndef DAOBJKM1_UKISHIMA_C_H
#define DAOBJKM1_UKISHIMA_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjKm1_Ukishima_c_classInit (historical alias DiamondLift_Spawn)
 *                 fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV19daObjKm1_Ukishima_c,
 *                 then the members below in this order.
 *   ~daObjKm1_Ukishima_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjKm1_Ukishima_c : dBgActor_c {
    /* THIS CLASS'S OWN, in dBgActor_c's TAIL PADDING: the base's last field ends
       at 0x31e and its size rounds to 0x320, so the Itanium ABI puts the first
       derived byte at 0x31e and sizeof stays 0x320 (same placement as
       daObjRc_Guruguru_c::mAngVelY). InitResources seeds it with 0x3c and
       Behavior counts it down, turning the island a quarter turn each time it
       reaches zero. */
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
};

typedef char daObjKm1_Ukishima_c_size_must_be_0x320[sizeof(daObjKm1_Ukishima_c) == 0x320 ? 1 : -1];

#endif /* DAOBJKM1_UKISHIMA_C_H */
