/* ShipUp's matched layout and real dBgActor_c inheritance. */
#ifndef SHIPUP_H
#define SHIPUP_H
#include "dBgActor_c.h"

struct ShipUp : dBgActor_c {
    /* dBgActor_c's data ends at 0x31e and its sizeof rounds to 0x320. The ABI
       reuses that tail padding for the first derived byte, as the matched
       ShipUp methods prove. */
    u8  mModelIndex;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 mBobAngle;            /* 0x320 */
    u8  pad_322[0x2];
    s32 mSoundHandle;            /* 0x324 */
    u16 unk_328;            /* 0x328 */
    virtual ~ShipUp();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

typedef char ShipUp_size_must_be_0x32c[sizeof(struct ShipUp) == 0x32c ? 1 : -1];

#endif
