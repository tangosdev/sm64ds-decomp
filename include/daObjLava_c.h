#ifndef DAOBJLAVA_C_H
#define DAOBJLAVA_C_H

#include "types.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Popping lava bubbles. No model: every frame it (re)issues particle
 * 0xb7 at the closest player's position, feeding last frame's handle
 * back so the emitter is recycled instead of stacked.
 *
 * SIZE 0xd8 from the factory's own `operator new` argument. The last
 * field closes on it.
 */
struct daObjLava_c : dActor_c {
    u8  pad_0d0[0x4];
    u32 mEffectHandle; /* 0x0d4 -- live particle handle */

    /* Inline and first: out-of-line mwccarm emits D0 before D1; retail
       has D1 at 0x020b6d28 below D0 at 0x020b6d4c. */
    virtual ~daObjLava_c() {}
    virtual s32 InitResources(); /* slot 0 */
    virtual s32 Behavior();      /* slot 6 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjLava_c_size_must_be_0xd8[sizeof(daObjLava_c) == 0xd8 ? 1 : -1];

#endif /* DAOBJLAVA_C_H */
