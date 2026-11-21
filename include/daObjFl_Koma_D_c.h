#ifndef DAOBJFL_KOMA_D_C_H
#define DAOBJFL_KOMA_D_C_H

#include "types.h"

#ifdef __cplusplus

#include "daObjKaitendai_c.h"

/**
 * Lethal Lava Land's spinning disc. No fields of its own:
 * sizeof(daObjFl_Koma_D_c) == 0x320 == sizeof(dBgActor_c).
 * Overrides the two slots the base leaves null.
 *
 * `daObjFl_Koma_D_c` is the RTTI name. The destructor is defined
 * out of line in the actor translation unit.
 */
struct daObjFl_Koma_D_c : daObjKaitendai_c {
    virtual ~daObjFl_Koma_D_c(); /* slots 16 (D1), 17 (D0) */

    virtual int CleanupResources(); /* slot  3 */
    virtual int InitResources();    /* slot  0 */
};

typedef char daObjFl_Koma_D_c_size_must_be_0x320[sizeof(daObjFl_Koma_D_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_KOMA_D_C_H */
