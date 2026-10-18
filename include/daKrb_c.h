#ifndef DAKRB_C_H
#define DAKRB_C_H
#include "types.h"
#include "SharedFilePtr.h"
#include "dCapEnemy_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "ModelAnim.h"
#include "dExtShadowModel_c.h"
#include "MaterialChanger.h"

struct KrbAnimationFileHandle : SharedFilePtr {
    u16 fileID;
    u8 numRefs;
    BCA_File *ptr;

    KrbAnimationFileHandle(u32 fileID);
    ~KrbAnimationFileHandle();
};

typedef char
    KrbAnimationFileHandle_size_must_be_0x08[sizeof(KrbAnimationFileHandle) == 0x08 ? 1 : -1];

/* Goomba (KURIBO 200 / KURIBO_S 201 / KURIBO_L 202) -- ov084/daKrb_c.
 *
 * RTTI ov084:0x021308d4 is the length-prefixed string 7daKrb_c;
 * _ZTI7daKrb_c at 0x021308e0 points its name word there and its base
 * at dCapEnemy_c. The tree used to carry this class under the coined
 * English name Goomba.
 *
 * Factories daKrb_c_classInit_KURIBO / _S / _L allocate 0x478 bytes;
 * that is this class's size. Member NAMES below are from this TU's
 * own methods.
 *
 * SM64DS RTTI names the implementation daKrb_c. The reconstructed
 * factories daKrb_c_classInit_KURIBO / _S / _L (historical aliases
 * Goomba_Spawn / GoombaSmall_Spawn / GoombaLarge_Spawn) install this
 * class's cartridge vtable; the reconstructed profile globals
 * g_profile_KURIBO / _S / _L are the registry descriptors. Exact
 * original SM64DS member spellings are not preserved.
 */

struct Player;

