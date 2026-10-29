#ifndef DAOBJKM2_GURA_C_H
#define DAOBJKM2_GURA_C_H

#include "types.h"
#include "daObjGuragura_c.h"

/**
 * daKpa_c in the Fire Sea's tilting slab. No fields of its own: the
 * factory allocates 0x350, which daObjGuragura_c fills. Overrides
 * the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers.
 *
 * `daObjKm2_Gura_c` is the RTTI name. `guragura` is the wobble.
 */
struct daObjKm2_Gura_c : daObjGuragura_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm2_Gura_c() {}
    s32 CleanupResources(); /* slot 3 */
    s32 InitResources();    /* slot 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm2_Gura_c_size_must_be_0x350[
    sizeof(daObjKm2_Gura_c) == 0x350 ? 1 : -1];
#endif

#endif /* DAOBJKM2_GURA_C_H */
