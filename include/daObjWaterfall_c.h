#ifndef DAOBJWATERFALL_C_H
#define DAOBJWATERFALL_C_H

#include "types.h"
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Waterfall mist. No model: InitResources picks a particle ID from the
 * current course, then every frame Behavior (re)issues that particle at
 * this actor's position, feeding last frame's handle back so the
 * emitter is recycled instead of stacked.
 *
 * SIZE 0xdc from the factory's own `operator new` argument. The last
 * field closes on it.
 */
struct daObjWaterfall_c : dActor_c {
    u8  pad_0d0[0x4];
    u16 mParticleID;     /* 0x0d4 */
    u8  pad_0d6[0x2];
    /* Handle, not a pointer: Behavior stores Particle::System::New's
       result and feeds it back as the unique-ID argument next frame.
       Nothing dereferences it. */
    u32 mParticleHandle; /* 0x0d8 */

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjWaterfall_c() {}
    virtual s32 InitResources(); /* slot 0 */
    virtual s32 Behavior();      /* slot 6 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjWaterfall_c_size_must_be_0xdc[
    sizeof(daObjWaterfall_c) == 0xdc ? 1 : -1];

#endif /* DAOBJWATERFALL_C_H */
