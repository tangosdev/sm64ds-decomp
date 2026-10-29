#ifndef DAOBJBSWDOOR_C_H
#define DAOBJBSWDOOR_C_H

#include "types.h"
#include "daObjSwdoor_c.h"

/**
 * Bob-omb Battlefield's switch-operated shutter. ov014 SHUTTER_BOB (40).
 *
 * `daObjBSwdoor_c` is the RTTI name. It derives from daObjSwdoor_c, not
 * from dBgActor_c: the destructor stores three vptrs. Factory allocates
 * 0x324. Overrides the three slots the base leaves null (InitResources,
 * CleanupResources, Behavior) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers; Init also Enable()s the mesh.
 *
 * No fields of its own: the bytes at 0x31e..0x321 (mTurnDir, mState, mTimer,
 * mEventBit) are daObjSwdoor_c's, written and read by the shared ov002
 * helpers, and daObjSwdoor_c already rounds to the factory's 0x324.
 */
struct daObjBSwdoor_c : daObjSwdoor_c {
    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBSwdoor_c() {}
    virtual s32 Behavior();         /* slot 6 */
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBSwdoor_c_size_must_be_0x324[sizeof(daObjBSwdoor_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJBSWDOOR_C_H */
