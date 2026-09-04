#ifndef ENEMYSPAWNER_H
#define ENEMYSPAWNER_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daECreate_c -- daECreate_c_classInit (was EnemySpawner_Spawn), g_profile_ENEMY_CREATE (was EnemySpawner_SpawnInfo)
 */

#include "dActor_c.h"

/* daECreate_c_classInit allocates 0xe0 bytes, constructs dActor_c, and stores
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

    virtual ~EnemySpawner();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
};

typedef char EnemySpawner_size_must_be_0xe0[
    sizeof(EnemySpawner) == 0xe0 ? 1 : -1];

#endif /* ENEMYSPAWNER_H */
