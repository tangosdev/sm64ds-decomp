/* Boo (TERESA, actor 0xd1) and Big Boo (BOSS_TERESA, actor 0xd2), ov063/daTrs_c.
 * One class serves both actor ids; mVariant picks the behaviour.
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
 *     0x434 dExtShadowModel_c (1st)           0x28  -> 0x45c
 *     0x45c dExtShadowModel_c (2nd)           0x28  -> 0x484
 *
 * Slots 18 (OnYoshiTryEat) and 29 (OnAimedAtWithEgg) are this class's own
 * overrides -- see include/dActor_c.h for the slot table.
 *
 * No daTrs_c() constructor is declared: the factories build the object
 * field-by-field through `return new daTrs_c()` (base C2, vtable store, then
 * each member's own constructor), so declaring one risks an implicit body the
 * compiler would inline somewhere the ROM does not.
 *
 * ~daTrs_c() is declared here and defined once, out of line and first, in
 * src/game/actors/d_a_trs.cpp, the TU it shares with daTBasket_c and daTrsIcon_c.
 * Under that file's `#pragma defer_codegen off` it emits D1 (0x02115ee0) and
 * D0 (0x02115f48) in ROM order; the extra D2 is licensed as homeless.
 *
 * This header has one consumer, src/game/actors/d_a_trs.cpp. Its member names are
 * descriptive readings of how that file uses each field, not ROM names.
 */
#ifndef DATRS_C_H
#define DATRS_C_H
#include "types.h"

#include "dCapEnemy_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The flags halfword at 0x5d4. Named (not anonymous inline) so
   check_header_offsets can parse the member declaration. Bit N is the N-th
   field; d_a_trs.cpp's TrsFlag enum spells the same bits as masks for the
   sites that matched as a halfword read-modify-write. From the uses:
   limitZ (0x1): InitResources sets it when LEVEL_ID is 0xc, mPosX is 0xbb8000
       and mAreaId is 2. Behavior then keeps mPosZ >= -0x12c000, and with it
       set 02116190's leave test is the player's z >= 0x3e8000 instead of the
       y / horizontal-distance test.
   carrying (0x2): the carried item (mCarriedModel) is placed and drawn.
       Cleared once the carried actor is spawned (021162c8, 02116e14) and on
       the replacement actor RespawnIfHasCap returns.
   followHeading (0x4): while set, Behavior copies mPrevAngleY into mAngleY.
       0211a810 clears it when a knockback starts and 0211a6f0 sets it again
       when it ends; 0211a3d0 clears it when the death spin starts.
   visible (0x8): Render and the placement helpers do nothing while clear.
   canAppear (0x10): variants 4 and 0xb leave state 0 only while set.
       For those variants InitResources sets it when the closest player's
       mPosZ is above -0x514000 (signed); Behavior's area-reset block sets it
       too.
   groundSeen (0x20): Behavior's ground ray found floor less than 0x12c000
       below. While set, a TERESA below variant 8 falls back to mLastGroundPos
       when the ray misses or the floor is farther down.
   hitByParam3Player (0x40): func_ov063_0211a0dc sets it when the attacker
       is a PLAYER (actor 0xbf) whose param1 is 3. The chase handlers clear it
       every frame; 021162c8 and 02116e14 pick state 4 instead of 3 while it
       is set, and 02118cd8 takes a hit point only while it is clear.
   defeated (0x80): 02118cd8 sets it when a Big Boo's mHitPoints reaches 0;
       Behavior's area-reset block runs only while it is clear.
   mirrored (0x100): 02118914 sets it (x = -home x, yaw turned by 0x8000)
       and each relocation (02117364) toggles it. While set: 0211640c places
       the body (mModelAnim) and mShadowModel1 at -x with the yaw negated,
       and also drops mShadowModel2 at +x;
       02117364's particles, 021177b0's particles and 0211a3d0's PoofDustAt
       (variant 0xf) use -x; 021177b0's camera midpoint is (player x - x) / 2
       instead of (x + player x) / 2; 02118458 spawns the fireball at the
       unnegated position and sets its byte +0x36e to 1 (0 when clear);
       0211aa34 picks polygon ID 2 instead of 0x16 below full opacity; and
       02119e38 stops counting mAttackTimer down.
   soundStarted (0x200): 02119074 sets it the first time variant 0xd starts
       its music-volume dip: Sound::PlaySub(0x20, 0x14, ...), which 021189f4
       re-issues until mSoundCount reaches 0x4b and then restores
       mSavedMusicVolume.
   camHeld (0x400): 021177b0 sets it and calls dCamera_c::SetFlag_3 for the
       defeat shot; 02117b0c clears it when the message closes. */
