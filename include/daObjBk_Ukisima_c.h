#ifndef DAOBJBK_UKISIMA_C_H
#define DAOBJBK_UKISIMA_C_H

#include "types.h"
#include "daObjKaitendai_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Whomp's Fortress spinning disc. No fields of its own:
 * sizeof(daObjBk_Ukisima_c) == 0x320 == sizeof(dBgActor_c).
 * Overrides the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers.
 *
 * `daObjBk_Ukisima_c` is the RTTI name.
 */
struct daObjBk_Ukisima_c : daObjKaitendai_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBk_Ukisima_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjBk_Ukisima_c_size_must_be_0x320[
    sizeof(daObjBk_Ukisima_c) == 0x320 ? 1 : -1];

#endif /* DAOBJBK_UKISIMA_C_H */
