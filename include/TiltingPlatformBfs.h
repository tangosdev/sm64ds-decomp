#ifndef TILTINGPLATFORMBFS_H
#define TILTINGPLATFORMBFS_H

#include "types.h"

/* Bowser in the Fire Sea's tilting slab.
 *
 * IT DOES NOT DERIVE FROM Platform. It derives from daObjGuragura_c, which derives from
 * Platform, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjGuragura_c's, then Platform's.
 * A one-level chain emits two.
 *
 *   _ZTI15daObjKm2_Gura_c  ov045 0x02112fd0
 *   _ZTS15daObjKm2_Gura_c  ov045 0x02112fe8
 *   _ZTV18TiltingPlatformBfs  ov045 0x02113020  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjGuragura_c, ov002 0x0210905c
 *
 * NO FIELDS OF ITS OWN: TiltingPlatformBfs_Spawn passes 848 = 0x350, which
 * daObjGuragura_c fills. It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjGuragura_c.h"

struct TiltingPlatformBfs : daObjGuragura_c {
    /* --- vtable --- */
    virtual ~TiltingPlatformBfs(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char TiltingPlatformBfs_size_must_be_0x350[sizeof(TiltingPlatformBfs) == 0x350 ? 1 : -1];

#endif /* __cplusplus */

#endif /* TILTINGPLATFORMBFS_H */
