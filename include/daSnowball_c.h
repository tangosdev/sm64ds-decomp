#ifndef DASNOWBALL_C_H
#define DASNOWBALL_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN12daSnowball_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 Model                      0x50   -> 0x350
 *     0x350 dExtShadowModel_c                0x28   -> 0x378
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `Snowball_Spawn` calls
 * `fBase_c::operator new(908)` -- 0x38c -- and stores `_ZTV12daSnowball_c`,
 * so that literal IS this class's sizeof. The last field read is the word at
 * 0x388, which ends at 0x38c.
 *
 * SM64DS proves this class as daSnowball_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daSnowball_c_classInit at 0x021264b4 (historical alias Snowball_Spawn)
 * allocates 0x38c and installs this class's cartridge vtable. It backs the
 * SNOWBALL registry profile, whose descriptor at 0x02128a98 is reconstructed
 * as g_profile_SNOWBALL.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct daSnowball_c;

/* A two-entry state record. daSnowball_c.cpp defines data_ov081_02128eb4
 * with these two methods; the compiler copies the descriptors in at overlay
 * load. `enter` runs once when the record is installed, `update` once per
 * Behavior. */
typedef int (daSnowball_c::*daSnowball_StateFn)();
struct daSnowball_StateRec {
    daSnowball_StateFn enter;   /* +0x00 -- func_ov081_021261b8 */
    daSnowball_StateFn update;  /* +0x08 -- func_ov081_021260fc */
};

struct daSnowball_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    dExtShadowModel_c                  mShadowModel;          /* 0x350 */
    daSnowball_StateRec         *mStateRec;             /* 0x378 -- installed by func_ov081_021261d4 */
    /* Where the actor was placed: InitResources copies mPosX/Y/Z here before
       lifting the snowball 0x32000 (50 units) above it. */
    s32                          mSpawnPosX;            /* 0x37c */
    s32                          mSpawnPosY;            /* 0x380 */
    s32                          mSpawnPosZ;            /* 0x384 */
    /* Cleared by the enter helper; set to 1 by the update helper the first time
       the snowball is below mSpawnPosY while mTerminalVelocity still holds its
       initial -0x3c000, at which point that helper also zeroes mVertSpeed and
       mVertAccel. Only those two helpers touch it. */
    s32                          mReachedSpawnY;        /* 0x388 */

    /* --- vtable --- */
    virtual ~daSnowball_c();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();

    /* State-record targets. The address is the method name. */
    int func_ov081_021261b8();
    int func_ov081_021260fc();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSnowball_c_size_must_be_0x38c[sizeof(daSnowball_c) == 0x38c ? 1 : -1];
#endif

#endif /* DASNOWBALL_C_H */
