#ifndef DAOBJFL_FALL_BLOCK_C_H
#define DAOBJFL_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

/**
 * Lethal Lava Land falling block. No fields of its own: the
 * factory allocates 0x34c, which daObjFallBlock_c fills. Overrides
 * the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov098 helpers.
 *
 * `daObjFl_Fall_Block_c` is the RTTI name.
 */

#ifdef __cplusplus

struct daObjFl_Fall_Block_c : daObjFallBlock_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjFl_Fall_Block_c() {}
    int CleanupResources(); /* slot 3 */
    int InitResources();    /* slot 0 */
};

typedef char daObjFl_Fall_Block_c_size_must_be_0x34c[sizeof(daObjFl_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_FALL_BLOCK_C_H */
