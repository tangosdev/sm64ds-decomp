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
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~daObjFl_Fall_Block_c() {}   /* no slot */
#else
    virtual ~daObjFl_Fall_Block_c() {}   /* D1 and D0 */
#endif
    int CleanupResources(); /* slot 3 */
    int InitResources();    /* slot 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Fall_Block_c_size_must_be_0x34c[sizeof(daObjFl_Fall_Block_c) == 0x34c ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJFL_FALL_BLOCK_C_H */
