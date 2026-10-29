#ifndef DAOBJCVSHUTTER_C_H
#define DAOBJCVSHUTTER_C_H

#include "types.h"
#include "daObjSwdoor_c.h"

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
 * No fields of its own: the bytes at 0x31e..0x321 (mTurnDir, mState, mTimer,
 * mEventBit) are daObjSwdoor_c's, written and read by the shared ov002
 * helpers, and daObjSwdoor_c already rounds to the factory's 0x324.
 */
struct daObjCvShutter_c : daObjSwdoor_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjCvShutter_c() {}
    virtual s32 Behavior();         /* slot 6 */
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCvShutter_c_size_must_be_0x324[sizeof(daObjCvShutter_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJCVSHUTTER_C_H */
