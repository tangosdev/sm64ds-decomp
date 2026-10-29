#ifndef DAKING_DONKETU_C_H
#define DAKING_DONKETU_C_H

#include "types.h"

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dExtShadowModel_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

/* Chief Chilly (KING_DONKETU 218) -- ov073/daKing_Donketu_c.
 *
 * RTTI ov073:0x02123058 is the length-prefixed string 16daKing_Donketu_c;
 * _ZTI16daKing_Donketu_c at 0x0212304c points its name word there and its
 * base at dEnemyBase_c. The tree used to carry this class under the coined
 * English name ChiefChilly. Factory allocates 0x504.
 *
 * daKing_Donketu_c_classInit is reconstructed (RTTI daKing_Donketu_c,
 * KING_DONKETU registry). Retail does not store that spelling.
 * Historical alias: ChiefChilly_Spawn. */

struct daKing_Donketu_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;                                /* 0x110 */
    dBgCh_Actr mWithMeshClsn;                              /* 0x150 */
    /* The boss's model and skeleton animation. The state handlers read the
       bone-matrix array (data.transforms, 0x320) and the model's own matrix
       (mat4x3, 0x328) to find where to put particles, treat its dExtFrameCtrl_c
       base (0x35c) as the clock they wait on, and set its playback speed
       (speed, 0x368; 0x1000 is 1.0). */
    dExtBlendAnmModel_c mBlendAnmModel; /* 0x30c */
    /* One state is two member-function pointers: the enter member runs once
       when the state is installed, the update member every frame. The sixteen
       records at data_ov073_02123320 .. data_ov073_02123410 are filled in by
       __sinit_ov073_02122874. */
    struct State {
        int (daKing_Donketu_c::*enter)();
        int (daKing_Donketu_c::*update)();
    };

    /* The current state: the address of one of the sixteen state records
       data_ov073_02123320 .. data_ov073_02123410, each a pair of
       pointers-to-member (enter, then update). ChiefChilly_ChangeState
       stores it and runs the enter member; Behavior runs the update member
       every frame and compares this against the records by ADDRESS to decide
       what else the frame does. */
    void *mState;                                          /* 0x37c */
    dExtShadowModel_c mShadowModel;                              /* 0x380 */
    /* The matrix func_ov073_021215cc hands DropShadowRadHeight: a pure
       translation to the boss's position >> 3, with y taken 0xa000 (10
       units) lower first. */
    Matrix4x3 mShadowMtx;                                  /* 0x3a8 */
    /* InitResources copies mPosX/mPosY/mPosZ here once, in the same breath as
       seeding both waypoint arrays with the same position. The part-2 cut-scene
       camera looks at it, the part-1 camera is placed relative to it, the key
       spawns above it, and the ..3390 and leap states turn toward it. Whether
       it is the middle of the arena is not established. */
    s32 mSpawnPosX;                                        /* 0x3d8 */
    s32 mSpawnPosY;                                        /* 0x3dc */
    s32 mSpawnPosZ;                                        /* 0x3e0 */
    /* The player the talk cut scenes address: the opening-talk state
       (..3330) and the second cut scene (..33e0) store their ClosestPlayer
       result here, and the states that wait out a talk (..3350, ..3410) ask
       it for its talk state. */
    Player *mTalkPlayer;                                   /* 0x3e4 */
    /* Two eight-entry waypoint sets, both seeded with the boss's own position by
       InitResources and refilled by daObjEwbIce_c::StateReport_Update. The steering helpers
       (func_ov073_021203ac / _02120610) index one of them with mWaypointCursor
       and hand the element to Vec3_HorzAngle/Vec3_VertAngle as the point to
       turn towards -- set B while mHitsRemaining reads 2, set A otherwise. */
    Vector3 mWaypointsA[8];                                /* 0x3e8 */
    Vector3 mWaypointsB[8];                                /* 0x448 */
    /* A copy of mGroundMissPos, taken by the enter handler of the state at
       data_ov073_021233d0; that state's update handler walks the boss
       horizontally toward a point 120.0 from it along mAngleY + 0x8000.
       mGroundMissPos is where the forward ground ray found no floor, so
       this is the arena edge the boss teeters at in that state. */
    s32 mEdgePosX; /* 0x4a8 */
    s32 mEdgePosY; /* 0x4ac */
    s32 mEdgePosZ; /* 0x4b0 */
    /* Scratch word the states use for themselves (most enter handlers that
       rely on it zero it; ..3360 zeroes it from its update instead): a 0/1 latch in several, a frame count that
       runs up to 0x82 in the state at data_ov073_02123340, and in the state
       at data_ov073_021233d0 a phase angle that advances 0x500 a frame and
       indexes the data_02082214 sine table. */
    s32 mStateScratch;                                     /* 0x4b4 */
    /* Hops counted by the waypoint-hop states: the update handler of the
       state at data_ov073_02123320 adds one per landing and switches to
       data_ov073_02123340 once it passes 7; the update handler of the state
       at data_ov073_02123400 sets it to 1 on its landing, and that state's
       enter handler zeroes it. */
    s32 mJumpCount;                                        /* 0x4b8 */
    /* Index of the model bone func_ov073_0211f2c0 combines with the model
       matrix to place its landing/step particles; it flips the index with `^= 1` after
       every call, and InitResources seeds it with 2, so it alternates 2, 3,
       2, ... (func_ov073_021215cc builds mFootPos from bones 2 and 3). */
    s32 mStepBoneIdx;                                      /* 0x4bc */
    /* Coins func_ov073_0211f61c has spawned on hits, counted only while
       SaveData::IsCharacterUnlocked(2) holds (SaveData.h: bit 2 is Wario);
       the spawn that takes it past 0x1e (30) also spawns a SCALEUP_KINOKO
       and zeroes it. */
    s32 mCoinsSpawned;                                     /* 0x4c0 */
    /* Index into mWaypointsA / mWaypointsB. func_ov073_021203ac advances it
       (+1, modulo 8) at the launch frame of each hop. */
    u8  mWaypointCursor;                                   /* 0x4c4 */
    /* InitResources stores 0xff, several state handlers store 0xff, ..3340's wait stores 0, and the
       two hop-landing handlers (func_ov073_021203ac / _02120610) copy
       mWaypointCursor into it; nothing in this file reads it. The reader
       is d_a_obj_ewb_ice.cpp's touch helper (daObjEwbIce_c::OnStoodOn): a rink
       ice piece touched by the boss moves on to its next state only when
       its spawn index equals this, i.e. the waypoint he just landed on
       (0xff: none). */
    u8 mLandedWaypoint; /* 0x4c5 */
    /* The heading (angle, 0x10000 = a full turn) the chase and
       return-to-arena states turn mPrevAngleY toward: the angle to the
       player, or to mSpawnPos. */
    s16 mTargetAngle;                                      /* 0x4c6 */
    /* Which leg of the state at data_ov073_02123360 (the chase state) is
       running; see func_ov073_02120ed0. Several enter handlers zero it. */
    u8  mPhase;                                            /* 0x4c8 */
    /* The forward ground ray found nothing while the boss was moving fast.
       Behavior sets it on a miss and clears it on a hit; func_ov073_02120ed0
       tests it for 1. */
    u8  mNoGroundAhead;                                    /* 0x4c9 */
    /* Set to 1 by func_ov073_02120910 together with mHitsRemaining = 1 and
       cleared by the enter handler of the state at data_ov073_02123380.
       func_ov073_021200e0 reads it twice: it plays a sound on the hop's
       launch frame while it is set, and the 0x82-frame wait after the
       landing runs only while it is clear. func_ov073_02120910 is the
       teeter-at-the-edge state (see mEdgePos), so the flag marks the leap
       back from the edge. Medium confidence. */
    u8 mEdgeRecovered; /* 0x4ca */
    /* A counter that starts at 3 (InitResources). func_ov073_02120c7c, the
       hit-reaction state, decrements it when mNoGroundAhead is 1 and takes the
       state at data_ov073_021233d0 when it reaches 0; func_ov073_02120910
       forces it to 1. A value of 2 selects waypoint set B and a larger
       horizontal speed, and Behavior's ground ray reaches twice as far while it
       is above 1. "Hits left" is the reading, not something the code states:
       it counts times the ground ray missed in the hit-reaction state, which
       a hit sends the boss to. */
    u8  mHitsRemaining;                                    /* 0x4cb */
    /* Frames left before the next hit can register: func_ov073_0211f61c counts
       it down with DecIfAbove0_Short, reports no hit while it is still nonzero,
       and sets it to 0x10 when a hit lands. */
    s16 mHitCooldown;                                      /* 0x4cc */
    u8  pad_4ce[0x2];
    /* Two jobs: the step the chase state's braking legs hand ApproachLinear
       to slow mHorzSpeed to zero (0x1000 or 0x2000), and the amplitude the
       state at data_ov073_021233d0 multiplies its sine by to rock
       mAngleX (its enter handler stores -0x1000). Named for the state that
       sets it, like mStateScratch, since the two jobs share no one role. */
    s32 mStateParam; /* 0x4d0 */
    /* Positions of bones 2 and 3, rebuilt each frame by func_ov073_021215cc
       (the model matrix times the bone matrix, shifted left by 3);
       func_ov073_02120c7c and func_ov073_02120ed0 pass both to
       RunningSlidingDustAt while the boss is sliding. Bones 2 and 3 are
       also the pair mStepBoneIdx alternates between for the landing/step
       particles, which places them at the feet. */
    Vector3 mFootPos[2]; /* 0x4d4 */
    /* Where the boss WAS when the ground ray missed: Behavior stores the live
       position here and then, except in the state at data_ov073_021233a0,
       rewinds mPos to mPrevPos. The enter handler of the
       state at data_ov073_021233d0 copies it into mEdgePos. */
    s32 mGroundMissPosX;                                   /* 0x4ec */
    s32 mGroundMissPosY;                                   /* 0x4f0 */
    s32 mGroundMissPosZ;                                   /* 0x4f4 */
    /* Particle unique ids: func_ov073_0211f144 renews both every call (they
       come back through Particle::System::New) with effects 0x77 and 0x78. */
    u32 mParticleId0;                                      /* 0x4f8 */
    u32 mParticleId1;                                      /* 0x4fc */
    /* The handle Sound::PlayLong returns for the looping sound several states
       keep alive; they pass it back in every frame. */
    u32 mSoundHandle;                                      /* 0x500 */

    /* OUT OF LINE, DECLARED FIRST. `#pragma defer_codegen off` in the TU
       emits D1 then D0 then homeless D2, the cartridge's order. */
    virtual ~daKing_Donketu_c();

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    void func_ov073_0211f144();
    int func_ov073_0212081c();
    void func_ov073_0211f2c0(int strength);
    void func_ov073_0211f494(void *pb);
    s32 func_ov073_0211f61c();
    int func_ov073_0211fa74();
    int func_ov073_0211fbec();
    int func_ov073_0211fbf4();
    int func_ov073_0211fc70();
    int func_ov073_0211fc78();
    int func_ov073_0211fe84();
    int func_ov073_0211fe8c();
    short func_ov073_0212000c();
    int func_ov073_0212005c();
    int func_ov073_02120098();
    int func_ov073_021200e0();
    int func_ov073_02120390();
    int func_ov073_021203ac();
    int func_ov073_021205f0();
    int func_ov073_02120610();
    int func_ov073_02120844();
    int func_ov073_021208e4();
    int func_ov073_02120910();
    int func_ov073_02120ad8();
    int func_ov073_02120b78();
    int func_ov073_02120c08();
    int func_ov073_02120c7c();
    int func_ov073_02120d80();
    int func_ov073_02120dec();
    int func_ov073_02120e60();
    int func_ov073_02120ed0();
    int func_ov073_0212122c();
    int func_ov073_0212128c();
    int func_ov073_02121378();
    int func_ov073_02121388();
    int func_ov073_02121538();
    void func_ov073_021215cc();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKing_Donketu_c_size_must_be_0x504[
    sizeof(daKing_Donketu_c) == 0x504 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAKING_DONKETU_C_H */
