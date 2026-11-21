#ifndef DABOOKGEN_C_H
#define DABOOKGEN_C_H

#include "dActor_c.h"

/* daBookGen_c_classInit allocates 0xd8 bytes, constructs dActor_c, and stores
 * _ZTV11daBookGen_c. D1 chains directly to dActor_c::~dActor_c, while
 * Behavior identifies the only derived field as the book-shot cooldown.
 *
 * NAME. The cartridge's RTTI names this class daBookGen_c: _ZTI11daBookGen_c
 * at ov020 0x02114838, _ZTS11daBookGen_c at 0x02114850, _ZTV11daBookGen_c at
 * 0x021148d8. It was carried here under the coined name BookShotSpawner until
 * that was replaced by the ROM's.
 */
struct daBookGen_c : dActor_c {
    u16 mSpawnTimer;        /* 0x0d4 */
    u8  pad_0d6[0x2];

    virtual ~daBookGen_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
};

typedef char daBookGen_c_size_must_be_0xd8[
    sizeof(daBookGen_c) == 0xd8 ? 1 : -1];

#endif /* DABOOKGEN_C_H */
