#ifndef DAJANGO_C_H
#define DAJANGO_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN9daJango_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first. Member NAMES are the ones this header already used --
 * a rebase should not also rename things its callers spell:
 *
 *     0x110 dCcAc_c       0x34   -> 0x144
 *     0x144 dCcAc_c       0x34   -> 0x178
 *     0x178 dBgCh_Actr             0x1bc  -> 0x334
 *     0x334 dExtBlendAnmModel_c           0x70   -> 0x3a4
 *     0x3a4 dExtShadowModel_c              0x28   -> 0x3cc
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daJango_c. The reconstructed factory
 * daJango_c_classInit (0x0211ce80, historical alias Klepto_Spawn)
 * installs this class's cartridge vtable and lives in
 * src/game/actors/d_a_jango.cpp alongside the rest of the class; the
 * reconstructed profile global g_profile_JANGO (historical alias
 * Klepto_SpawnInfo) is its registry descriptor.
 */

#include "dEnemyBase_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "PathPtr.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

struct daJango_c : dEnemyBase_c {
    /* Two cylinders, both owned by this actor. [1] is the one the attack tests in
       HandleCylinderHit read (its hitFlags and otherOwner); [2], which asks for
       no vulnerability bits at all (Init's last argument is 0), is the one whose
       otherOwner is checked for the player's actorID before the cap is taken.
       Radii and height are fix12: [1] is 100 x 160 units, [2] is 60 x 160. */
    dCcAc_c           mdCcAc_c1;  /* 0x110 */
    dCcAc_c           mdCcAc_c2;  /* 0x144 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x178 */
    dExtBlendAnmModel_c mBlendAnmModel;               /* 0x334 */
    dExtShadowModel_c                  mShadowModel;          /* 0x3a4 */
    /* Both Matrix4x3 members make the synthesised constructor emit Matrix4x3's
       vague-linkage destructor, which the ROM has no symbol for; the TU
       manifest deadstrips it, as for daKpFr_c.
       Drop-shadow matrix: UpdateModelAndShadow builds it from mPos >> 3 (dropped
       0x18000 = 24 units) every frame and hands it to DropShadowRadHeight. */
    Matrix4x3 mShadowMat; /* 0x3cc */
    /* Where the carried actor is drawn: the bone matrix of the model (entry 6 of
       its transforms) times the model matrix, then a fixed offset and rotation
       that depend on mCarriedItem. UpdateModelAndShadow builds it and stores its
       address in the carried actor's +0xc8 word. */
    Matrix4x3 mHeldMat; /* 0x3fc */
    /* Behavior reads this word as a pointer to a pair of pointers-to-member,
       {enter, update}: ChangeState(this, record) stores it and calls
       `enter` at once, Behavior calls `update` (the +0x08 member) on `this`
       every frame, and the code compares it by address only against the HURT
       record (Behavior) and the SWOOP record (HandleCylinderHit); the other
       records (data_ov062_0211e15c / _0211e16c / _0211e17c) are only passed to
       ChangeState. The five records are filled in by
       __sinit_ov062_0211d6fc; src/game/actors/d_a_jango.cpp names them. */
    void                        *mState;                /* 0x42c */
    /* The path node Klepto is at. InitResources has PathPtr::GetNode write the
       node at mPathNodeIndex here (the carrying-a-star branch then teleports the
       actor onto it: `mPosX = mPathNodePosX;` ...), and UpdateFlyPathState sets
       it to the actor's own position each time it snaps onto a node. */
    union {
        struct {
            s32 mPathNodePosX, mPathNodePosY, mPathNodePosZ;
        };
        Vector3 mPathNodePos; /* 0x430 */
    };
    /* Two uses on the one word. In UpdateCircleState (circling) it is a phase
       counter for the animation switch: 0, then 1 when jango_attack.bca is
       started, then 2..0x18 counting up, then back to 0. In the swoop it is the
       turn rate: EnterSwoopState stores 0x300, 0x400, 0x500 or 0x600 (random)
       and UpdateSwoopState uses it (plus 0x500 outside level id 0x10) as the
       ApproachLinear step for pitch and yaw. EnterCircleState stores 0;
       EnterHurtState stores 1, then 0 again when it found a held actor.
       Named for the states that own it, since the two jobs share no role. */
    s32 mStateParam; /* 0x43c */
    /* Countdown that holds the circling state back from leaving along the path:
       EnterCircleState stores 0, or 2 for the silver-star variant and level ids
       0x18 and 0x19, and UpdateCircleState takes one off whenever the phase
       counter above moves from 1 to 2 (again only for those variants). The actor
       leaves for the path only when this and mStateTimer are both 0 and mAngleY
       is within 45 degrees of the node's direction. */
    s32 mLeaveDelay; /* 0x440 */
    /* Counted down by DecIfAbove0_Short at the top of every frame. While it is
       non-zero the circling state flies the actor back toward mSpawnPos; it is
       reloaded with 0x1e (30 frames) whenever the actor has just lost its
       target or its cargo: the carried actor is lost, the player was hit, the
       hurt animation ended, the swoop ran out, the player vanished. */
    u16                          mTimer;                /* 0x444 */
    /* Set to 1 by ChangeTarget when the path node it picks is index ^ 2 (assumed
       to be the far side of the square), and only on level id 0x10. It is cleared
       first and recomputed on every b3ac call past the tick gate and the
       no-change check. EnterFlyPathState
       then gives the actor a 20 unit/frame vertical speed and a 60 frame
       mStateTimer, and UpdateFlyPathState clears it when that timer runs out.
       The 20.0 vector is turned by a -0x4000 pitch, so the leg to the far node
       starts as a climb. */
    u8 mRiseToFarNode; /* 0x446 */
    /* Set to 1 just before HandleCylinderHit calls daPly_c::Hurt; the next
       EnterCircleState (circling state entry) zeroes mStateTimer and clears
       it. Set even if Hurt does nothing. */
    u8                           mHitPlayer;            /* 0x447 */
    /* 2 when the spawn word's item field reads 2 (which then forces
       mCarriedItem to 1). What reads it: the hurt state spawns
       ACTOR_SILVER_STAR instead of ACTOR_STAR, HandleCylinderHit never takes
       the player's cap for it, Behavior does not remove it when off screen,
       and the circling and path states treat it like level ids 0x18 / 0x19.
       The name rests on the ACTOR_SILVER_STAR spawns. */
    u8                           mSilverStarFlag;       /* 0x448 */
    u8  pad_449[0x1];
    /* The yaw the actor is steering its mPrevAngleY toward. Written with
       Vec3_HorzAngle toward the spawn point or the player (InitResources, the
       circling and swoop states), with mAngleY on entering CIRCLE, or with
       mPrevAngleY + 0x4000 after a wall hit, and read as the target of
       ApproachLinear / AngleDiff. */
    s16                          mTargetAngleY;         /* 0x44a */
    /* The uniqueID of the held actor (not its actorID); 0 when nothing is held. */
    s32                          mHeldActorID;          /* 0x44c */
    /* The position Behavior writes into the held actor (about Klepto's own mPos)
       every frame; where the held actor is drawn comes from mHeldMat. */
    s32                          mHeldPosX;             /* 0x450 */
    s32                          mHeldPosY;             /* 0x454 */
    s32                          mHeldPosZ;             /* 0x458 */
    u8  pad_45c[0x4];
    /* Ticked (+1, & 7) at the top of ChangeTarget, which only does its
       work when it wraps to 0; UpdateFlyPathState stores 7 so the very next
       call runs. */
    s32 mPathPickTick;                                  /* 0x460 */
    s32                          mPathId;               /* 0x464 */
    /* 0 (CARRIES_NO_STAR): no star; starts out empty-handed and goes for the
       player's cap, after which mHeldActorID is the cap. 1 (CARRIES_STAR):
       carries a star (ACTOR_STAR, or ACTOR_SILVER_STAR when mSilverStarFlag is
       2) and flies the path. */
    s32                          mCarriedItem;          /* 0x468 */
    /* (param1 >> 12) & 0xf, used for one thing only: it is OR-ed with 0x50 to
       build the spawn parameter of the actor daJango_c is carrying (and with
       0x40 when the star is let go). */
    s32                          mHeldItemParam;        /* 0x46c */
    /* InitResources stores 4: the number of path nodes. UpdateFlyPathState
       reads it as the wrap point of the previous-node index, and
       ChangeTarget and UpdateFlyPathState both loop or wrap at the
       literal 4. */
    s32                          mPathNodeCount;        /* 0x470 */
    s32                          mPathNodeIndex;        /* 0x474 -- the index handed to PathPtr::GetNode */
    u8  pad_478[0xc];
    /* InitResources snapshots mPos before movement. Keep the component names
       and vector view over this same 0x484-0x490 storage. */
    union {
        struct {
            s32 mSpawnPosX, mSpawnPosY, mSpawnPosZ;
        };
        Vector3 mSpawnPos; /* 0x484 */
    };

