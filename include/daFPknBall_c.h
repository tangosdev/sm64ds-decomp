#ifndef DAFPKNBALL_C_H
#define DAFPKNBALL_C_H

#include "types.h"

/* daFPknBall_c (FPAKUN_BALL, actor 254): the fireball the fire piranha plant
 * (daFPkn_c) spits. As daFPkn_c spawns it (variant 3) it keeps its heading
 * for a fixed distance, where other variants steer toward the closest player.
 * It burns a Player it touches (Player::Burn) and dies in a puff of dust when
 * it runs out of range, hits a wall or touches water.
 *
 * Derives from dEnemyBase_c, on the evidence of its own destructor:
 * `_ZN12daFPknBall_cD1Ev` stores this vtable, destroys its members in reverse
 * declaration order, then calls `dEnemyBase_c::~dEnemyBase_c`. The allocation
 * is 888 (0x378) bytes (`daFPknBall_c_classInit`).
 *
 * The typed members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr      0x1bc  -> 0x300
 *     0x300 dExtShadowModel_c     0x28   -> 0x328
 *     0x328 Matrix4x3       0x30   -> 0x358
 *
 * The 0x12c word the old header carried as unk_12c is mdCcAc_c + 0x1c.
 *
 * Member NAMES that predate this comment (mdCcAc_c, mWithMeshClsn, mShadowModel)
 * are kept so callers spelling them still compile; the rest are named for what
 * Behavior does with them.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct daFPknBall_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    dExtShadowModel_c                  mShadowModel;          /* 0x300 */
    Matrix4x3                    mShadowMat;            /* 0x328 -- translation only; the ball's position >> 3 */
    struct Player *              mTargetPlayer;         /* 0x358 -- ClosestPlayer, refreshed every Behavior */
    s32                          mTargetSpeed;          /* 0x35c -- what mHorzSpeed approaches; dActor_c.h names SpawnFireball's fourth argument unk35c; daFPkn_c passes 0xa000 */
    s32                          mDistanceFlown;        /* 0x360 -- sum of mHorzSpeed each frame */
    s32                          mMaxDistance;          /* 0x364 -- 0x5dc000 (1500 units) from InitResources */
    s16                          mTargetAngleY;         /* 0x368 -- toward the player, else the current heading */
    u16                          unk_36a;               /* 0x36a -- zeroed by InitResources, otherwise unused */
    u8  pad_36c[0x1];
    u8                           mVariant;              /* 0x36d -- param1 & 7; daFPkn_c spawns 3. Other than 0 and 4, InitResources sets vulnFlags 0x8000 and OnYoshiTryEat returns 5 */
    u8                           mMirrored;             /* 0x36e -- when set, the effect and dust position uses -x; never written by this class */
    u8  pad_36f[0x1];
    u32                          mTrailEffect;          /* 0x370 -- last result of Particle::System::NewUnkCallback818, passed back in each frame */
    void *                       mSparkEffect;          /* 0x374 -- last result of Particle::System::New, passed back in each frame */

    /* --- vtable --- */
    virtual ~daFPknBall_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int InitResources();
    int Render();

    void func_ov002_020f88ec();
    void func_ov002_020f897c();
    void func_ov002_020f8b24();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daFPknBall_c_size_must_be_0x378[sizeof(daFPknBall_c) == 0x378 ? 1 : -1];
#endif

#endif /* DAFPKNBALL_C_H */
