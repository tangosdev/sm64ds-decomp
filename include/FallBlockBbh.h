#ifndef FALLBLOCKBBH_H
#define FALLBLOCKBBH_H

#include "types.h"

/* Big Boo's Haunt's falling block.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFallBlock_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFallBlock_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI20daObjTh_Fall_Block_c  ov063 0x0211eb04
 *   _ZTS20daObjTh_Fall_Block_c  ov063 0x0211eb1c
 *   _ZTV12FallBlockBbh  ov063 0x0211eb58  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFallBlock_c, ov015 0x02114874
 *
 * NO FIELDS OF ITS OWN: daObjTh_Fall_Block_c_classInit passes 844 = 0x34c, which daObjFallBlock_c
 * fills exactly. It overrides slots 0 and 3, which the base leaves null.
 *
 * NOT ONE OF THE FLATTENED ROWS. tools/rtti_reconcile.py never graded this class,
 * because the tree held no belief about its base at all rather than the wrong one.
 * It is here because it is the fourth leaf of the same intermediate and its
 * destructor stores the same three vptrs.
 */

#ifdef __cplusplus

#include "daObjFallBlock_c.h"

struct FallBlockBbh : daObjFallBlock_c {
    /* --- vtable --- */
    virtual ~FallBlockBbh();       /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char FallBlockBbh_size_must_be_0x34c[sizeof(FallBlockBbh) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* FALLBLOCKBBH_H */
