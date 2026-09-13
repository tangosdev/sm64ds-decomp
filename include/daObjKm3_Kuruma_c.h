#ifndef DAOBJKM3_KURUMA_C_H
#define DAOBJKM3_KURUMA_C_H

#include "types.h"
#include "daObjKuruma_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Sky's rickshaw cart (`kuruma`). No fields of its
 * own: the factory allocates 0x320 = sizeof(dBgActor_c), which
 * daObjKuruma_c fills. Overrides the two slots the base leaves null
 * (InitResources, CleanupResources) and hands this overlay's
 * model/collision descriptor to the shared ov002 helpers.
 *
 * `daObjKm3_Kuruma_c` is the RTTI name. The axle in this overlay is
 * daObjKm3_Kurumajiku_c.
 */
struct daObjKm3_Kuruma_c : daObjKuruma_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm3_Kuruma_c() {}
    s32 CleanupResources();            /* slot  3 */
    s32 InitResources();               /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm3_Kuruma_c_size_must_be_0x320[
    sizeof(daObjKm3_Kuruma_c) == 0x320 ? 1 : -1];

#endif /* DAOBJKM3_KURUMA_C_H */