struct BooFlags16 {
    u16 limitZ:1, carrying:1, followHeading:1, visible:1, canAppear:1,
        groundSeen:1, hitByParam3Player:1, defeated:1, mirrored:1,
        soundStarted:1, camHeld:1;
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char BooFlags16_size_must_be_0x2[sizeof(BooFlags16) == 0x2 ? 1 : -1];
#endif

struct daTrs_c : dCapEnemy_c {
    /* A count, kept on a Big Boo: each Boo that reports to it (mBigBooID)
       adds one when its death spin ends (0211a3d0, variants other than 0
       and 2) or, for variant 2, when its talk starts (0211934c). Zeroed by InitResources; 02119074 sets it
       to 0xa for variants 0xc/0xf and waits for >= 5 before appearing, and
       02116244 spawns the ICON_TERESA star marker once it is >= 5. */
    s32 mBooDefeatCount;           /* 0x180 */
    dCcAcPos_c mdCcAcPos_c;  /* 0x184 */
    dBgCh_Actr               mWithMeshClsn;               /* 0x1c4 */
    ModelAnim                  mModelAnim;                  /* 0x380 */
    /* Not "mModel": dCapEnemy_c's own Model at 0x114 is inherited, and calling
       this one mModel shadows it. This is the item the Boo carries, not its
       body (that is mModelAnim): InitResources loads the Boo cage (T_BASKET) or a
       coin model chosen by mCarriedID. Render draws it at mCarriedScaleX
       while `carrying` is set. */
    Model                      mCarriedModel;               /* 0x3e4 */
    dExtShadowModel_c                mShadowModel1;               /* 0x434 */
    dExtShadowModel_c                mShadowModel2;               /* 0x45c */
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
    /* uniqueID of the MERRYGOROUND (actor 0x9f), resolved on demand by
       02119c18; 021163d0 reads byte +0x153 of that actor. */
    u32 mCachedActorID;        /* 0x498 */
    /* uniqueID of the ICON_TERESA star marker (actor 0xd3, daTrsIcon_c) a Big
       Boo spawns (InitResources, 02116244). Behavior moves it to the Big Boo
       every frame, 02118eac has it spawn its star, and CleanupResources
       destroys it. */
    u32 mSpawnedActorID;       /* 0x49c */
    /* Actor id of the carried item, spawned when it is released: T_BASKET
       (0xd4) for variant 5, COIN/RED_COIN/BLUE_COIN (0x120/0x121/0x122) for
       variants 0/1/2/6/7/8..0xb; it also picks mCarriedModel's file. END_OF
       (0x187) is the "nothing" default. */
    u16 mCarriedID;            /* 0x4a0 */
    u8  pad_4a2[0x2];
    /* mShadowModel1's drop matrix, rebuilt by 021166ac / 0211640c and handed
       to DropShadowRadHeight. Spelled as twelve words (the sites store
       through a Matrix4x3 cast); mShadowMtx2 below is the typed twin. */
    s32 mShadowMtx[12];        /* 0x4a4 */
    /* mShadowModel2's drop matrix, same 021166ac/DropShadowRadHeight pair. */
    Matrix4x3 mShadowMtx2;     /* 0x4d4 */
    /* Where the carried item sits: 02119ab0 copies the Boo's position here
       (+0xa000 in y, +0x3c000 for the cage) while `carrying` is set, and
       021166ac anchors mCarriedModel's matrix at it >> 3. The carried actor
       is spawned here when it is released. */
    s32 mCarriedPosX;          /* 0x504 */
    s32 mCarriedPosY;          /* 0x508 */
    s32 mCarriedPosZ;          /* 0x50c */
    /* Carried-item draw scale (0xc00 = 0.75 each); Render hands it to
       mCarriedModel. */
    s32 mCarriedScaleX;        /* 0x510 */
    s32 mCarriedScaleY;        /* 0x514 */
    s32 mCarriedScaleZ;        /* 0x518 */
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
    /* Camera goals for variant 0xf's intro scene (02117cdc), built from the
       talking player's position in case 2 and moved in case 5:
       func_020092c4 is handed dCamera_c::pos with mTalkCamPos* and
       dCamera_c::lookAt with mTalkCamLook* in cases 3 and 7. */
    s32 mTalkCamPosX;          /* 0x54c */
    s32 mTalkCamPosY;          /* 0x550 */
    s32 mTalkCamPosZ;          /* 0x554 */
    s32 mTalkCamLookX;         /* 0x558 */
    s32 mTalkCamLookY;         /* 0x55c */
    s32 mTalkCamLookZ;         /* 0x560 */
    /* Cap anchor: 021166ac repositions the cap from here and the actor
       angles (y rebuilt from mClsnOffY plus scale height). */
    s32 mCapPosX;              /* 0x564 */
    s32 mCapPosY;              /* 0x568 */
    s32 mCapPosZ;              /* 0x56c */
    s16 mHomeAngleX;           /* 0x570 */
    s16 mHomeAngleY;           /* 0x572 */
    s16 mHomeAngleZ;           /* 0x574 */
    u8  pad_576[0x6];
    /* MUSIC_VOLUME_LSL_12 (data_0209b490[0]) as read when variant 0xd
       appears (02119074); 021189f4 hands it >> 0xc to Sound::PlaySub. */
    s32 mSavedMusicVolume;     /* 0x57c */
    s32 mDistToPlayer;         /* 0x580 -- Vec3_HorzDist each Behavior */
    /* Live scale factor for the collision volume: 02116a1c multiplies
       mClsnRadius/mClsnHeight by this into mdCcAcPos_c's words. */
    s32 mClsnScale;            /* 0x584 */
    /* Per-chase randoms, redrawn by 02119894/02119960 whenever mStateTimer
       is 0. mSpeedJitter = (random 0..0xfff) * 5, taken off the 0xa000 speed
       base handed to 0211a030; mTurnJitter = (((r << 7) + 0x800) >> 12) for a
       random r in 0..0xfff, i.e. 0..0x80, added to the 0x180 turn step. */
    s32 mSpeedJitter;          /* 0x588 */
    s32 mTurnJitter;           /* 0x58c */
    s32 mClsnRadius;           /* 0x590 -- int units, shifted << 0xc for Init */
    s32 mClsnHeight;           /* 0x594 */
    /* Z bias added to mClsnOffZ (Init seeds the offset with it; 021160d4 adds
       it after the recompute). -0x14000, -0x24000 for variant 5. */
    s32 mClsnZBias;            /* 0x598 */
    /* Chase leash from mHomePosX: ((param1 >> 8) & 0xff) * 0x64000. */
    s32 mLeashDist;            /* 0x59c */
    /* Children lost: CleanupResources increments the spawner's (see
       mSpawnerID); variant 3 stops spawning at 5. 0211a3d0 also writes it on
       the dying Boo itself: 1 when the spin starts, 2 when it ends. */
    s32 mChildDeaths;          /* 0x5a0 */
    /* Per-variant death mode (data_ov063_0211e22c[mVariant]): nonzero dies
       into the death table on an invincible kill, 0 goes to state 5. It also
       picks the end of the death spin (0211975c) and what a defeated Big Boo
       does with its star marker (02118b98); 2 is special in 02119bb0. */
    s32 mDeathMode;            /* 0x5a4 */
    s32 unk_5a8;               /* 0x5a8 -- written 0xa/0, no reader recovered */
    /* Hurt damage passed to Player::Hurt (data_ov063_0211e1ec[mVariant]). */
    u32 mHurtDamage;           /* 0x5ac */
    s16 mAngleToPlayer;        /* 0x5b0 -- Vec3_HorzAngle each Behavior */
    s16 unk_5b2;               /* 0x5b2 -- yaw snapshot, no reader recovered */
    /* Knockback headings: 0211a810 saves mPrevAngleY in the first and picks
       the push direction in the second: the player's yaw, or else by the
       sign of the cosine of (mPrevAngleY - mAngleToPlayer) (SINE_TABLE
       data_02082214[2d+1]) -- player behind: mPrevAngleY, otherwise
       mPrevAngleY + 0x8000. 0211a76c holds
       mPrevAngleY at the second while it runs and 0211a6f0 restores the
       first. */
    s16 mPreKnockbackAngleY;   /* 0x5b4 */
    s16 mKnockbackAngleY;      /* 0x5b6 */
    u16 unk_5b8;               /* 0x5b8 -- read >> 4, no nonzero writer found */
    /* Carried-item yaw: 021166ac adds 0xc00 a frame unless mCarriedID is
       T_BASKET, so a coin spins and the cage does not. */
    s16 mCarriedAngleY;        /* 0x5ba */
    /* Desired yaw: 02116fac and 02119cc0 set it and steer mPrevAngleY toward it. */
    s16 mTargetAngleY;         /* 0x5bc */
    /* Variant 0xf: frames until the next fireball attack. 0211873c starts
       state 4 at 0; reloaded with 0xb4 + a 6-bit random (02117cdc, 02118458)
       or 0x3c + a 6-bit random (02117364); 02119e38 counts it down when
       0211a8a4 returns nonzero and `mirrored` is clear. */
    u16 mAttackTimer;          /* 0x5be */
    /* While nonzero the chase handlers skip steering or pass a zero turn
       step and speed. 0211adfc and 0211a8a4 set it to 30; Behavior counts
       it down (DecIfAbove0_Short). */
    u16 mChaseDelay;           /* 0x5c0 */
    /* Variant 0xf, state 6: set to 0x78 when the relocation starts (021177b0,
       0211873c), counted down by 02117364, and set to 0x78 again once the
       Boo has moved; 021172a8 skips the fade step while it is above 0x5a. */
    u16 mRelocateTimer;        /* 0x5c2 */
    /* Variant 0xf: frames until the next relocation. 0211873c starts state 6
       at 0; reloaded with 0x12c + (random % 150) (02117364, 02117cdc);
       02119e38 counts it down when 0211a8a4 returns nonzero. */
    u16 mNextRelocateTimer;    /* 0x5c4 */
    /* Secret-sound counter (021189f4, 0211934c); Behavior hides only at 0. */
    u16 mSoundCount;           /* 0x5c6 */
    /* Opacity, 0..0xff; 0xff at Init. Render skips below 8, 021166ac feeds
       >> 3 to ApplyOpacity, and 0211aa34 moves it toward mTargetOpacity by
       0x14 a frame. */
    u8  mOpacity;              /* 0x5c8 */
    u8  mTargetOpacity;        /* 0x5c9 */
    /* Hit points: 3 for a Big Boo, 1 for a Boo (InitResources). 02118cd8 and
       021177b0 take one per hit; 02118458 indexes data_ov063_0211e1c0 with
       mHitPoints - 1, and 0211873c / 02118f74 pick their turn step and speed
       from it. */
    u8  mHitPoints;            /* 0x5ca */
    /* Children spawned (variant 3); gated against mChildDeaths. */
    u8  mSpawnCount;           /* 0x5cb */
    /* State within the variant's handler (each handler has its own table). */
    u8  mState;                /* 0x5cc */
    u8  mPrevState;            /* 0x5cd -- mStateTimer resets on change */
    /* Step within a talk or cutscene state (02117b0c, 02117cdc, 0211934c). */
    u8  mSubState;             /* 0x5ce */
    /* Variant from param1's low nibble (+0xc for the 0xd2 boss): 0/1/2/6/9/0xa
       chase (021192d4), 3 spawner (02116bf4), 4/0xb scripted-path Boo that
       appears, waits for the player, flies to fixed coordinates, then fades
       (4) or vanishes (0xb) (02116a1c/02116fac),
       5 star-gated cage carrier (02116d38), 7 trio spawner (0211ab68),
       8 (02119274), 0xc/0xd/0xe Big Boo (021189f4), 0xf the Big Boo that
       leaves the OBJ_KEY (021172a8). */
    u8  mVariant;              /* 0x5cf */
    /* Home area: the IsAreaShowing index and the spawn area. */
    s8  mAreaIdx;              /* 0x5d0 */
    /* Talk-message step (0 idle, 1 shown, 2 dropped). */
    u8  mTalkStep;             /* 0x5d1 */
    /* Variant 0xf: extra fireball volleys left; 0211873c draws 0..2 when the
       attack starts and 02118458 counts it down. */
    u8  mAttackRepeats;        /* 0x5d2 */
    u8  unk_5d3;               /* 0x5d3 -- zeroed, no reader recovered */
    /* Typed (not a u8 placeholder) so member access compiles to the ROM's
       add+ldrh instead of a literal-pool address load. */
    BooFlags16 mBooFlags;      /* 0x5d4 */
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

