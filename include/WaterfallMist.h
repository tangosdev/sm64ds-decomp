#ifndef WATERFALLMIST_H
#define WATERFALLMIST_H

#include "dActor_c.h"

/* WaterfallMist_Spawn allocates 0xdc bytes, constructs dActor_c, and stores
 * _ZTV13WaterfallMist. Its D1 chains directly to dActor_c::~dActor_c, proving
 * the inheritance below. Behavior reads 0xd4 as a particle ID and both reads
 * and writes a pointer at 0xd8, correcting the generated one-byte marker.
 */
struct WaterfallMist : dActor_c {
    u8    pad_0d0[0x4];
    u16   mParticleID;       /* 0x0d4 */
    u8    pad_0d6[0x2];
    /* Stays void*: the only use (Behavior) stores Particle::System::New's
       result and passes it straight back as a u32 handle on the next call.
       Nothing ever dereferences it, so no pointee type is evidenced -- naming
       one would be invention, not recovery. */
    void *mParticle;         /* 0x0d8 */

    virtual ~WaterfallMist() {}

    virtual int InitResources();
    virtual int Behavior();
};

typedef char WaterfallMist_size_must_be_0xdc[
    sizeof(WaterfallMist) == 0xdc ? 1 : -1];

#endif /* WATERFALLMIST_H */
