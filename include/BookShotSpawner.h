#ifndef BOOKSHOTSPAWNER_H
#define BOOKSHOTSPAWNER_H

#include "dActor_c.h"

/* BookShotSpawner_Spawn allocates 0xd8 bytes, constructs dActor_c, and stores
 * _ZTV15BookShotSpawner. D1 chains directly to dActor_c::~dActor_c, while
 * Behavior identifies the only derived field as the book-shot cooldown.
 */
struct BookShotSpawner : dActor_c {
    u8  pad_0d0[0x4];
    u16 mSpawnTimer;        /* 0x0d4 */
    u8  pad_0d6[0x2];

    virtual ~BookShotSpawner() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
};

typedef char BookShotSpawner_size_must_be_0xd8[
    sizeof(BookShotSpawner) == 0xd8 ? 1 : -1];

#endif /* BOOKSHOTSPAWNER_H */
