#ifndef ENEMYSPAWNER_H
#define ENEMYSPAWNER_H

#include "dActor_c.h"

/* EnemySpawner_Spawn allocates 0xe0 bytes, constructs dActor_c, and stores
 * _ZTV12EnemySpawner. D1 chains directly to dActor_c::~dActor_c. The remaining
 * four fields are closed by InitResources and Behavior at 0xd4..0xdc.
 */
struct EnemySpawner : dActor_c {
    u8  pad_0d0[0x4];
    u16 mActorToSpawn;      /* 0x0d4 */
    u8  pad_0d6[0x2];
    s32 mPreviousEventBit;  /* 0x0d8 */
    u8  mEventBit;          /* 0x0dc */
    u8  pad_0dd[0x3];

    virtual ~EnemySpawner() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
};

typedef char EnemySpawner_size_must_be_0xe0[
    sizeof(EnemySpawner) == 0xe0 ? 1 : -1];

#endif /* ENEMYSPAWNER_H */
