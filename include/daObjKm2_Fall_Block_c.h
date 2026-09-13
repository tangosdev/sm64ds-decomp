#ifndef DAOBJKM2_FALL_BLOCK_C_H
#define DAOBJKM2_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Fire Sea's falling block. No fields of its own: the
 * factory allocates 0x34c, which daObjFallBlock_c fills. Overrides
 * the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov098 helpers.
 *
 * `daObjKm2_Fall_Block_c` is the RTTI name.
 */
struct daObjKm2_Fall_Block_c : daObjFallBlock_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm2_Fall_Block_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm2_Fall_Block_c_size_must_be_0x34c[
    sizeof(daObjKm2_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* DAOBJKM2_FALL_BLOCK_C_H */
