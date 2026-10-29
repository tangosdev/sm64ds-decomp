#ifndef DAMENBO_C_H
#define DAMENBO_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9daMenbo_cD1Ev` destroys each member, and
 * `Skeeter_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV9daMenbo_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c dExtAnmModel_c                  0x64    -> 0x370
 *
 * SIZE IS THE ROM'S OWN: `Skeeter_Spawn` calls
 * `fBase_c::operator new(944)` -- 0x3b0 -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS proves this class as daMenbo_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daMenbo_c_classInit at 0x02132654 (historical alias Skeeter_Spawn)
 * allocates 0x3b0 and installs this class's cartridge vtable. It backs the
 * MENBO registry profile, whose descriptor at 0x02134144 is reconstructed as
 * g_profile_MENBO.
 */

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct daMenbo_c : dEnemyBase_c {
    dCcAcPos_c    mdCcAcPos_c; /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x150 */
    dExtAnmModel_c mAnmModel;                         /* 0x30c */
    void*                        mState;                /* 0x370 */
    /* Where the skeeter drops anchor: InitResources and, whenever it is off
       the water, the water-rest and water-chase updates (02131ac4, 02131c48)
       copy mPos here. Patrol (02131648) and 021314a0 aim back at it through
       Vec3_HorzAngle, and patrol turns around past 0x3c0000 from it. Was
       unk_374/378/37c. */
    s32 mAnchorPosX; /* 0x374 */
    s32 mAnchorPosY; /* 0x378 */
    s32 mAnchorPosZ; /* 0x37c */
    /* The four ripple particles func_ov090_02130f94 re-feeds every frame,
       one per leg bone, through Particle::System::New. Was pad_380. */
    u32 mRippleParticles[4]; /* 0x380 */
    /* Loops of the current anim: idle, patrol and water chase count it up
       and leave the state past a threshold. */
    s32 mAnimLoops;                                     /* 0x390 */
    /* Turn timer: patrol loads 0xa and water chase 8 when they turn the
       heading; Behavior counts it down. While it runs, water chase eases the
       heading fast and patrol holds its speed. Was unk_394. */
    u16 mTurnTimer; /* 0x394 */
    /* Speed hold: patrol loads 0x1e after a turn or on spotting a player and
       re-picks speed and anim rate only once this and mTurnTimer are both 0;
       its entry (02131a74) clears it. Behavior counts it down. Was unk_396. */
    u16 mSpeedHoldTimer; /* 0x396 */
    /* Patrol re-jitters the heading by a random step whenever this reaches 0
       and reloads it with a random 0..0x3f; a turn or a spotted player
       reloads it too. Behavior counts it down. Was unk_398. */
    u16 mWanderTimer;                                   /* 0x398 */
    u16 mTargetAngleY;                                  /* 0x39a */
    /* 1 while the skeeter stands on water: the ground probe (02131378) sets
       it when the floor is below the water height and Y is at the water;
       InitResources sets it for a surface with flag 0x20. Idle and patrol go
       to water rest on it; water chase and water rest go idle without it.
       Was unk_39c. */
    u8 mOnWater; /* 0x39c */
    /* Edge latches for patrol's turns, so each turn fires once: chase
       (021314a0, a player in range), far (beyond 0x3c0000 of the anchor),
       wall (dBgCh_Actr::IsOnWall) and probe (the line probe ahead missed).
       Was pad_39d. */
    u8 mChaseLatch; /* 0x39d */
    u8 mFarLatch;   /* 0x39e */
    u8 mWallLatch;  /* 0x39f */
    u8 mProbeLatch; /* 0x3a0 */
    /* func_ov090_021310b4 stores 3 when a 0x4380 hit kills the skeeter
       (mDeathState 3); Behavior's death branch then pitches it over and
       spins it while this is 3. Was unk_3a1. */
    u8 mDeathTumble; /* 0x3a1 */
    u8  pad_3a2[0x2];
    /* Anim playback rate: every state entry sets 0x1000, patrol 0x2000 near
       a player, and Behavior copies it into the frame controller's speed
       each frame. Was unk_3a4. */
    s32 mAnimSpeed; /* 0x3a4 */
    /* Floor height under the skeeter: the ground probe's clsnY (02131378,
       InitResources), data_02092138 when InitResources finds none. Was
       unk_3a8. */
    s32 mFloorY; /* 0x3a8 */
    /* The height it stands at: the water height data_0209f32c when the
       floor is below it, else mFloorY. Y is clamped up to it, ripples sit
       on it and dying at or below it pays the coins. Was unk_3ac. */
    s32 mSurfaceY; /* 0x3ac */

    /* --- vtable --- */
    virtual ~daMenbo_c();

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* State machine: the four nodes pair an entry with an update; both are
       pointer-to-member slots on this class (idle, water chase, water rest,
       patrol). */
    int func_ov090_02131584();
    int func_ov090_02131608();
    int func_ov090_02131648();
    int func_ov090_02131a74();
    int func_ov090_02131ac4();
    int func_ov090_02131b94();
    int func_ov090_02131c48();
    int func_ov090_02131db0();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daMenbo_c_size_must_be_0x3b0[sizeof(daMenbo_c) == 0x3b0 ? 1 : -1];
#endif

#endif /* DAMENBO_C_H */
