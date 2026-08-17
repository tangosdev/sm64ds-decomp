#ifndef FALLBLOCKWF_H
#define FALLBLOCKWF_H

#include "types.h"

/* Bob-omb Battlefield's falling block.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFallBlock_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFallBlock_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI20daObjBk_Fall_Block_c  ov015 0x02114868
 *   _ZTS20daObjBk_Fall_Block_c  ov015 0x021148a0
 *   _ZTV11FallBlockWf  ov015 0x021148dc  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFallBlock_c, ov015 0x02114874
 *
 * NO FIELDS OF ITS OWN: FallBlockWf_Spawn passes 844 = 0x34c, which daObjFallBlock_c
 * fills exactly. It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjFallBlock_c.h"

struct FallBlockWf : daObjFallBlock_c {
    /* --- vtable --- */
    virtual ~FallBlockWf();        /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char FallBlockWf_size_must_be_0x34c[sizeof(FallBlockWf) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* FALLBLOCKWF_H */
