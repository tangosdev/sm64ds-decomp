#ifndef DAOBJKM1_KURUMAJIKU_C_H
#define DAOBJKM1_KURUMAJIKU_C_H

#include "types.h"
#include "daObjKurumajiku_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Dark World's rickshaw axle (`kurumajiku`). No fields of
 * its own: the factory allocates 0x330, which daObjKurumajiku_c
 * fills (four mounted-actor IDs at 0x320). Overrides the two slots
 * the base leaves null (InitResources, CleanupResources) and hands
 * this overlay's model/collision descriptor to the shared ov002
 * helpers, plus the cart actor id KM1_KURUMA.
 *
 * `daObjKm1_Kurumajiku_c` is the RTTI name. The cart in this overlay
 * is daObjKm1_Kuruma_c.
 */
struct daObjKm1_Kurumajiku_c : daObjKurumajiku_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm1_Kurumajiku_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm1_Kurumajiku_c_size_must_be_0x330[
    sizeof(daObjKm1_Kurumajiku_c) == 0x330 ? 1 : -1];

#endif /* DAOBJKM1_KURUMAJIKU_C_H */
