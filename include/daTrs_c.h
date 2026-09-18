/* Boo (TERESA, actor 0xd1) and Big Boo (BOSS_TERESA, actor 0xd2): the
 * Big Boo's Haunt chase ghost, ov063/daTrs_c.
 *
 * Derives from dCapEnemy_c: this class's own destructor tears down its six
 * members, then chains to _ZN11dCapEnemy_cD2Ev -- the exact function the
 * sibling daKrb_c's destructor calls -- and the factories call
 * _ZN11dCapEnemy_cC2Ev before storing _ZTV7daTrs_c.
 *
 * SIZE 0x5e0, both factories' own literal. dCapEnemy_c ends at 0x180;
 * everything from there down is daTrs_c's own. The six members close exactly
 * on one another, in construction and (reversed) in teardown order:
 *
 *     0x184 dCcAcPos_c   0x40  -> 0x1c4
 *     0x1c4 dBgCh_Actr                0x1bc -> 0x380
 *     0x380 ModelAnim                   0x64  -> 0x3e4
 *     0x3e4 Model                       0x50  -> 0x434
 *     0x434 ShadowModel (1st)           0x28  -> 0x45c
 *     0x45c ShadowModel (2nd)           0x28  -> 0x484
 *
 * Slots 18 (OnYoshiTryEat) and 29 (OnAimedAtWithEgg) are this class's own
 * overrides -- see include/dActor_c.h for the slot table.
 *
 * No daTrs_c() constructor is declared: the factories build the object
 * field-by-field through `return new daTrs_c()` (base C2, vtable store, then
 * each member's own constructor), so declaring one risks an implicit body the
 * compiler would inline somewhere the ROM does not.
 *
 * ~daTrs_c() is declared but not defined in-class, so _ZN7daTrs_cD1Ev /
 * _ZN7daTrs_cD0Ev are the compiler's own synthesized bodies. The D1/D0 pair
 * stays in two one-function files: a single out-of-line definition emits
 * D2/D0/D1 (see the measurement on include/daObjBk_Dossunbar_c.h), and the
 * inline-in-header form would home both bodies in the key-function TU, which
 * cannot license the 0x02115ee0 range under one-range-per-file delinks.
 *
 * unk_5cc (state) and unk_5cf (variant) keep their placeholder spellings:
 * the ov063 state helper src/game/actors/daTrs_c/func_ov063_021166ac.cpp
 * names them, and renaming is that TU's follow-up, not this header's.
 * Their roles are documented below, not in the spelling.
 */
#ifndef DATRS_C_H
#define DATRS_C_H
#include "types.h"

#include "dCapEnemy_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* named (not anonymous inline) so check_header_offsets can parse the member
   declaration; layout identical. Bits, proven by use:
   b0: set when the course is 0xc and the spawn matches (InitResources); gates
       the z clamp in Behavior. b1: body model drawn (Render, helper).
   b2: set for variants 5/0/1/2/6/8..0xb and 7; gates the yaw copy in
       Behavior. b3: drawn at all (Render, helper); cleared on invincible
       death. b4: set when the player is high (variants 4/0xb) and in the
       hide block. b5: ground found (Behavior raycast). b6: unused. b7: set in
       the hide block; while clear the Boo keeps hiding. */
