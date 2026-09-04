#ifndef DAOBJKM2_UKISHIMA_C_H
#define DAOBJKM2_UKISHIMA_C_H

#include "types.h"

/* Bowser in the Fire Sea's floating floor. `ukishima` is a floating island.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjUkiyuka_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjUkiyuka_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI19daObjKm2_Ukishima_c  ov045 0x02112efc
 *   _ZTS19daObjKm2_Ukishima_c  ov045 0x02112f14
 *   _ZTV19daObjKm2_Ukishima_c  ov045 0x02112f50  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjUkiyuka_c, ov002 0x02109104
 *
 * NO FIELDS OF ITS OWN, and this is the leaf that fixes the base's size:
 * daObjKm2_Ukishima_c_classInit (historical alias FloatingFloorBfs_Spawn)
 * passes 812 = 0x32c, which is daObjUkiyuka_c exactly. It overrides slots 0
 * and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjUkiyuka_c.h"

struct daObjKm2_Ukishima_c : daObjUkiyuka_c {
    /* --- vtable --- */
    virtual ~daObjKm2_Ukishima_c();   /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char FloatingFloorBfs_size_must_be_0x32c[sizeof(daObjKm2_Ukishima_c) == 0x32c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM2_UKISHIMA_C_H */
