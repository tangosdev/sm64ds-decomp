#ifndef DAOBJBSWDOOR_C_H
#define DAOBJBSWDOOR_C_H

#include "types.h"
#include "daObjSwdoor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bob-omb Battlefield's switch-operated shutter. ov014 SHUTTER_BOB (40).
 *
 * `daObjBSwdoor_c` is the RTTI name. It derives from daObjSwdoor_c, not
 * from dBgActor_c: the destructor stores three vptrs. Factory allocates
 * 0x324. Overrides the three slots the base leaves null (InitResources,
 * CleanupResources, Behavior) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers; Init also Enable()s the mesh.
 *
 * The four bytes at 0x320 are this leaf's: the ov002 setup helper writes
 * mTimer and mEventBit from param1. 0x31e/0x31f sit in dBgActor_c's tail
 * padding and are not declared here.
 */
struct daObjBSwdoor_c : daObjSwdoor_c {
    u8 mTimer;     /* 0x320 */
    u8 mEventBit;  /* 0x321 */
    u8 pad_322[2];

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBSwdoor_c() {}
    virtual s32 Behavior();           /* slot 6 */
    virtual s32 CleanupResources();   /* slot 3 */
    virtual s32 InitResources();      /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjBSwdoor_c_size_must_be_0x324[sizeof(daObjBSwdoor_c) == 0x324 ? 1 : -1];

#endif /* DAOBJBSWDOOR_C_H */
