#ifndef ROTATINGPLATFORMLLL_H
#define ROTATINGPLATFORMLLL_H

#include "types.h"

/* Lethal Lava Land's spinning disc. `koma` is a spinning top.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKaitendai_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI16daObjFl_Koma_D_c  ov022 0x02113d98
 *   _ZTS16daObjFl_Koma_D_c  ov022 0x02113db0
 *   _ZTV19RotatingPlatformLll  ov022 0x02113de8  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN: RotatingPlatformLll_Spawn passes 800 = 0x320 = sizeof(dBgActor_c).
 * It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct RotatingPlatformLll : daObjKaitendai_c {
    /* --- vtable --- */
    virtual ~RotatingPlatformLll(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RotatingPlatformLll_size_must_be_0x320[sizeof(RotatingPlatformLll) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROTATINGPLATFORMLLL_H */
