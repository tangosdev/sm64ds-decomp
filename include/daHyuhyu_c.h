#ifndef DAHYUHYU_C_H
#define DAHYUHYU_C_H

#include "types.h"

/* Fwoosh, the cloud that blows (profile HYUHYU, ov091). ROM RTTI
 * daHyuhyu_c; direct base dEnemyBase_c (_ZTI10daHyuhyu_c word 2 is
 * ov002:0x021081c0, _ZTI12dEnemyBase_c). daHyuhyu_c_classInit allocates
 * 0x378.
 *
 * One class, two roles, picked by param1's low byte (mVariant):
 *   0  the cloud itself: turns toward the nearest player and, when one is
 *      close and in front, plays its blow animation and fires gusts;
 *   1  one gust: spawned by the cloud as another HYUHYU actor with
 *      param1 = 1, it flies along its pitch and heading and knocks the cap
 *      off the player it hits. It has no model.
 *
 * mState points at one of three two-word records (enter, execute) of
 * pointer-to-members that ov091's static initializer fills in .bss:
 * data_ov091_021356d0 (cloud waiting), data_ov091_021356b0 (cloud
 * blowing) and data_ov091_021356c0 (gust flying).
 *
 * The vtable at ov091:0x021353ac is 31 slots, the same as dEnemyBase_c's,
 * and differs from it only in the slots declared below.
 *
 * Historical aliases: Fwoosh (the class), Fwoosh_Spawn (the factory),
 * Fwoosh_SpawnInfo (the profile).
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct daHyuhyu_c;

/* One state: its enter and execute handlers. Both are optional. */
struct daHyuhyu_c_State {
    int (daHyuhyu_c::*enter)();       /* 0x00 */
    int (daHyuhyu_c::*execute)();     /* 0x08 */
};

struct daHyuhyu_c : dEnemyBase_c {
    dCcAc_c mdCcAc_c;                 /* 0x110 */
    dBgCh_Actr mWithMeshClsn;         /* 0x144 */
    ModelAnim mModelAnim;             /* 0x300 -- the cloud only */
    daHyuhyu_c_State *mState;         /* 0x364 */
    u32 mGustParticle;                /* 0x368 -- particle system handles */
    u32 mGustParticle2;               /* 0x36c */
    u8  pad_370[0x4];
    s32 mVariant;                     /* 0x374 -- 0 cloud, 1 gust */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daHyuhyu_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
    virtual void OnPendingDestroy();
    virtual int OnYoshiTryEat();               /* slot 18 */
    virtual void OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int OnAimedAtWithEgg();            /* slot 29 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daHyuhyu_c_size_must_be_0x378[
    sizeof(daHyuhyu_c) == 0x378 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAHYUHYU_C_H */
