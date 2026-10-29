#ifndef DAGROCK_C_H
#define DAGROCK_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN9daGrock_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dExtSimpleModel_c                      0x50   -> 0x160
 *     0x160 dExtShadowModel_c                0x28   -> 0x188
 *     0x1b8 dCcAcPos_c  0x40   -> 0x1f8
 *     0x1f8 dBgCh_Actr               0x1bc  -> 0x3b4
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daGrock_c_classInit` calls
 * `fBase_c::operator new(968)` -- 0x3c8 -- and stores `_ZTV9daGrock_c`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x3c4; the difference is trailing space no source reads.
 *
 * SM64DS RTTI names the implementation daGrock_c. The reconstructed factory
 * daGrock_c_classInit (historical alias RollingRock_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_GORO_ROCK
 * (historical alias RollingRock_SpawnInfo) is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dBgCh_Actr.h"

struct daGrock_c : dEnemyBase_c {
    dExtSimpleModel_c mModel;       /* 0x110 */
    dExtShadowModel_c mShadowModel; /* 0x160 */
    u8 pad_188[0x30];
    dCcAcPos_c mdCcAcPos_c;   /* 0x1b8 */
    dBgCh_Actr mWithMeshClsn; /* 0x1f8 */
    /* On a star rock, the uniqueID of the spawner that made it (the spawner
       stores it into the child right after Spawn); 0 on everything else.
       When the star rock falls out of the level it looks the spawner up by
       this id and clears its mStarRockOut. */
    u32 mSpawnerID; /* 0x3b4 */
    /* The rolling sound's Sound::PlayLong handle, passed back in each frame
       the rock rolls on the ground; InitResources zeroes it. */
    s32 mRollSoundID; /* 0x3b8 */
    u8 pad_3bc[0x2];
    u8 mType; /* 0x3be */
    /* Spawner only: rocks spawned since the last star rock. Kind 1 spawns
       the star rock once this reaches 5, then zeroes it. */
    u8 mSpawnCount; /* 0x3bf */
    /* The slot dActor_c::TrackStar returned for mStarID (-1 when none);
       UntrackStar / UntrackAndSpawnStar take it back (s8 &trackStarID). */
    s8 mTrackStarID; /* 0x3c0 */
    /* The star index from param1's second nibble, 0xff when the rock
       carries none; handed to TrackStar, UntrackAndSpawnStar and passed on
       in the star rock's spawn parameter. */
    u8 mStarID; /* 0x3c1 */
    /* Spawner only: 1 while its star rock is alive (set on the spawn,
       cleared by the star rock when it falls). No second star rock is
       spawned while it is set. */
    u8 mStarRockOut; /* 0x3c2 */
    u8 pad_3c3[0x1];
    /* The rolling dust's Particle::System::New handle, renewed each frame
       the rock rolls on the ground; InitResources zeroes it. */
    s32 mDustParticleID; /* 0x3c4 */

    /* --- vtable --- */
    virtual ~daGrock_c();

    virtual s32 OnAimedAtWithEgg(); /* slot 29 */

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();

    void SpawnStar();
    void UpdateModelTransform();
    void HandleCylinderHit();
    void HandleMeshCollision();
    void ApplyGravityAndMove();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daGrock_c_size_must_be_0x3c8[sizeof(daGrock_c) == 0x3c8 ? 1 : -1];
#endif

#endif /* DAGROCK_C_H */
