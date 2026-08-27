#ifndef DAOBJKM3_KURUMA_C_H
#define DAOBJKM3_KURUMA_C_H

#include "types.h"
#include "daObjKuruma_c.h"

/* Bowser in the Sky's rickshaw platform -- the cart on the axle. Its factory is
 * named RickshawPlatformBs_Spawn, and that name is trustworthy for pairing (unlike
 * its sibling RickshawBs_Spawn, see include/daObjKm3_Kurumajiku_c.h); still paired
 * here by vtable ADDRESS per the recovery recipe, and it agrees with the name.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKuruma_c, which derives
 * from dBgActor_c. Its destructor stores THREE vptrs (func_ov047_021113f8 /
 * func_ov047_02111448, both `complete` today under placeholder names).
 *
 *   _ZTI17daObjKm3_Kuruma_c  ov047 0x021123fc
 *   _ZTS17daObjKm3_Kuruma_c  ov047 0x02112414
 *   _ZTV17daObjKm3_Kuruma_c  ov047 0x0211244c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKuruma_c, ov002 0x02109254
 *
 * NO FIELDS ANYWHERE IN THE CHAIN ABOVE dBgActor_c. RickshawPlatformBs_Spawn passes
 * 800 = 0x320 = sizeof(dBgActor_c); there is no room for one. This class overrides
 * slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

struct daObjKm3_Kuruma_c : daObjKuruma_c {
    /* --- vtable --- */
    virtual ~daObjKm3_Kuruma_c() {}      /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjKm3_Kuruma_c_size_must_be_0x320[sizeof(daObjKm3_Kuruma_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM3_KURUMA_C_H */
