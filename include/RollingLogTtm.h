#ifndef ROLLINGLOGTTM_H
#define ROLLINGLOGTTM_H

#include "types.h"

/* Tall, Tall Mountain's rolling log. `hm` is the ROM's tag for that stage.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjMaruta_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjMaruta_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI15daObjHmMaruta_c  ov030 0x021159f8
 *   _ZTS15daObjHmMaruta_c  ov030 0x02115a10
 *   _ZTV13RollingLogTtm  ov030 0x02115a48  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjMaruta_c, ov022 0x02114350
 *
 * SIZE 0x344, from RollingLogTtm_Spawn's literal 836. It overrides slots 0, 3 and 6,
 * all three of which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjMaruta_c.h"

struct RollingLogTtm : daObjMaruta_c {
    /* UNOBSERVED, not inherited. Nothing in the tree reads this span: neither this
       class's own three methods nor daObjMaruta_c's. It is here rather than on the
       base because the base's Behavior slot is null, so no method of the base could
       be reading it -- see include/daObjMaruta_c.h. */
    u8  pad_320[0x24];
    /* --- vtable --- */
    virtual ~RollingLogTtm();      /* slots 16 (D1), 17 (D0) */

    int Behavior();                    /* slot  6 */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char RollingLogTtm_size_must_be_0x344[sizeof(RollingLogTtm) == 0x344 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROLLINGLOGTTM_H */