struct daKrb_c : dCapEnemy_c {
    dCcAc_c mdCcAc_c;         /* 0x180 */
    dBgCh_Actr mWithMeshClsn; /* 0x1b4 */
    ModelAnim mModelAnim;     /* 0x370 */
    dExtShadowModel_c mShadowModel; /* 0x3d4 */
    MaterialChanger mMaterialChanger; /* 0x3fc */
    /* Last position that IsGoingOffCliff did not reject; Behavior snaps back to
       it when the next step would carry the Goomba off a ledge. */
    Vector3 mSafePos;       /* 0x410 */
    /* Spawn position, copied in by InitResources. A Goomba lost in water,
       toxic ground or quicksand, stuck for 300 frames, or killed while capped
       or carrying a pending star is moved back here (RespawnIfHasCap).
       daKuriKing_c overwrites it for its minions. */
    Vector3 mHomePos;       /* 0x41c */
    /* Position sampled by Behavior to decide whether the Goomba is stuck. */
    Vector3 mStuckCheckPos; /* 0x428 */
    s32 mState;            /* 0x434 -- a State */
    /* Result of ClosestPlayer(), refreshed by func_ov084_02129cf4. */
    Player *mClosestPlayer;    /* 0x438 */
    /* uniqueID of the Goomba King for a king minion (stored by daKuriKing_c). */
    s32 mTargetUniqueID;    /* 0x43c */
    /* Distance to the target the Goomba reacts to (it chases or flees from the
       player), 0x61a8000 (25000.0 units) when there is none; 0x7fffffff until
       the first func_ov084_02129cf4 call. */
    s32 mDistToPlayer;      /* 0x440 */
    /* Horizontal speed mHorzSpeed is being driven toward. Walk speed is
       data_ov084_02130228[type], run speed data_ov084_02130268[type]. */
    s32 mTargetHorzSpeed;   /* 0x444 */
    /* Range inside which a Goomba notices the player (a capped Goomba flees,
       a capless one chases), set by
       func_ov084_021290d4: 500.0 units, shrinking for a capless Goomba that
       has been stuck more than 10 frames. */
    s32 mChaseRadius;       /* 0x448 */
    /* param1 as InitResources left it. The exit of STATE_TUMBLE stores this
       word into mFlags (0xb0), not back into param1. */
    s32 mSavedParam;            /* 0x44c */
    /* Wander: frames left before a new heading is chosen. */
    s16 mHeadingHoldTimer;  /* 0x450 */
    /* STATE_TUMBLE: bounce countdown, 60 frames when the tumble starts. */
    u16 mBounceCountdown;   /* 0x452 */
    s16 mWanderRerollTimer; /* 0x454 */
    /* STATE_WALK: frames counted while the Goomba stays within 10.0 units of
       mStuckCheckPos; reset when it moves farther. */
    u16 mStuckTimer;            /* 0x456 */
    /* Unstick countdown. Behavior sets it to 90 when a Goomba with a cap has
       been stuck 30 frames, and func_ov084_0212abd4 sets it to mStuckTimer for
       a capless one stuck longer than 30; func_ov084_021290d4 counts it down.
       While it runs the Goomba is steered by its own branch of
       func_ov084_0212abd4 and the chase radius is left alone. */
    u16 mUnstickTimer;      /* 0x458 */
    /* Spawn heading, later overwritten with the heading toward the chase target
       or home (func_ov084_02129cf4). */
    s16 mInitAngleY;            /* 0x45a */
    /* Heading mPrevAngleY is turned toward. */
    s16 mTargetAngleY;      /* 0x45c */
    u8  pad_45e[0x2];
    s32 mGoombaType;            /* 0x460 -- a GoombaType */
    u8  mRewardType;            /* 0x464 -- a RewardType */
    s8  mStarTracked;            /* 0x465 */
    u8  mStarID;            /* 0x466 */
    /* Bounce counter in STATE_TUMBLE (sound chosen by it); cleared on entry. */
    u8  unk_467;            /* 0x467 */
    u8  mMoveFlags;   /* 0x468 -- MoveFlags */
    u8  pad_469[0x3];
    /* hitFlags of the last hit reaction, saved by func_ov084_02129ed4. */
    u32 mLastHitFlags;      /* 0x46c */
    u8  pad_470[0x4];
    /* Slot among the king's minions; daKuriKing_c stores its running spawn
       count here and the Goomba hands it to func_ov074_0212087c. */
    u8  mMinionIndex;       /* 0x474 */
    /* Reloaded with 30 while the king's mState is 4 and mWalkSpeed is 0. */
    u8  unk_475;            /* 0x475 */
    u8  pad_476[0x2];

    /* OUT OF LINE, DECLARED FIRST. `#pragma defer_codegen off` in the TU
       emits D1 then D0 then homeless D2, the cartridge's order. */
    virtual ~daKrb_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    int OnYoshiTryEat();                        /* slot 18 */
    void OnTurnIntoEgg(Player &player);          /* slot 19 */
    int OnAimedAtWithEgg();                     /* slot 29 */

    void func_ov084_021290d4();
    void func_ov084_02129168(dActor_c *actor);
    void func_ov084_02129238();
    void func_ov084_0212934c();
    void func_ov084_02129498();
    void func_ov084_021294d0();
    void func_ov084_021296cc();
    void func_ov084_02129864();
    int func_ov084_021298d0();
    int func_ov084_02129a00();
    void func_ov084_02129c9c();
    void func_ov084_02129cf4(Fix12i distThresh);
    void func_ov084_02129ed4();
    void func_ov084_0212a6f8();
    void func_ov084_0212a774();
    void func_ov084_0212ab48();
    void func_ov084_0212abd4();
    void func_ov084_0212af74();
    void func_ov084_0212b2dc();
    void func_ov084_0212a580();
    void func_ov084_0212aab0();

