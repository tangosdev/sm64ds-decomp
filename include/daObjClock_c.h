#ifndef DAOBJCLOCK_C_H
#define DAOBJCLOCK_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

/* ROM RTTI identifies daObjClock_c as a dActor_c. Both clock-hand profiles
 * allocate 0x128 bytes and construct Model at +0xd4. The hand index selects
 * the long (0) or short (1) hand. Unobserved byte ranges remain unnamed. */

#ifdef __cplusplus

extern "C" void *_ZN7fBase_cnwEj(u32 size);

struct daObjClock_c : dActor_c {
    /* mwcc requires unsigned long for operator new; forward to the existing
     * actor allocator, whose configured ABI takes unsigned int. */
    static void *operator new(unsigned long size) { return _ZN7fBase_cnwEj(size); }

    u8    unk_0d0[0x4];    /* 0x0d0 */
    Model mModel;          /* 0x0d4 */
    u8    mHandIndex;      /* 0x124 -- 0 = long, 1 = short */
    u8    unk_125[0x3];    /* 0x125 */

    /* This inline form emits the retail D1/D0 pair without an extra D2. */
    virtual ~daObjClock_c() {}         /* slots 16, 17 */

    /* Overrides remain virtual through their inherited signatures.
     * InitResources is the first declared non-inline virtual; its definition
     * in the class TU supplies the vtable and RTTI group. */
    virtual int InitResources();       /* slot 0 */
    virtual int CleanupResources();    /* slot 3 */
    virtual int Behavior();            /* slot 6 */
    virtual int Render();              /* slot 9 */
};

typedef char daObjClock_c_size_must_be_0x128[sizeof(daObjClock_c) == 0x128 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCLOCK_C_H */
