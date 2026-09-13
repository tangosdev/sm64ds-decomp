#ifndef DAOBJWC_OBJ07_C_H
#define DAOBJWC_OBJ07_C_H

#include "types.h"
#include "daObjKaitendai_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Wet-Dry World's spinning disc. No fields of its own: the
 * factory allocates 0x320 = sizeof(dBgActor_c), which
 * daObjKaitendai_c fills. Overrides the two slots the base leaves
 * null (InitResources, CleanupResources) and hands this overlay's
 * model/collision descriptor to the shared ov002 helpers.
 *
 * `daObjWc_Obj07_c` is the RTTI name. Coined name was RotatingPlatformWdw.
 */
struct daObjWc_Obj07_c : daObjKaitendai_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjWc_Obj07_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjWc_Obj07_c_size_must_be_0x320[
    sizeof(daObjWc_Obj07_c) == 0x320 ? 1 : -1];

#endif /* DAOBJWC_OBJ07_C_H */
