#ifndef DAKLR_C_H
#define DAKLR_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

/* Bullet Bill (profile KILLER). ROM RTTI daKlr_c; direct base dEnemyBase_c.
 * Factory allocates 0x3e0.
 *
 * Five boundaries close on sizes other headers assert:
 *
 *     dEnemyBase_c                       ends 0x110
 *     dCcAcPos_c   0x110 + 0x040 = 0x150  -> dBgCh_Actr
 *     dBgCh_Actr   0x150 + 0x1bc = 0x30c  -> the body Model
 *     Model        0x30c + 0x050 = 0x35c  -> the second Model
 *     Model        0x35c + 0x050 = 0x3ac  -> the shadow
 *     ShadowModel  0x3ac + 0x028 = 0x3d4  -> mState
 *
 * The vtable overrides slots 0, 3, 6, 9 (InitResources, CleanupResources,
 * Behavior, Render), 16/17 (D1, D0) and 29 (OnAimedAtWithEgg); every other
 * slot is dEnemyBase_c's inherited word.
 *
 * daKlr_c_classInit is reconstructed (RTTI daKlr_c, KILLER registry).
 * Retail does not store that spelling. Historical aliases: BulletBill (the
 * class), BulletBill_Spawn.
 */
struct daKlr_c : dEnemyBase_c {
    dCcAcPos_c  mCylClsn;          /* 0x110 */
    dBgCh_Actr  mWallClsn;         /* 0x150 */
    Model       mModel;            /* 0x30c */
    Model       mModel2;           /* 0x35c -- same matrix, second file */
    ShadowModel mShadow;           /* 0x3ac */
    s32         mState;            /* 0x3d4 -- 0 flying, 1 shot down */
    u32         mSmokeTrailID;     /* 0x3d8 -- Particle::System unique ID */
    /* Written by daObjBkKillerdai_c::Behavior right after the spawn: the
       launcher that fired this bill, so its own mesh never counts as a wall. */
    dActor_c   *mLauncher;         /* 0x3dc */

    /* Inline is load-bearing: out-of-line emits D0 before D1. */
    virtual ~daKlr_c() {}

    virtual int OnAimedAtWithEgg();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKlr_c_size_must_be_0x3e0[sizeof(daKlr_c) == 0x3e0 ? 1 : -1];
#endif

#endif /* DAKLR_C_H */
