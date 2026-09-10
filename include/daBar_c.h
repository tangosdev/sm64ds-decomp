#ifndef DABAR_C_H
#define DABAR_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);
extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
    dActor_c *self, int offsetY, int radius, int clipDistance, int farDistance);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height, u32 flags, u32 vulnFlags);

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
        return _ZN7fBase_cnwEj(size);
    }

    /* Local adapters preserve the existing scalar call boundaries.
     * The measured collision-call alternatives are recorded in the handoff. */
    void SetRanges(Fix12i offsetY, Fix12i radius, Fix12i clipDistance, Fix12i farDistance) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
            this, offsetY, radius, clipDistance, farDistance);
    }
    void InitClsn(Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mClsn, this, radius, height, flags, vulnFlags);
    }
};

typedef char daBar_c_size_must_be_0x108[
    sizeof(daBar_c) == 0x108 ? 1 : -1];

struct DaBarSpawnInfo {
    daBar_c *(*classInit)();
    u16 executeOrder;      /* +4 behavior priority */
    u16 drawOrder;         /* +6 render priority */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char DaBarSpawnInfo_size_must_be_0x1c[
    sizeof(DaBarSpawnInfo) == 0x1c ? 1 : -1];

#endif /* DABAR_C_H */
