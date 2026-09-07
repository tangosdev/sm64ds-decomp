#ifndef DAOBJRCBURANKO_C_H
#define DAOBJRCBURANKO_C_H

#include "types.h"

/* ov036 -- the Rainbow Cruise swinging platform (profile RC_BURANKO): a mesh
 * collider hung like a pendulum, swinging back and forth about its Z axis
 * forever.
 *
 * MEASURED from extracted/overlays/overlay_0036.bin, base 0x021111a0:
 *   _ZTS16daObjRcBuranko_c  0x02113a60  bytes spell "16daObjRcBuranko_c"
 *   _ZTI16daObjRcBuranko_c  0x02113a54  +4 -> _ZTS, +8 -> 0x021089ec = _ZTI10dBgActor_c
 *   _ZTV16daObjRcBuranko_c  0x02113a98  V-8 = 0, V-4 = &_ZTI  (the address point)
 *
 * THE DIRECT BASE IS dBgActor_c -- the RTTI base pointer says so, and the
 * 0x320 allocation in daObjRcBuranko_c_classInit agrees with dBgActor_c's own
 * 0x320: the one field below lives in the two tail bytes dBgActor_c rounds up
 * to, so the derived class costs nothing extra.
 *
 * Vtable diff against dBgActor_c: only slots 0, 3, 6, 9, 16 and 17 hold words
 * inside ov036. Every other slot of the cartridge's 32 still points into arm9,
 * so nothing else is overridden.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjRcBuranko_c : dBgActor_c {
    s16 mSwingVelZ;   /* 0x31e -- signed angular velocity added to mAngleZ each
                         frame; Behavior nudges it by +/-4 against the sign of
                         mAngleZ, which is what makes the platform a pendulum
                         rather than a turntable. It occupies the two bytes
                         dBgActor_c's own 0x31e..0x31f tail padding leaves. */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, and declared first, so this TU is the
       vtable's home. Both ROM bodies are empty; an inline body also drops the
       D2 variant the cartridge never carried. */
    virtual ~daObjRcBuranko_c() {}  /* slots 16 (D1), 17 (D0) */

    /* declared in reverse of ROM address order, as the TU emits them */
    int InitResources();      /* slot  0 -- 0x0211137c */
    int Behavior();           /* slot  6 -- 0x021112f0 */
    int Render();             /* slot  9 -- 0x021112c8 */
    int CleanupResources();   /* slot  3 -- 0x02111284 */
};

typedef char daObjRcBuranko_c_size_must_be_0x320[sizeof(daObjRcBuranko_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJRCBURANKO_C_H */
