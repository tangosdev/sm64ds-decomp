#ifndef DAOBJBK_FALL_BLOCK_C_H
#define DAOBJBK_FALL_BLOCK_C_H

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
 *   _ZTV20daObjBk_Fall_Block_c  ov015 0x021148dc  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFallBlock_c, ov015 0x02114874
 *
 * NO FIELDS OF ITS OWN: daObjBk_Fall_Block_c_classInit passes 844 = 0x34c, which daObjFallBlock_c
 * fills exactly. It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjFallBlock_c.h"

struct daObjBk_Fall_Block_c : daObjFallBlock_c {
    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjBk_Fall_Block_c() {}       /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char daObjBk_Fall_Block_c_size_must_be_0x34c[sizeof(daObjBk_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJBK_FALL_BLOCK_C_H */
