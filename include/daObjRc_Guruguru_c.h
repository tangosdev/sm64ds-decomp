#ifndef DAOBJRC_GURUGURU_C_H
#define DAOBJRC_GURUGURU_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The RC_GURUGURU profile's powered turntable: a platform that simply spins
 * about its own yaw axis for as long as it is alive, carrying whatever stands
 * on it, and switches its collision mesh off once the actor is flagged.
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjRc_Guruguru_c_classInit (historical alias ArmedRotatingPlatform_Spawn)
 *                 fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(),
 *                 stores _ZTV18daObjRc_Guruguru_c.
 *   ~daObjRc_Guruguru_c   its own vptr, then dBgActor_c's -- inlined, because
 *                 dBgActor_c's destructor is defined in its class body -- then
 *                 dBgActor_c's dBgW_KcMbg at 0x124 and Model at 0xd4, then
 *                 dActor_c. All three are the BASE's; this class adds one s16,
 *                 which has no destructor of its own.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly
 * on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c at ov036
 * 0x02113dcc. Only the slots declared below differ; every other slot holds the
 * base's own word and is inherited, so it is deliberately not redeclared here.
 */
struct daObjRc_Guruguru_c : dBgActor_c {
    /* THIS CLASS'S OWN, in dBgActor_c's TAIL PADDING: the base's last field
       ends at 0x31e and its size rounds to 0x320, so the Itanium ABI puts the
       first derived byte at 0x31e and sizeof stays 0x320. InitResources seeds
       it with -0x80, or with mAngleZ when the level author set one, and
       Behavior adds it to mAngleY every frame -- that is the whole spin. */
    s16 mAngVelY;             /* 0x31e */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjRc_Guruguru_c() {}           /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char daObjRc_Guruguru_c_size_must_be_0x320[sizeof(daObjRc_Guruguru_c) == 0x320 ? 1 : -1];

#endif /* DAOBJRC_GURUGURU_C_H */
