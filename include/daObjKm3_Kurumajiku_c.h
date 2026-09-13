#ifndef DAOBJKM3_KURUMAJIKU_C_H
#define DAOBJKM3_KURUMAJIKU_C_H

#include "types.h"
#include "daObjKurumajiku_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Sky's rickshaw axle (`kurumajiku`). No fields of
 * its own: the factory allocates 0x330, which daObjKurumajiku_c
 * fills (four mounted-actor IDs at 0x320). Overrides the two slots
 * the base leaves null (InitResources, CleanupResources) and hands
 * this overlay's model/collision descriptor to the shared ov002
 * helpers, plus the cart actor id KM3_KURUMA.
 *
 * `daObjKm3_Kurumajiku_c` is the RTTI name. The cart in this overlay
 * is daObjKm3_Kuruma_c.
 */
struct daObjKm3_Kurumajiku_c : daObjKurumajiku_c {
    /* Inline empty dtor: mwccarm emits D1 then D0. */
    virtual ~daObjKm3_Kurumajiku_c() {}
    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm3_Kurumajiku_c_size_must_be_0x330[
    sizeof(daObjKm3_Kurumajiku_c) == 0x330 ? 1 : -1];

#endif /* DAOBJKM3_KURUMAJIKU_C_H */