struct BooFlags16 { u16 b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char BooFlags16_size_must_be_0x2[sizeof(BooFlags16) == 0x2 ? 1 : -1];
#endif

struct daTrs_c : dCapEnemy_c {
    /* Zeroed by Init; 02116244 spawns the 0xd3 follower once it reaches 5,
       02119074 reloads 0xa, 0211934c compares == 5 on a second Boo. */
    s32 mTimer180;                 /* 0x180 */
    dCcAcPos_c mdCcAcPos_c;  /* 0x184 */
    dBgCh_Actr               mWithMeshClsn;               /* 0x1c4 */
    ModelAnim                  mModelAnim;                  /* 0x380 */
    /* Not "mModel": dCapEnemy_c's own Model at 0x114 is inherited, and calling
       this one mModel shadows it. This is the Boo's own body model: Render
       draws it at mBodyScaleX and both destructors run Model::D1 over it. */
    Model                      mBodyModel;                  /* 0x3e4 */
    ShadowModel                mShadowModel1;               /* 0x434 */
    ShadowModel                mShadowModel2;               /* 0x45c */
    /* Refreshed from ClosestPlayer every Behavior; the angle/distance below
       are measured against it. */
    Player *mClosestPlayer;    /* 0x484 */
    /* The player this Boo is talking to (02117b0c StartTalk/GetTalkState). */
    Player *mTalkPlayer;       /* 0x488 */
    /* Scratch for FindWithID results; every user clears it after the use. */
    dActor_c *mFoundActor;     /* 0x48c */
    /* uniqueID of the 0xd/0xe Big Boo, resolved by 02119c58 by scanning the
       0xd2 actors; re-found by 0211934c. */
    u32 mBigBooID;             /* 0x490 */
    /* uniqueID of the spawner: variant 3's 02116bf4 stores its own id in each
       child it spawns, and CleanupResources bumps that parent's mChildDeaths. */
    u32 mSpawnerID;            /* 0x494 */
    /* uniqueID of the actor 0x9f, resolved on demand by 02119c18. */
    u32 mCachedActorID;        /* 0x498 */
    /* uniqueID of the spawned 0xd3 follower (Init, 02116244); Behavior
       re-resolves it every frame and drags it along. Cleanup destroys it. */
    u32 mSpawnedActorID;       /* 0x49c */
    /* What this Boo shows: the actor id variant 5 spawns (0xd4), and the
       coin-model selector for variants 0/1/2/6/8..0xb (0x120/0x121/0x122;
       0x122 loads the blue coin model). 0x187 is the unset default. */
    u16 mCarriedID;            /* 0x4a0 */
    u8  pad_4a2[0x2];
    /* Twelve words, not a Matrix4x3: this header is included by files that do
       not pull common.h (see daBmb_c.h's mMatrix). The shadow-drop matrix,
       rebuilt by 021166ac and handed to DropShadowRadHeight. */
    s32 mShadowMtx[12];        /* 0x4a4 */
    u8  pad_4d4[0x30];
    /* Body-model placement: 021166ac rebuilds the 0x400 rotation matrix from
       mBodyAngleY and anchors it here. */
    s32 mBodyPosX;             /* 0x504 */
    s32 mBodyPosY;             /* 0x508 */
    s32 mBodyPosZ;             /* 0x50c */
    /* Body-model draw scale (0xc00 each); Render hands it to mBodyModel. */
    s32 mBodyScaleX;           /* 0x510 */
    s32 mBodyScaleY;           /* 0x514 */
    s32 mBodyScaleZ;           /* 0x518 */
    /* Spawn position; Behavior restores here on hide and on respawn, and the
       leash distance is measured from here. */
    s32 mHomePosX;             /* 0x51c */
    s32 mHomePosY;             /* 0x520 */
    s32 mHomePosZ;             /* 0x524 */
    /* Last grounded position; Behavior falls back here when the raycast finds
       no ground within range. */
    s32 mLastGroundPosX;       /* 0x528 */
    s32 mLastGroundPosY;       /* 0x52c */
    s32 mLastGroundPosZ;       /* 0x530 */
    /* Collision center, as an offset from the actor position: the Init offset
       for mdCcAcPos_c, recomputed by 021160d4 from the bone matrix. */
    s32 mClsnOffX;             /* 0x534 */
    s32 mClsnOffY;             /* 0x538 */
    s32 mClsnOffZ;             /* 0x53c */
    /* Bone-matrix offset captured at Init; 021160d4 subtracts it back out of
       the recomputed collision center. */
    s32 mClsnBaseX;            /* 0x540 */
    s32 mClsnBaseY;            /* 0x544 */
    s32 mClsnBaseZ;            /* 0x548 */
    u8  pad_54c[0x18];
    /* Cap anchor: 021166ac repositions the cap from here and the actor
       angles (y rebuilt from mClsnOffY plus scale height). */
    s32 mCapPosX;              /* 0x564 */
    s32 mCapPosY;              /* 0x568 */
    s32 mCapPosZ;              /* 0x56c */
    s16 mHomeAngleX;           /* 0x570 */
    s16 mHomeAngleY;           /* 0x572 */
    s16 mHomeAngleZ;           /* 0x574 */
    u8  pad_576[0xa];
    s32 mDistToPlayer;         /* 0x580 -- Vec3_HorzDist each Behavior */
    /* Live scale factor for the collision volume: 02116a1c multiplies
       mClsnRadius/mClsnHeight by this into mdCcAcPos_c's words. */
    s32 mClsnScale;            /* 0x584 */
    u8  pad_588[0x8];
    s32 mClsnRadius;           /* 0x590 -- int units, shifted << 0xc for Init */
    s32 mClsnHeight;           /* 0x594 */
    /* Z bias added to mClsnOffZ (Init seeds the offset with it; 021160d4 adds
       it after the recompute). -0x14000, -0x24000 for variant 5. */
    s32 mClsnZBias;            /* 0x598 */
    /* Chase leash from mHomePosX: ((param1 >> 8) & 0xff) * 0x64000. */
    s32 mLeashDist;            /* 0x59c */
    /* Children lost: CleanupResources increments the spawner's (see
       mSpawnerID); variant 3 stops spawning at 5. */
    s32 mChildDeaths;          /* 0x5a0 */
    /* Per-variant death mode (data_ov063_0211e22c[unk_5cf]): nonzero dies
       into the death table on invincible kill, 0 rebirths into state 5. */
    s32 mDeathMode;            /* 0x5a4 */
    s32 unk_5a8;               /* 0x5a8 -- written 0xa/0, no reader recovered */
    /* Hurt damage passed to Player::Hurt (data_ov063_0211e1ec[unk_5cf]). */
    u32 mHurtDamage;           /* 0x5ac */
    s16 mAngleToPlayer;        /* 0x5b0 -- Vec3_HorzAngle each Behavior */
    s16 unk_5b2;               /* 0x5b2 -- yaw snapshot, no reader recovered */
    u8  pad_5b4[0x4];
    u16 unk_5b8;               /* 0x5b8 -- read >> 4, no nonzero writer found */
    /* Body-model yaw: spins by 0xc00 (021166ac) unless mCarriedID is 0xd4. */
    s16 mBodyAngleY;           /* 0x5ba */
    /* Desired yaw: 02116fac steers mAngleY toward this. */
    s16 mTargetAngleY;         /* 0x5bc */
    u8  pad_5be[0x2];
    u16 mTimer5c0;             /* 0x5c0 -- DecIfAbove0_Short'ed by Behavior */
    u16 mTimer5c2;             /* 0x5c2 -- variant 0xf timers (021172a8 etc.) */
    u16 mTimer5c4;             /* 0x5c4 */
    /* Secret-sound counter (021189f4, 0211934c); Behavior hides only at 0. */
    u16 mSoundCount;           /* 0x5c6 */
    /* Fade/opacity: 0xff at Init; Render skips below 8, 021166ac feeds >> 3
       to ApplyOpacity. */
    u8  mOpacity;              /* 0x5c8 */
    u8  pad_5c9;
    /* 1-based row into data_ov063_0211e1c0 (02118458): 3 for the boss, 1. */
    u8  mDataIdx;              /* 0x5ca */
    /* Children spawned (variant 3); gated against mChildDeaths. */
    u8  mSpawnCount;           /* 0x5cb */
    u8  unk_5cc;               /* 0x5cc -- state (0/1/2/3/5/6 seen) */
    u8  mPrevState;            /* 0x5cd -- mStateTimer resets on change */
    u8  mSubState;             /* 0x5ce -- talk-flow sub-state (02117b0c) */
    /* Variant from param1's low nibble (+0xc for the 0xd2 boss): 0/1/2/6/9/10
       chase (021192d4), 3 spawner (02116bf4), 4/0xb watcher (02116a1c/02116fac),
       5 star-gated carrier (02116d38), 7 (0211ab68), 8 (02119274), 0xc/0xd/0xe
       boss (021189f4), 0xf key (021172a8). */
    u8  unk_5cf;               /* 0x5cf */
    /* Home area: the IsAreaShowing index and the spawn area. */
    s8  mAreaIdx;              /* 0x5d0 */
    /* Talk-message step (0 idle, 1 shown, 2 dropped). */
    u8  mTalkStep;             /* 0x5d1 */
    u8  pad_5d2;
    u8  unk_5d3;               /* 0x5d3 -- zeroed, no reader recovered */
    /* 0x5d4 -- the flags halfword. Typed (not a u8 placeholder) so member
       access compiles to the ROM's add+ldrh instead of a literal-pool
       address load. */
    BooFlags16 mFlags_5d4;  /* 0x5d4 */
    u8  pad_5d6[0x2];
    /* Recycled effect handles (0x95/0x96/0x97/0x98), like daBmb_c's. */
    u32 mParticle0;            /* 0x5d8 */
    u32 mParticle1;            /* 0x5dc */

    virtual ~daTrs_c();

    /* --- vtable, own overrides --- */
    virtual int  OnYoshiTryEat();      /* slot 18 */
    virtual int  OnAimedAtWithEgg();   /* slot 29 */

    int  Behavior();           /* slot 6, fBase_c */
    int  CleanupResources();
    int  InitResources();
    void OnPendingDestroy();
    int  Render();

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daTrs_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTrs_c_size_must_be_0x5e0[sizeof(daTrs_c) == 0x5e0 ? 1 : -1];
#endif

#endif
