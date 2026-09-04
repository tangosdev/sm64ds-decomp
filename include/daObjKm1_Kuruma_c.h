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

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjKm1_Kuruma_c : daObjKuruma_c {
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);

    /* MEASURED -- INLINE ON PURPOSE. Declaring this last makes the class TU
       emit D1 before D0, in the cartridge's order, without a leaf D2 body. */
    virtual ~daObjKm1_Kuruma_c() {}    /* slots 16 (D1), 17 (D0) */
};

inline void *daObjKm1_Kuruma_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKm1_Kuruma_c_size_must_be_0x320[sizeof(daObjKm1_Kuruma_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM1_KURUMA_C_H */
