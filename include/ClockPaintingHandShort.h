#ifndef CLOCKPAINTINGHANDSHORT_H
#define CLOCKPAINTINGHANDSHORT_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

/* daObjClock_c is an alias at the same vtable address. The ROM's D1/D0 symbols,
 * vtable, base teardown, constructor call, and 0xd4 Model construction together
 * establish the actual leaf class: dActor_c occupies 0x000..0x0cf, four bytes of
 * derived padding precede the owned Model, and the one-byte hand index follows it.
 */
struct ClockPaintingHandShort : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 */
    Model mModel;          /* 0x0d4 */
    u8 mHandIndex;         /* 0x124 */
    u8 pad_125[0x3];       /* 0x125 */

    virtual ~ClockPaintingHandShort();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char ClockPaintingHandShort_size_must_be_0x128[
    sizeof(struct ClockPaintingHandShort) == 0x128 ? 1 : -1];

#endif
