#ifndef DABAR_C_H
#define DABAR_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/* ROM-proven class identity: _ZTS7daBar_c and _ZTI7daBar_c at
 * ov002:0x02108444/0x02108450. InvisiblePole_Spawn is retained as an
 * evidence-bounded C ABI alias; it is not evidence for the C++ class name.
 *
 * The factory allocates 0x108 bytes, constructs dActor_c, stores this class's
 * vptr, and constructs dCcAc_c at 0xd4. The complete destructor tears down the
 * same member and base in reverse order, closing the layout independently. */
struct daBar_c : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 */
    dCcAc_c mClsn;         /* 0x0d4 */

    /* InitResources is the first out-of-line virtual/key function. Together
     * with this inline destructor, mwccarm naturally emits retail D1 then D0,
     * the RTTI/vtable group, and no retained D2. */
    virtual ~daBar_c() {}
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char daBar_c_size_must_be_0x108[
    sizeof(daBar_c) == 0x108 ? 1 : -1];

/* Typed owner for the ROM descriptor at ov002:0x0210845c. Its retained
 * InvisiblePole spelling is an evidence-bounded C ABI alias. */
struct DaBarSpawnInfo {
    daBar_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char DaBarSpawnInfo_size_must_be_0x1c[
    sizeof(DaBarSpawnInfo) == 0x1c ? 1 : -1];

#endif /* DABAR_C_H */
