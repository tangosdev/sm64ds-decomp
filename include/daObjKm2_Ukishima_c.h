#ifndef DAOBJKM2_UKISHIMA_C_H
#define DAOBJKM2_UKISHIMA_C_H

#include "types.h"
#include "daObjUkiyuka_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bowser in the Fire Sea's floating floor. No fields of its own: the
 * factory allocates 0x32c, which daObjUkiyuka_c fills. Overrides the
 * two slots the base leaves null (InitResources, CleanupResources)
 * and hands this overlay's model/collision descriptor to the shared
 * ov002 helpers. Init also stores kBobAmplitude into mBobAmplitude.
 *
 * `daObjKm2_Ukishima_c` is the RTTI name. `ukishima` is a floating
 * island.
 */
struct daObjKm2_Ukishima_c : daObjUkiyuka_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjKm2_Ukishima_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjKm2_Ukishima_c_size_must_be_0x32c[
    sizeof(daObjKm2_Ukishima_c) == 0x32c ? 1 : -1];

#endif /* DAOBJKM2_UKISHIMA_C_H */
