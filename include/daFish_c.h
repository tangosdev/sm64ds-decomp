#ifndef DAFISH_C_H
#define DAFISH_C_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

/* A school of fish (profile FISH). ROM RTTI daFish_c; direct base dActor_c.
 *
 * One class plays two roles. The actor the level places is an invisible
 * spawner (mIsSpawner set by InitResources): when the player comes near it
 * spawns 1..15 fish of its own actor ID around itself, waits for the player
 * to leave, then waits for every fish to be gone before it can spawn again.
 * Each spawned fish is visible and follows its spawner by uniqueID; it
 * removes itself as soon as the spawner is gone or back in its idle wait.
 *
 * _ZTI8daFish_c names dActor_c as the direct base. D1/D0 call ModelAnim's
 * complete destructor at +0xd4 and dActor_c's base destructor. The factory
 * allocates 0x160, and mVariant's padding closes exactly on it.
 *
 * daFish_c_classInit is reconstructed (RTTI daFish_c, FISH registry).
 * Retail does not store that spelling. Historical aliases: Fish (the class),
 * Fish_Spawn.
 */
struct daFish_c : dActor_c {
    u8 pad_0d0[0x4];        /* 0x0d0 */
    ModelAnim mModelAnim;   /* 0x0d4 -- its Animation base sits at 0x124 */
    s32 mSeed;              /* 0x138 -- set to 1 at construction, never read here */
    u32 mSpawnerID;         /* 0x13c -- a fish's spawner; the spawner's own ID */
    s32 mSurfaceY;          /* 0x140 -- spawner only: fish spawn below this */
    s32 mMaxSpeed;          /* 0x144 -- fleeing */
    s32 mTriggerDist;       /* 0x148 -- distance that ends the current state */
    s32 mState;             /* 0x14c -- index into the state table */
    s32 mStateTimer;        /* 0x150 -- -1 on a state change, so 0 on entry */
    s16 mTurnSpeed;         /* 0x154 */
    s16 mTargetAngle;       /* 0x156 */
    u8  mNumFish;           /* 0x158 -- spawner only: live fish */
    u8  mIsSpawner;         /* 0x159 */
    u8  mWanderTimer;       /* 0x15a */
    u8  mModelIndex;        /* 0x15b */
    u8  mVariant;           /* 0x15c -- 0, or 1..3 for the wandering kinds */
    u8  pad_15d[0x3];       /* 0x15d */

    /* The constructor's only body is mSeed = 1, through an out-of-line
       arm9 setter, after dActor_c's and mModelAnim's constructors. */
    daFish_c();

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daFish_c() {}

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual void OnPendingDestroy();   /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daFish_c_size_must_be_0x160[sizeof(daFish_c) == 0x160 ? 1 : -1];
#endif

#endif /* DAFISH_C_H */
