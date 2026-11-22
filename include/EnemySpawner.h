#ifndef ENEMYSPAWNER_H
#define ENEMYSPAWNER_H

/* daECreate_c is the cartridge RTTI name at ov002:0x0210b304.
 * Its typeinfo at 0x0210b2f8 identifies dActor_c as the base.
 * The factory and profile spellings remain reconstructed names.
 */

#include "dActor_c.h"

/* daECreate_c_classInit allocates 0xe0 bytes, constructs dActor_c, and stores
 * _ZTV11daECreate_c. D1 chains directly to dActor_c::~dActor_c. The three named
 * fields are constrained by InitResources and Behavior at 0xd4..0xdc.
 */
struct daECreate_c : dActor_c {
    u16 mActorToSpawn;      /* 0x0d4 */
    u8  pad_0d6[0x2];
    s32 mPreviousEventBit;  /* 0x0d8 */
    u8  mEventBit;          /* 0x0dc */
    u8  pad_0dd[0x3];

    virtual ~daECreate_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
};

typedef char EnemySpawner_size_must_be_0xe0[
    sizeof(daECreate_c) == 0xe0 ? 1 : -1];

typedef daECreate_c EnemySpawner;

#endif /* ENEMYSPAWNER_H */
