#ifndef RICKSHAWBS_H
#define RICKSHAWBS_H

#include "types.h"

/* Bowser in the Sky's spinning disc.
 *
 * IT DOES NOT DERIVE FROM Platform. It derives from daObjKaitendai_c, which derives from
 * Platform, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjKaitendai_c's, then Platform's.
 * A one-level chain emits two.
 *
 *   _ZTI20daObjKm3_Kaitendai_c  ov047 0x02112328
 *   _ZTS20daObjKm3_Kaitendai_c  ov047 0x02112340
 *   _ZTV10RickshawBs  ov047 0x0211237c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjKaitendai_c, ov002 0x021091ac
 *
 * NO FIELDS OF ITS OWN, and the factory that says so is NOT the one named after this
 * class. `func_ov047_021113bc`, still unnamed, is the function that allocates 800 =
 * 0x320 and stores this class's vtable second. RickshawBs_Spawn allocates 816 and
 * builds daObjKm3_Kurumajiku_c: the ov047 "Bs" names are crossed, which #1521
 * recorded in include/RickshawPlatformBs.h, and untangling them is a config change
 * and its own piece of work. It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

struct RickshawBs : daObjKaitendai_c {
    /* --- vtable --- */
    virtual ~RickshawBs();         /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RickshawBs_size_must_be_0x320[sizeof(RickshawBs) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* RICKSHAWBS_H */
