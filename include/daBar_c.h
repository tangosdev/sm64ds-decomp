#ifndef DABAR_C_H
#define DABAR_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Invisible climbable pole. Mario grabs mClsn.
 */
struct daBar_c : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 */
    dCcAc_c mClsn;         /* 0x0d4 climb cylinder */

    virtual ~daBar_c() {}
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daBar_c_size_must_be_0x108[
    sizeof(daBar_c) == 0x108 ? 1 : -1];

struct DaBarSpawnInfo {
    daBar_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char DaBarSpawnInfo_size_must_be_0x1c[
    sizeof(DaBarSpawnInfo) == 0x1c ? 1 : -1];

#endif /* DABAR_C_H */
