#ifndef DABAR_C_H
#define DABAR_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/**
 * Invisible climbable pole. Mario grabs mClsn.
 */
struct daBar_c : dActor_c {
    u8 pad_0d0[0x4]; /* 0x0d0 unused */
    dCcAc_c mClsn;   /* 0x0d4 climb cylinder */

    virtual ~daBar_c() {}
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBar_c_size_must_be_0x108[sizeof(daBar_c) == 0x108 ? 1 : -1];
#endif

#endif /* DABAR_C_H */
