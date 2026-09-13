#ifndef DAOBJRC_GURUGURU_C_H
#define DAOBJRC_GURUGURU_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Rainbow Ride armed rotating platform (ARMED_ROTATING_PLATFORM 132,
 * profile RC_GURUGURU). ov036 is mixed (tricky triangles / ship wing /
 * donut block / swinging platform / ROTATING_PLATFORM_RR / flying carpet);
 * this is the spinning platform, not kaitendai/carpet/buranko/hane.
 *
 * RTTI ov036:0x02113d90 spells daObjRc_Guruguru_c. _ZTI+8 at 0x02113d84
 * points at _ZTI10dBgActor_c, so dBgActor_c is the direct and only base.
 * SIZE 0x320 is the factory's own literal: one s16 in dBgActor_c's tail
 * padding at 0x31e, and sizeof stays 0x320.
 *
 * Historical alias: ArmedRotatingPlatform. The coined name that sat on
 * vtable 0x02113dcc is gone; the ROM type string is the class name.
 *
 * Vtable diff against _ZTV10dBgActor_c: only slots 0, 3, 6, 9, 16 and 17
 * differ. Every other slot holds the base's word and is inherited.
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

    /* size_t == unsigned long on this ABI; unsigned int is illegal.
       A plain `new` without this relocates to the global `_Znwm`. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjRc_Guruguru_c_size_must_be_0x320[sizeof(daObjRc_Guruguru_c) == 0x320 ? 1 : -1];

#endif /* DAOBJRC_GURUGURU_C_H */
