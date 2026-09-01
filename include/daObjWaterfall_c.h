#ifndef DAOBJWATERFALL_C_H
#define DAOBJWATERFALL_C_H

#include "dActor_c.h"

/* Retail public vtable address point used by the allocation seam below. The
 * class definition remains the authority that makes this a modeled vtable,
 * rather than a factory-local shadow declaration. */
extern "C" int _ZTV16daObjWaterfall_c[];

/* The retail RTTI record at ov002:0x0210945c spells daObjWaterfall_c and
 * points to the vtable whose address point is 0x021094a0.
 * daObjWaterfall_c_classInit (historical alias WaterfallMist_Spawn) allocates
 * 0xdc bytes and installs that table. The source-style classInit spelling is
 * a lineage-supported reconstruction, not a ROM-preserved symbol.
 * The D1 chains directly to dActor_c::~dActor_c, proving the inheritance below.
 * Behavior reads 0xd4 as a particle ID and both reads and writes a pointer at
 * 0xd8, correcting the generated one-byte marker.
 */
struct daObjWaterfall_c : dActor_c {
    u8    pad_0d0[0x4];
    u16   mParticleID;       /* 0x0d4 */
    u8    pad_0d6[0x2];
    /* Stays void*: the only use (Behavior) stores Particle::System::New's
       result and passes it straight back as a u32 handle on the next call.
       Nothing ever dereferences it, so no pointee type is evidenced -- naming
       one would be invention, not recovery. */
    void *mParticle;         /* 0x0d8 */

    /* InitResources is the first out-of-line virtual/key function. Together
     * with this inline destructor, mwccarm owns the retail D1/D0 pair in the
     * ROM's own order and the complete class RTTI/vtable group, without
     * retaining a D2 body. An out-of-line destructor definition emits D0
     * before D1 and cannot be isolated against the ROM span. */
    virtual ~daObjWaterfall_c() {}

    virtual int InitResources();
    virtual int Behavior();
};

typedef char daObjWaterfall_c_size_must_be_0xdc[
    sizeof(daObjWaterfall_c) == 0xdc ? 1 : -1];

#endif /* DAOBJWATERFALL_C_H */