    /* --- vtable --- */
    virtual ~daJango_c() {}

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* The helpers the five state records' pointer-to-member pairs name (the
       enter/update halves), plus the ones they call. The cartridge keeps no
       names for them; the N64 klepto.inc.c function each one stands in for
       is noted where there is one. */
    void UpdateCircleVelocity(); /* circle velocity words */
    int ChangeTarget();          /* klepto_change_target: pick the next path node */
    int HandleCylinderHit();     /* cylinder contact tests (hurt, cap steal) */
    int UpdateRiseState();       /* RISE update */
    int EnterRiseState();        /* RISE enter */
    int UpdateHurtState();       /* HURT update: klepto_act_struck_by_mario */
    int EnterHurtState();        /* HURT enter */
    int EnterSwoopState();       /* SWOOP enter */
    int UpdateSwoopState();      /* SWOOP update: klepto_act_dive_at_mario */
    int UpdateCircleState();     /* CIRCLE update: klepto_circle_target */
    int EnterCircleState();      /* CIRCLE enter */
    int UpdateFlyPathState();    /* FLY_PATH update: klepto_approach_target */
    int EnterFlyPathState();     /* FLY_PATH enter */
    int ChangeState(void *p);    /* select a state record */
    void UpdateModelAndShadow(); /* per-frame matrices */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Klepto_size_must_be_0x490[sizeof(daJango_c) == 0x490 ? 1 : -1];
#endif

#endif /* DAJANGO_C_H */
