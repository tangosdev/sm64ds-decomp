#ifndef DAOBJRC_KAITENDAI_C_H
#define DAOBJRC_KAITENDAI_C_H

#include "types.h"

/* Rainbow Ride's spinning disc.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKaitendai_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI19daObjRc_Kaitendai_c  ov036 0x02113b20
 *   _ZTS19daObjRc_Kaitendai_c  ov036 0x02113b38
 *   _ZTV19daObjRc_Kaitendai_c  ov036 0x02113b74  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN: daObjRc_Kaitendai_c_classInit (historical alias
 * RotatingPlatformRr_Spawn) passes 800 = 0x320 = sizeof(dBgActor_c). It
 * overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct daObjRc_Kaitendai_c : daObjKaitendai_c {
    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjRc_Kaitendai_c() {}   /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RotatingPlatformRr_size_must_be_0x320[sizeof(daObjRc_Kaitendai_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJRC_KAITENDAI_C_H */
