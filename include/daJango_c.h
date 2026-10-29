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
       func_ov062_0211b51c read (its hitFlags and otherOwner); [2], which asks for
       no vulnerability bits at all (Init's last argument is 0), is the one whose
       otherOwner is checked for the player's actorID before the cap is taken.
       Radii and height are fix12: [1] is 100 x 160 units, [2] is 60 x 160. */
    dCcAc_c           mdCcAc_c1;  /* 0x110 */
    dCcAc_c           mdCcAc_c2;  /* 0x144 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x178 */
    dExtBlendAnmModel_c mBlendAnmModel;               /* 0x334 */
    dExtShadowModel_c                  mShadowModel;          /* 0x3a4 */
    /* Both matrices are 12 words rather than a Matrix4x3 member: a Matrix4x3
       member makes the synthesised constructor emit a Matrix4x3 destructor that
       the ROM has no symbol for, and the ROM build refuses the object.
       Drop-shadow matrix: func_ov062_0211c6a8 builds it from mPos >> 3 (dropped
       0x18000 = 24 units) every frame and hands it to DropShadowRadHeight. */
    s32                          mShadowMat[12];        /* 0x3cc -- a Matrix4x3 spelled as 12 words */
    /* Where the carried actor is drawn: the bone matrix of the model (entry 6 of
       its transforms) times the model matrix, then a fixed offset and rotation
       that depend on mCarriedItem. func_ov062_0211c6a8 builds it and stores its
       address in the carried actor's +0xc8 word. */
    s32                          mHeldMat[12];          /* 0x3fc -- a Matrix4x3 spelled as 12 words */
    /* Behavior reads this word as a pointer to a pair of pointers-to-member,
       {enter, update}: func_ov062_0211c658(this, record) stores it and calls
       `enter` at once, Behavior calls `update` (the +0x08 member) on `this`
       every frame, and the code compares it by address only against the HURT
       record (Behavior) and the SWOOP record (func_ov062_0211b51c); the other
       records (data_ov062_0211e15c / _0211e16c / _0211e17c) are only passed to
       func_ov062_0211c658. The five records are filled in by
       __sinit_ov062_0211d6fc; src/game/actors/d_a_jango.cpp names them. */
    void                        *mState;                /* 0x42c */
    /* The path node Klepto is at. InitResources has PathPtr::GetNode write the
       node at mPathNodeIndex here (the carrying-a-star branch then teleports the
       actor onto it: `mPosX = mPathNodePosX;` ...), and func_ov062_0211c2f4 sets
       it to the actor's own position each time it snaps onto a node. */
    s32                          mPathNodePosX;         /* 0x430 */
    s32                          mPathNodePosY;         /* 0x434 */
    s32                          mPathNodePosZ;         /* 0x438 */
    /* Two uses on the one word. In func_ov062_0211bd10 (circling) it is a phase
       counter for the animation switch: 0, then 1 when jango_attack.bca is
       started, then 2..0x18 counting up, then back to 0. In the swoop it is the
       turn rate: func_ov062_0211bc54 stores 0x300, 0x400, 0x500 or 0x600 (random)
       and func_ov062_0211ba84 uses it (plus 0x500 outside level id 0x10) as the
       ApproachLinear step for pitch and yaw. func_ov062_0211c218 stores 0;
       func_ov062_0211b930 stores 1, then 0 again when it found a held actor.
       Named for the states that own it, since the two jobs share no role. */
    s32 mStateParam; /* 0x43c */
    /* Countdown that holds the circling state back from leaving along the path:
       func_ov062_0211c218 stores 0, or 2 for the silver-star variant and level ids
       0x18 and 0x19, and func_ov062_0211bd10 takes one off whenever the phase
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
    /* Set to 1 by func_ov062_0211b3ac when the path node it picks is index ^ 2 (assumed
       to be the far side of the square), and only on level id 0x10. It is cleared
       first and recomputed on every b3ac call past the tick gate and the
       no-change check. func_ov062_0211c594
       then gives the actor a 20 unit/frame vertical speed and a 60 frame
       mStateTimer, and func_ov062_0211c2f4 clears it when that timer runs out.
       The 20.0 vector is turned by a -0x4000 pitch, so the leg to the far node
       starts as a climb. */
    u8 mRiseToFarNode; /* 0x446 */
    /* Set to 1 just before func_ov062_0211b51c calls Player::Hurt; the next
       func_ov062_0211c218 (circling state entry) zeroes mStateTimer and clears
       it. Set even if Hurt does nothing. */
    u8                           mHitPlayer;            /* 0x447 */
    /* 2 when the spawn word's item field reads 2 (which then forces
       mCarriedItem to 1). What reads it: the hurt state spawns
       ACTOR_SILVER_STAR instead of ACTOR_STAR, func_ov062_0211b51c never takes
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
    /* Ticked (+1, & 7) at the top of func_ov062_0211b3ac, which only does its
       work when it wraps to 0; func_ov062_0211c2f4 stores 7 so the very next
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
    /* InitResources stores 4: the number of path nodes. func_ov062_0211c2f4
       reads it as the wrap point of the previous-node index, and
       func_ov062_0211b3ac and func_ov062_0211c2f4 both loop or wrap at the
       literal 4. */
    s32                          mPathNodeCount;        /* 0x470 */
    s32                          mPathNodeIndex;        /* 0x474 -- the index handed to PathPtr::GetNode */
    u8  pad_478[0xc];
    /* InitResources copies mPosX/mPosY/mPosZ here once, before any movement. */
    s32                          mSpawnPosX;            /* 0x484 */
    s32                          mSpawnPosY;            /* 0x488 */
    s32                          mSpawnPosZ;            /* 0x48c */

    /* --- vtable --- */
    virtual ~daJango_c() {}

    virtual s32   OnAimedAtWithEgg();      /* slot 29 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* The helpers the five state records' pointer-to-member pairs name (the
       enter/update halves), plus the ones they call. Each takes the object as
       implicit this where the old C sources took it explicitly. The original
       names are not recovered; the addresses stand in. */
    void func_ov062_0211b2fc();    /* circle velocity words */
    int  func_ov062_0211b3ac();    /* pick the next path node */
    int  func_ov062_0211b51c();    /* cylinder contact tests (hurt, cap steal) */
    int  func_ov062_0211b800();    /* RISE update */
    int  func_ov062_0211b880();    /* RISE enter */
    int  func_ov062_0211b8d8();    /* HURT update */
    int  func_ov062_0211b930();    /* HURT enter */
    int  func_ov062_0211bc54();    /* SWOOP enter */
    int  func_ov062_0211bd10();    /* CIRCLE update */
    int  func_ov062_0211c218();    /* CIRCLE enter */
    int  func_ov062_0211c2f4();    /* FLY_PATH update */
    int  func_ov062_0211c594();    /* FLY_PATH enter */
    int  func_ov062_0211c658(void *p); /* select a state record */
    void func_ov062_0211c6a8();    /* per-frame matrices */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Klepto_size_must_be_0x490[sizeof(daJango_c) == 0x490 ? 1 : -1];
#endif

#endif /* DAJANGO_C_H */
