#ifndef DAOBJKM1_KURUMA_C_H
#define DAOBJKM1_KURUMA_C_H

#include "types.h"
#include "daObjKuruma_c.h"

/* The Bob-omb Battlefield rickshaw platform -- the cart on the axle. ROM name
 * daObjKm1_Kuruma_c.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKuruma_c, which derives
 * from dBgActor_c. _ZN17daObjKm1_Kuruma_cD1Ev stores three vptrs, not two.
 *
 *   _ZTI17daObjKm1_Kuruma_c   ov043 0x0211240c
 *   _ZTS17daObjKm1_Kuruma_c   ov043 0x02112424
 *   _ZTV17daObjKm1_Kuruma_c ov043 0x0211245c  (its record sits at V-4)
 *   kind                      __si_class_type_info, ONE base, offset 0
 *   base                      daObjKuruma_c, ov002 0x02109254
 *
 * NO FIELDS ANYWHERE IN THE CHAIN ABOVE dBgActor_c.
 * daObjKm1_Kuruma_c_classInit (historical alias daObjKm1_Kuruma_c_Spawn)
 * passes 800 = 0x320 = sizeof(dBgActor_c); there is no room for one. This
 * class overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

struct daObjKm1_Kuruma_c : daObjKuruma_c {
    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjKm1_Kuruma_c() {}    /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjKm1_Kuruma_c_size_must_be_0x320[sizeof(daObjKm1_Kuruma_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM1_KURUMA_C_H */
