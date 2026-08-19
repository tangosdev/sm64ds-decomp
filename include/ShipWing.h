#ifndef SHIPWING_H
#define SHIPWING_H

#include "CommonModel.h"
#include "dActor_c.h"

/* ShipWing_Spawn allocates 0x11c bytes, constructs dActor_c, and constructs a
 * CommonModel at 0xd4. D1 destroys that model before chaining to dActor_c.
 * InitResources snapshots the three actor angles at 0x110..0x114 and Behavior
 * advances the frame and two parameter-derived mode flags at 0x116..0x119.
 */
struct ShipWing : dActor_c {
    u8          pad_0d0[0x4];
    CommonModel mModel;          /* 0x0d4 */
    s16         mBaseAngleX;     /* 0x110 */
    s16         mBaseAngleY;     /* 0x112 */
    s16         mBaseAngleZ;     /* 0x114 */
    u16         mFrame;          /* 0x116 */
    u8          mReverseMotion;  /* 0x118 */
    u8          mPlaySound;      /* 0x119 */
    u8          pad_11a[0x2];

    virtual ~ShipWing();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

typedef char ShipWing_size_must_be_0x11c[
    sizeof(ShipWing) == 0x11c ? 1 : -1];

#endif /* SHIPWING_H */
