#ifndef ROLLINGLOGLLL_H
#define ROLLINGLOGLLL_H

#include "types.h"

/* Lethal Lava Land's rolling log.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjMaruta_c, which derives
 * from dBgActor_c, and the difference is in the bytes rather than only in the RTTI:
 * its destructor stores THREE vptrs -- its own, daObjMaruta_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI15daObjFlMaruta_c  ov022 0x02114344
 *   _ZTS15daObjFlMaruta_c  ov022 0x0211436c
 *   _ZTV13RollingLogLll    ov022 0x021143a4  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjMaruta_c, ov022 0x02114350 -- whose vtable is in ov080, not ov022
 *
 * SIZE 0x344, from RollingLogLll_Spawn's literal 836. It overrides slots 0
 * (InitResources), 3 (CleanupResources) and 6 (Behavior), all three of which the
 * base leaves null; the base supplies only Render, the two destructors and
 * OnHitByMegaChar.
 *
 * WHAT THIS HEADER USED TO SAY: a `Model mModel` at 0xd4 and a
 * `u8 mMovingMeshCollider` marker at 0x124, over `u8 pad_000[0xd4]`. Both were
 * dBgActor_c's, twice removed, and are inherited now. None of this class's three
 * methods reads either by name -- Behavior reaches the collider as
 * `(char *)this + 0x124` -- so nothing in the tree changes with them.
 */

#ifdef __cplusplus

#include "daObjMaruta_c.h"

struct RollingLogLll : daObjMaruta_c {
    /* UNOBSERVED, not inherited. Nothing in the tree reads this span: neither this
       class's own three methods nor daObjMaruta_c's. It is here rather than on the
       base because the base's Behavior slot is null, so no method of the base
       could be reading it -- see include/daObjMaruta_c.h. */
    u8  pad_320[0x24];

    /* --- vtable --- */
    virtual ~RollingLogLll();          /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RollingLogLll_size_must_be_0x344[sizeof(RollingLogLll) == 0x344 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROLLINGLOGLLL_H */