    /* Existing leaf forwarding adapter: `return new daTrs_c()` routes
       through the retail allocator. fBase_c also declares an adapter. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* The func_ov063_* methods. r0 is this Boo. The address is the name:
       the cartridge does not spell these. func_ov063_021169c4 lives on
       daTBasket_c; func_ov063_02118f24 and func_ov063_0211a0a8 stay free
       (their first parameter is not a daTrs_c). */
    void          func_ov063_021160d4();
    int           func_ov063_02116190();
    void          func_ov063_02116244();
    void          func_ov063_021162c8();
    unsigned char func_ov063_021163d0();
    void          func_ov063_0211640c();
    void          func_ov063_021166ac();
    void          func_ov063_02116a1c();
    void          func_ov063_02116bf0();
    void          func_ov063_02116bf4();
    void          func_ov063_02116d38();
    void          func_ov063_02116d98();
    void          func_ov063_02116dbc();
    void          func_ov063_02116df0();
    void          func_ov063_02116e14();
    int           func_ov063_02116f48();
    void          func_ov063_02116fac();
    void          func_ov063_021172a8();
    void          func_ov063_02117364();
    void          func_ov063_02117650();
    void          func_ov063_0211776c();
    void          func_ov063_021177b0();
    void          func_ov063_02117b0c();
    void          func_ov063_02117cdc();
    void          func_ov063_02118458();
    void          func_ov063_0211873c();
    void          func_ov063_02118914();
    void          func_ov063_021189f4();
    int           func_ov063_02118b2c();
    void          func_ov063_02118b98();
    void          func_ov063_02118cd8();
    void          func_ov063_02118ddc();
    void          func_ov063_02118e5c();
    void          func_ov063_02118ea0();
    void          func_ov063_02118eac();
    void          func_ov063_02118f50();
    void          func_ov063_02118f74();
    void          func_ov063_02119074();
    void          func_ov063_02119274();
    void          func_ov063_021192d4();
    void          func_ov063_0211934c();
    void          func_ov063_0211975c();
    void          func_ov063_02119870();
    void          func_ov063_02119894();
    void          func_ov063_02119960();
    void          func_ov063_02119a2c();
    void          func_ov063_02119a50();
    void          func_ov063_02119ab0();
    void          func_ov063_02119b1c();
    void          func_ov063_02119b84();
    void          func_ov063_02119bb0();
    void          func_ov063_02119c18(unsigned int id);
    void          func_ov063_02119c58();
    void          func_ov063_02119cc0(int unused, s16 a2, int a3);
    void          func_ov063_02119e38(int a1, short a2, int a3);
    void          func_ov063_0211a030(int a, int b);
    int           func_ov063_0211a0dc();
    int           func_ov063_0211a3d0();
    int           func_ov063_0211a564(int arg1);
    int           func_ov063_0211a634(int arg);
    void          func_ov063_0211a6f0();
    void          func_ov063_0211a718();
    void          func_ov063_0211a76c(int cond, int val);
    void          func_ov063_0211a810(int cond);
    int           func_ov063_0211a8a4();
    void          func_ov063_0211a960();
    void          func_ov063_0211a964(int arg1);
    void          func_ov063_0211aa34();
    void          func_ov063_0211ab68();
    int           func_ov063_0211ad00();
    int           func_ov063_0211adb4();
    void          func_ov063_0211adfc();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTrs_c_size_must_be_0x5e0[sizeof(daTrs_c) == 0x5e0 ? 1 : -1];
#endif

#endif
