#ifndef KOOPA_H
#define KOOPA_H

#include "types.h"

/* Retail ov062 RTTI at 0x0211da5c spells "8daNknk_c". Its typeinfo at
 * 0x0211da68 names dEnemyBase_c; the vtable address point is 0x0211dab4.
 * Normal (NOKONOKO, actor 0xcb) and small (NOKONOKO_S, actor 0xcc)
 * factories both allocate 0x3d0 bytes of this one class.
 *
 * Field names below are from this TU's reads. mKoopaVariant is 0 while
 * the Koopa still has its shell, 1 after the shell is popped
 * (DropShell), back to 0 when it touches a SHELL (actor 0x11d)
 * in state 2, and 2 for the small Koopa. mHomePos is the spawn point the
 * leash is measured from. mSafePos is the last spot that was not a
 * cliff edge. mAimAngle faces the closest player, or home when that
 * player is outside the leash. mTargetAngle is what mPrevAngle.y is
 * steered toward. mTimer counts the idle pause and the post-kick wait.
 * mTurnTimer counts down to the next random heading. mTurning stays set
 * while AngleAwayFromWallOrCliff's turn is still running. mFootstep
 * blocks a second dust puff in the same animation window.
 */

#include "dEnemyBase_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daNknk_c : dEnemyBase_c {
    dCcAc_c         mdCc_c;                  /* 0x110 */
    dBgCh_Actr      mWithMeshClsn;           /* 0x144 */
    dExtAnmModel_c mAnmModel;                /* 0x300 */
    dExtShadowModel_c     mShadowModel;            /* 0x364 */
    s32             mState;                  /* 0x38c */
    s32             mKoopaVariant;           /* 0x390 */
    s32             mModelIndex;             /* 0x394 */
    u8              mAnimIndex;              /* 0x398 */
    u8              pad_399[3];              /* 0x399 */
    Vector3 mHomePos; /* 0x39c -- spawn point used by the leash and return heading */
    Vector3 mSafePos; /* 0x3a8 */
    daPly_c *mClosestPlayer;                 /* 0x3b4 */
    s32             mPlayerDist;             /* 0x3b8 */
    s32             mAnimSpeed;              /* 0x3bc */
    s16             mAimAngle;               /* 0x3c0 */
    s16             mTargetAngle;            /* 0x3c2 */
    u16             mWalkState;              /* 0x3c4 */
    u16             mTimer;                  /* 0x3c6 */
    u16             mTurnTimer;              /* 0x3c8 */
    u16             mInvincibleTimer;        /* 0x3ca */
    u8              mTurning;                /* 0x3cc */
    u8              mFootstep;               /* 0x3cd */
    u8              mLandingDustTimer;       /* 0x3ce */
    u8              pad_3cf;                 /* 0x3cf */

    /* --- vtable --- */
    virtual ~daNknk_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* State and behavior helpers. The cartridge records no English names
     * for them, so every spelling below is reconstructed from this TU's
     * reads; the vtable and the sinit do not reference any of them, so
     * these spellings only matter to this file and symbols.txt.
     *
     * Behavior dispatches on mKoopaVariant: 0 and 2 (shelled, normal and
     * small) go to UpdateStateWithShell, 1 (shell popped) to
     * UpdateStateNoShell. Those switch on mState:
     *
     *   mState  with shell (0, 2)   no shell (1)
     *   0       WaitUntilGrounded   WaitUntilGrounded
     *   1       UpdateWait          UpdateChaseShell
     *   2       UpdateWalkWithShell UpdateDive
     *   3       UpdateRun           UpdateDive
     *   4       UpdateShellSlide    UpdateWait
     *   5       --                  UpdateWalkNoShell
     */
    void KillOnWaterFloor();
    void SpawnLandingDust();
    void SpawnFootstepDust(unsigned int a1, unsigned int a2, unsigned int a3, unsigned short a4);
    void SetAnimByIndex(int idx);
    void SpawnBlueCoin();
    void UpdateStateNoShell();
    void UpdateStateWithShell();
    void BounceOffHurtPlayer();
    int DistToClosestPlayer();
    dActor_c *FindNearestShell(); /* nearest SHELL (actor 0x11d), or 0 */
    void DropShell();
    void HandleCollision();
    void SlideToStop(int limit);
    void TryStartRun();
    void UpdateWalkEnd();
    void UpdateWalk();
    void UpdateWalkStart();
    void UpdateClosestPlayer(int lim);
    void UpdateModelMatrixAndShadow();
    void UpdateWalkNoShell();
    void UpdateDive();
    void UpdateChaseShell();
    void WaitUntilGrounded();
    void UpdateShellSlide();
    void UpdateRun();
    void UpdateWalkWithShell();
    void UpdateWait();
};

// Community source compatibility name; retail RTTI is daNknk_c.
typedef daNknk_c Koopa;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Koopa_size_must_be_0x3d0[sizeof(Koopa) == 0x3d0 ? 1 : -1];
#endif

#endif /* KOOPA_H */
