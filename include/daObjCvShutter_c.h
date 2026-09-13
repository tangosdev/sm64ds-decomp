#ifndef DAOBJCVSHUTTER_C_H
#define DAOBJCVSHUTTER_C_H

#include "types.h"
#include "daObjSwdoor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Hazy Maze Cave's switch-operated shutter. ov021 SHUTTER_HMC (67).
 *
 * `daObjCvShutter_c` is the RTTI name (`cv` is the ROM's tag for that stage).
 * It derives from daObjSwdoor_c, not from dBgActor_c: the destructor stores
 * three vptrs. Factory allocates 0x324. Overrides the three slots the base
 * leaves null (InitResources, CleanupResources, Behavior) and hands this
 * overlay's model/collision descriptor to the shared ov002 helpers. Behavior
 * then re-syncs the collision mesh when IsClsnInRange says it is in range.
 *
 * The four bytes at 0x320 are daObjSwdoor_c base fields: the shared ov002
 * setup helper writes them for both leaves (PR #2579 types that helper as
 * daObjBSwdoor_c *). This leaf still declares mTimer/mEventBit here.
 * 0x31e/0x31f sit in dBgActor_c's tail padding and are not declared here.
 */
struct daObjCvShutter_c : daObjSwdoor_c {
    u8 mTimer;     /* 0x320 */
    u8 mEventBit;  /* 0x321 */
    u8 pad_322[2];

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjCvShutter_c() {}
    virtual s32 Behavior();           /* slot 6 */
    virtual s32 CleanupResources();   /* slot 3 */
    virtual s32 InitResources();      /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjCvShutter_c_size_must_be_0x324[sizeof(daObjCvShutter_c) == 0x324 ? 1 : -1];

#endif /* DAOBJCVSHUTTER_C_H */
