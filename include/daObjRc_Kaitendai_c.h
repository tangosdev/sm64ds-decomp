#ifndef DAOBJRC_KAITENDAI_C_H
#define DAOBJRC_KAITENDAI_C_H

#include "types.h"
#include "daObjKaitendai_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Rainbow Ride's spinning disc. No fields of its own: the factory
 * allocates 0x320 = sizeof(dBgActor_c), which daObjKaitendai_c fills.
 * Overrides the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers. Start angle is one of two
 * statics, chosen by param1.
 *
 * `daObjRc_Kaitendai_c` is the RTTI name. Coined name was RotatingPlatformRr.
 */
struct daObjRc_Kaitendai_c : daObjKaitendai_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjRc_Kaitendai_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjRc_Kaitendai_c_size_must_be_0x320[
    sizeof(daObjRc_Kaitendai_c) == 0x320 ? 1 : -1];

#endif /* DAOBJRC_KAITENDAI_C_H */
