#ifndef DAOBJKM1_DORIFU_C_H
#define DAOBJKM1_DORIFU_C_H

#include "types.h"
#include "daObjDorifu_c.h"

/**
 * daKpa_c in the Dark World's drifting stairs (`dorifu`). No fields of
 * its own: the factory allocates 0xdcc, which daObjDorifu_c fills
 * (mPlankModels[5] at 0x320, mPlankClsn[5] at 0x4b0).
 * Overrides the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's five-entry file table
 * to the shared overloads.
 *
 * `daObjKm1_Dorifu_c` is the RTTI name.
 */
struct daObjKm1_Dorifu_c : daObjDorifu_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm1_Dorifu_c() {}
    s32 CleanupResources(); /* slot  3 */
    s32 InitResources();    /* slot  0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKm1_Dorifu_c_size_must_be_0xdcc[sizeof(daObjKm1_Dorifu_c) == 0xdcc ? 1 : -1];
#endif

#endif /* DAOBJKM1_DORIFU_C_H */
