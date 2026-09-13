#ifndef DAOBJKM1_DORIFU_C_H
#define DAOBJKM1_DORIFU_C_H

#include "types.h"
#include "daObjDorifu_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Dark World's drifting stairs (`dorifu`). No fields of
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
    s32 CleanupResources();                /* slot  3 */
    s32 InitResources();                   /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm1_Dorifu_c_size_must_be_0xdcc[
    sizeof(daObjKm1_Dorifu_c) == 0xdcc ? 1 : -1];

#endif /* DAOBJKM1_DORIFU_C_H */
