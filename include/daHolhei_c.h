#ifndef DAHOLHEI_C_H
#define DAHOLHEI_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

/* Chuckya (HOLHEI 190) -- ov062/daHolhei_c.
 *
 * RTTI ov062:0x0211d9ac is the string 10daHolhei_c. The factory
 * daHolhei_c_classInit allocates 0x438 bytes. Member names below are from
 * this TU's own reads and writes. d_a_holhei.cpp defines g_profile_HOLHEI.
 */

struct HolheiState;  /* enter / update PMF pair, defined in d_a_holhei.cpp */

struct daHolhei_c : dEnemyBase_c {
    dCcAc_c     mdCc_c;        /* 0x110 */
    dBgCh_Actr  mMeshClsn;     /* 0x144 */
    dExtAnmModel_c mModel;     /* 0x300 */
    /* Current state record. Behavior calls its update PMF, when non-null,
       unless UpdateKillByInvincibleChar has already ended the frame.
       InitResources enters data_ov062_0211dee0 through
       ChangeState. */
    HolheiState *mState;       /* 0x364 */
    dExtShadowModel_c mShadowModel;  /* 0x368 */
    /* UpdateShadow / UpdateHeldShadow copy identity here, write pos>>3 into
       the translation, and pass it to DropShadowRadHeight. */
    Matrix4x3   mShadowMtx;    /* 0x390 */
    Vector3 mHomePos;          /* 0x3c0 -- InitResources copies mPos here */
    /* St_Charge_Main stores the closest player's pos when the charge starts. */
    Vector3 mChasePos; /* 0x3cc */
    /* Not dActor_c::mPrevPos at 0x068. Behavior rewinds mPos from these on a
       cliff or level-fence trip, then republishes them. */
    s32         mPrevPosX;     /* 0x3d8 */
    s32         mPrevPosY;     /* 0x3dc */
    s32         mPrevPosZ;     /* 0x3e0 */
    /* 1 when IsGoingOffCliff or the level fence trips, else 0. */
    u8          mEdgeStop;     /* 0x3e4 */
    /* St_Grabbed_Main sets this once it has started the grab animation. */
    u8          mGrabAnim;     /* 0x3e5 */
    /* Behavior counts this down. St_TurnHome_Main will not turn while it is set. */
    u16         mTurnWait;     /* 0x3e6 */
    /* Behavior counts this down. St_Walk_Main will not re-engage the player
       while it is set; St_SlopeTurn_Main and St_WallTurn_Main reload it with
       0x1e. */
    u16         mChaseCooldown;/* 0x3e8 */
    u8          pad_3ea[2];    /* 0x3ea */
    /* Sound::PlayLong handle. St_Charge_Main and St_Walk_Main pass id 0x18a. */
    u32         mMoveSound;    /* 0x3ec */
    /* 0 aim at the player, 1 run, 2 past the point saved in mChasePos. */
    s32         mChargeStep;   /* 0x3f0 */
    s16         mTargetAngY;   /* 0x3f4 */
    u8          pad_3f6[2];    /* 0x3f6 */
    /* Actor being held, or the player holding this. +0xc8 on that actor is
       still an unnamed dActor_c gap; Behavior and Render read it raw. */
    daPly_c *mHeld; /* 0x3f8 */
    /* BuildHoldMatrix builds this from bone 3 and stores its address at
       this+0xc8, the same unnamed gap. */
    Matrix4x3   mHoldMtx;      /* 0x3fc */
    /* Behavior zeroes these. HomeCarryOffset homes them toward the two
       Vector3s at data_ov062_0211df10 and hands them to UpdateCarry. */
    Vector3 mCarryOffs; /* 0x42c */

    /* INLINE IS LOAD-BEARING. Out-of-line, mwccarm emits D0 before D1 plus
       a homeless D2; the cartridge has D1 at 0x02115ee0 then D0 at 0x02115f28
       and no D2. */
    virtual ~daHolhei_c() {}

    virtual s32 OnAimedAtWithEgg(); /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* The ten state records in d_a_holhei.cpp hold these as {enter, update}:
       _Init is slot 0, _Main is slot 1. Declared below in the handlers' own
       ascending-address order, which is the record each pair belongs to:
         St_Throw_*      data_ov062_0211deb0
         St_LiftLoop_*   data_ov062_0211de80
         St_Lift_*       data_ov062_0211def0
         St_Thrown_*     data_ov062_0211dec0
         St_Grabbed_*    data_ov062_0211dea0
         St_Charge_*     data_ov062_0211de90
         St_SlopeTurn_*  data_ov062_0211de70
         St_WallTurn_*   data_ov062_0211df00
         St_Walk_*       data_ov062_0211dee0  -- InitResources enters this one
         St_TurnHome_*   data_ov062_0211ded0
       Every one returns 1. The state names are coined. */
    int St_Throw_Main();
    int St_Throw_Init();
    int St_LiftLoop_Main();
    int St_LiftLoop_Init();
    int St_Lift_Main();
    int St_Lift_Init();
    int St_Thrown_Main();
    int St_Thrown_Init();
    int St_Grabbed_Main();
    int St_Grabbed_Init();
    int St_Charge_Main();
    int St_Charge_Init();
    int St_SlopeTurn_Main();
    int St_SlopeTurn_Init();
    int St_WallTurn_Main();
    int St_WallTurn_Init();
    int St_Walk_Main();
    int St_Walk_Init();
    int St_TurnHome_Main();
    int St_TurnHome_Init();
    int ChangeState(HolheiState *);
    void UpdateHeldShadow();
    void UpdateShadow();
    void HandleCylinderHit();
    /* 1 on a wall, 2 on a slope steeper than 0x1000, else 0. */
    int ClassifyObstruction();
    /* Bone 3's transform times the model's yaw, stored where a holder looks. */
    void BuildHoldMatrix();
    /* Homes the carry offset toward the held actor's slot, then publishes the carried matrix. */
    void HomeCarryOffset();
};

typedef char daHolhei_c_size_must_be_0x438[sizeof(daHolhei_c) == 0x438 ? 1 : -1];

#endif /* DAHOLHEI_C_H */
