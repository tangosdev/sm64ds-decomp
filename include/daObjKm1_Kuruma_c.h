#ifndef DAOBJKM1_KURUMA_C_H
#define DAOBJKM1_KURUMA_C_H

#include "types.h"
#include "daObjKuruma_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Dark World's rickshaw cart (`kuruma`). No fields of its
 * own: the factory allocates 0x320 = sizeof(dBgActor_c), which
 * daObjKuruma_c fills. Overrides the two slots the base leaves null
 * (InitResources, CleanupResources) and hands this overlay's
 * model/collision descriptor to the shared ov002 helpers.
 *
 * `daObjKm1_Kuruma_c` is the RTTI name. The axle in this overlay is
 * daObjKm1_Kurumajiku_c.
 */
struct daObjKm1_Kuruma_c : daObjKuruma_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm1_Kuruma_c() {}
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm1_Kuruma_c_size_must_be_0x320[
    sizeof(daObjKm1_Kuruma_c) == 0x320 ? 1 : -1];

#endif /* DAOBJKM1_KURUMA_C_H */