    /* Size class, stored in mGoombaType by InitResources from the actor id
       (KURIBO_S 201 -> 0, KURIBO 200 -> 1, KURIBO_L 202 -> 2). Type 3 is the
       KURIBO spawned by the Goomba King (daKuriKing_c): it is created with
       spawn parameter 0xeeee or 0xeeef, keeps the king's uniqueID in
       mTargetUniqueID and walks toward a spot func_ov074_0212087c derives from
       the king and mMinionIndex. The per-type tables at
       data_ov084_02130204..02130268 are indexed by this value. */
    enum GoombaType {
        GOOMBA_SMALL = 0,
        GOOMBA_NORMAL = 1,
        GOOMBA_LARGE = 2,
        GOOMBA_KING_MINION = 3
    };

    /* mState indexes the five-entry record table data_ov084_02130d74 that
       __sinit_ov084_0213035c fills in this order; Behavior calls the record's
       pointer-to-member. The names say what each handler does. */
    enum State {
        STATE_WALK = 0,       /* func_ov084_0212b2dc: wander, chase, or (minion) follow the king */
        STATE_HOP_START = 1,  /* func_ov084_0212ab48: start a hop (-> STATE_AIRBORNE); set by func_ov084_02129ed4 when the Goomba touches the player in STATE_WALK */
        STATE_AIRBORNE = 2,   /* func_ov084_0212aab0: in the air, turning toward mTargetAngleY */
        STATE_TUMBLE = 3,     /* func_ov084_0212a774: thrown clear (func_ov084_02129168) after the cap came off or a Yoshi-held Goomba landed */
        STATE_PAUSE = 4       /* func_ov084_0212a6f8: stand still until mWanderRerollTimer runs out; nothing in this TU enters it */
    };

    /* mRewardType, from bits 4..7 of the spawn parameter. Types 1 and 2 both
       load the silver-star model and spawn a SILVER_STAR (179) plus a STARBASE
       (180) when the Goomba dies; type 2 only does so when mStarID equals
       data_0209f344[data_0209f208[0]], and becomes 3 once it has. */
    enum RewardType {
        REWARD_NONE = 0,
        REWARD_SILVER_STAR = 1,
        REWARD_SILVER_STAR_IF_CURRENT = 2,
        REWARD_SPENT = 3
    };

    /* mDeathState (dEnemyBase_c) values this class writes in its hit reaction
       (func_ov084_02129ed4), named for the hit that sets them. */
    enum DeathState {
        DEATH_NONE = 0,
        DEATH_STOMPED = 1,       /* jumped on, or a spin / ground-pound hit */
        DEATH_PUNCHED = 2,
        DEATH_KICKED = 3,        /* kick, breakdance or slide kick (hit bits 0x380) */
        DEATH_FIRE = 4,
        DEATH_DIVE_OR_EGG = 5,   /* dive or egg hit (bits 0x2400), or touched by a Player for whom IsOnShell() is true */
        DEATH_EXPLOSION = 6,
        DEATH_HIT_20000 = 7      /* hit bit 0x20000 (not in dCc_c.h's table), or a hit reported during the Yoshi-eat update or STATE_TUMBLE */
    };

    /* unk_108 (dEnemyBase_c) is the kind of coin SpawnCoin drops: its value
       minus one indexes the u16 table at data_ov002_020ff014 = {288 COIN,
       289 RED_COIN, 290 BLUE_COIN}. 0 drops nothing. */
    enum CoinKind {
        COIN_NONE = 0,
        COIN_PLAIN = 1,
        COIN_RED = 2,
        COIN_BLUE = 3
    };

    /* mMoveFlags bits. */
    enum MoveFlags {
        MOVE_AVOIDING = 1,        /* last AngleAwayFromWallOrCliff call turned the Goomba away */
        MOVE_STEP_SOUND_LATCH = 2 /* footstep sound already played for this step */
    };
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKrb_c_size_must_be_0x478[sizeof(daKrb_c) == 0x478 ? 1 : -1];
#endif

#endif
