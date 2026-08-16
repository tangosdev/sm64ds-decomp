#ifndef RICKSHAWPLATFORMBDW_H
#define RICKSHAWPLATFORMBDW_H

#include "types.h"
#include "daObjKuruma_c.h"

/* The Bob-omb Battlefield rickshaw platform -- the cart on the axle. ROM name
 * daObjKm1_Kuruma_c.
 *
 * IT DOES NOT DERIVE FROM Platform. It derives from daObjKuruma_c, which derives
 * from Platform. _ZN19RickshawPlatformBdwD1Ev stores three vptrs, not two.
 *
 *   _ZTI17daObjKm1_Kuruma_c   ov043 0x0211240c
 *   _ZTS17daObjKm1_Kuruma_c   ov043 0x02112424
 *   _ZTV19RickshawPlatformBdw ov043 0x0211245c  (its record sits at V-4)
 *   kind                      __si_class_type_info, ONE base, offset 0
 *   base                      daObjKuruma_c, ov002 0x02109254
 *
 * NO FIELDS ANYWHERE IN THE CHAIN ABOVE Platform. RickshawPlatformBdw_Spawn
 * passes 800 = 0x320 = sizeof(Platform); there is no room for one. This class
 * overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

struct RickshawPlatformBdw : daObjKuruma_c {
    /* --- vtable --- */
    virtual ~RickshawPlatformBdw();    /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RickshawPlatformBdw_size_must_be_0x320[sizeof(RickshawPlatformBdw) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* RICKSHAWPLATFORMBDW_H */
