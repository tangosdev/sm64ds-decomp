#ifndef ROTATINGPLATFORMRR_H
#define ROTATINGPLATFORMRR_H

#include "types.h"

/* Rainbow Ride's spinning disc.
 *
 * IT DOES NOT DERIVE FROM Platform. It derives from daObjKaitendai_c, which derives from
 * Platform, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then Platform's.
 * A one-level chain emits two.
 *
 *   _ZTI19daObjRc_Kaitendai_c  ov036 0x02113b20
 *   _ZTS19daObjRc_Kaitendai_c  ov036 0x02113b38
 *   _ZTV18RotatingPlatformRr  ov036 0x02113b74  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN: RotatingPlatformRr_Spawn passes 800 = 0x320 = sizeof(Platform).
 * It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct RotatingPlatformRr : daObjKaitendai_c {
    /* --- vtable --- */
    virtual ~RotatingPlatformRr(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RotatingPlatformRr_size_must_be_0x320[sizeof(RotatingPlatformRr) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROTATINGPLATFORMRR_H */
