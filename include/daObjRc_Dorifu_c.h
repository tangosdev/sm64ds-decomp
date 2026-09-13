#ifndef DAOBJRC_DORIFU_C_H
#define DAOBJRC_DORIFU_C_H

#include "types.h"
#include "daObjDorifu_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Rainbow Ride's Tricky Triangles (`dorifu`). No fields of its
 * own: the factory allocates 0xdcc, which daObjDorifu_c fills
 * (five plank models at 0x320, five plank colliders at 0x4b0).
 * Overrides the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's five-entry file table
 * to the shared overloads.
 *
 * `daObjRc_Dorifu_c` is the RTTI name.
 */
struct daObjRc_Dorifu_c : daObjDorifu_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjRc_Dorifu_c() {}
    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjRc_Dorifu_c_size_must_be_0xdcc[
    sizeof(daObjRc_Dorifu_c) == 0xdcc ? 1 : -1];

#endif /* DAOBJRC_DORIFU_C_H */
