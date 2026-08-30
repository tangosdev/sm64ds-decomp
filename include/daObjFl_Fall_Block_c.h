#ifndef DAOBJFL_FALL_BLOCK_C_H
#define DAOBJFL_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

/* The Lethal Lava Land falling block. Only ever named by its factory,
 * FallBlockLll_Spawn ("Lll" for Lethal Lava Land) -- distinct from the
 * already-real daObjFallBlock_c (ov098), which is this class's base, not
 * this class itself.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFallBlock_c, which
 * derives from dBgActor_c, and the difference is in the bytes rather than only
 * in the RTTI: its destructor stores THREE vptrs -- its own,
 * daObjFallBlock_c's, then dBgActor_c's (func_ov022_02112380 /
 * func_ov022_021123d0, both `complete` today under placeholder names). A
 * one-level chain emits two. Its siblings include/FallBlockWf.h (ov015),
 * FallBlockBfs.h (ov045) and FallBlockBbh.h (ov063) already document the
 * identical shape.
 *
 *   _ZTI20daObjFl_Fall_Block_c  ov022 0x02114270
 *   _ZTS20daObjFl_Fall_Block_c  ov022 0x02114288
 *   _ZTV20daObjFl_Fall_Block_c  ov022 0x021142c4  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFallBlock_c, ov015 0x02114874 (typeinfo record), vtable in ov098
 *
 * NO FIELDS OF ITS OWN: FallBlockLll_Spawn passes 844 = 0x34c, which
 * daObjFallBlock_c already fills exactly. It overrides slots 0 and 3, which
 * the base leaves null.
 */

#ifdef __cplusplus

struct daObjFl_Fall_Block_c : daObjFallBlock_c {
    /* --- vtable --- */
    virtual ~daObjFl_Fall_Block_c() {}       /* slots 16 (D1), 17 (D0) */

    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */
};

typedef char daObjFl_Fall_Block_c_size_must_be_0x34c[sizeof(daObjFl_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_FALL_BLOCK_C_H */
