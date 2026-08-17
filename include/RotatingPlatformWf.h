#ifndef ROTATINGPLATFORMWF_H
#define ROTATINGPLATFORMWF_H

#include "types.h"

/* Bob-omb Battlefield's spinning disc.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKaitendai_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI17daObjBk_Ukisima_c  ov015 0x02114798
 *   _ZTS17daObjBk_Ukisima_c  ov015 0x021147b0
 *   _ZTV18RotatingPlatformWf  ov015 0x021147e8  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN: RotatingPlatformWf_Spawn passes 800 = 0x320 = sizeof(dBgActor_c).
 * It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct RotatingPlatformWf : daObjKaitendai_c {
    /* --- vtable --- */
    virtual ~RotatingPlatformWf(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RotatingPlatformWf_size_must_be_0x320[sizeof(RotatingPlatformWf) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROTATINGPLATFORMWF_H */
