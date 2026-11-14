#ifndef DACOIN_C_H
#define DACOIN_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daCoin_c -- daCoin_c_classInit_COIN (was Coin_Spawn), g_profile_COIN (was Coin_SpawnInfo)
 *   daCoin_c -- daCoin_c_classInit_RED_COIN (was RedCoin_Spawn), g_profile_RED_COIN (was RedCoin_SpawnInfo)
 *   daCoin_c -- daCoin_c_classInit_BLUE_COIN (was BlueCoin_Spawn), g_profile_BLUE_COIN (was BlueCoin_SpawnInfo)
 */
#include "types.h"
#include "dActor_c.h"
#include "dExtCommonModel_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "math/Matrix.h"

struct daStarBase_c;

/* THREE WITNESSES:
 *
 *   daCoin_c_classInit_COIN / daCoin_c_classInit_BLUE_COIN / daCoin_c_classInit_RED_COIN
 *       fBase_c::operator new(948 = 0x3b4), dActor_c::dActor_c(), stores
 *       _ZTV8daCoin_c, then the five members below in this order.
 *   _ZN8daCoin_cD0Ev  the same five members destroyed in reverse, then ~dActor_c.
 *
 * All three Spawn entry points build the SAME class (same vtable, same
 * size) -- daCoin_c is the coin actor shared by yellow/blue/red spawn paths.
 *
 * SIZE 0x3b4 is the factory's own literal; the last member (mInBrickBlock, 1 byte)
 * closes at 0x3b1 and rounds up to 0x3b4 under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0 (mAreaId + pad_0cd + unk_0ce), so
 * mEatingPlayer at 0x0d0 is daCoin_c's first own field. Consumers that used the
 * old duplicated names (mParam, mActorID, mAreaId, unk_074, unk_0b0, ...)
 * were repointed to the inherited dActor_c/fBase_c names.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. daCoin_c overrides
 * slot 0 (InitResources), slot 3 (CleanupResources), slot 6 (Behavior) and
 * slot 9 (Render) -- all still fBase_c's own slots in dActor_c -- plus 18
 * (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot holds the base's
 * own word and is inherited, so it is deliberately not redeclared here.
 * Every override, InitResources and CleanupResources included, is defined as
 * a real member in src/game/actors/d_a_coin.cpp. The out-of-line destructor is the
 * key function, so that TU owns the vtable.
 */
struct daCoin_c : dActor_c {
    s32 mEatingPlayer;            /* 0x0d0 */
    s32 mPuzzleManagerID;            /* 0x0d4 */
    /* dExtCommonModel_c member, named by the class's own destructor calling
       dExtCommonModel_c's D1 at +0x0d8 -- a relocation the ROM build
       checks. Was a u8 marker. [daCoin_c::~daCoin_c, src/game/actors/d_a_coin.cpp] */
    dExtCommonModel_c mCommonModel1;            /* 0x0d8 */
    /* dExtCommonModel_c member, named by the class's own destructor calling
       dExtCommonModel_c's D1 at +0x114 -- a relocation the ROM build
       checks. Was a u8 marker. [daCoin_c::~daCoin_c, src/game/actors/d_a_coin.cpp] */
    dExtCommonModel_c mCommonModel2;            /* 0x114 */
    /* dExtShadowModel_c member, named by the class's own destructor calling
       dExtShadowModel_c's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [daCoin_c::~daCoin_c, src/game/actors/d_a_coin.cpp] */
    dExtShadowModel_c mShadowModel;            /* 0x150 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x178 -- a relocation the ROM build
       checks. Was a u8 marker. [daCoin_c::~daCoin_c, src/game/actors/d_a_coin.cpp] */
    dCcAc_c mdCc_c;            /* 0x178 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1ac -- a relocation the ROM build
       checks. Was a u8 marker. [daCoin_c::~daCoin_c, src/game/actors/d_a_coin.cpp] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1ac */
    Matrix4x3 mShadowMat;        /* 0x368 */
    s32 mFloorPosY;            /* 0x398 */
    /* Read only by StateFollowTarget, which copies that actor's position
       (raised by 0xc8000) into the coin every frame. Written by
       dActor_c::GivePlayerCoins, which points it at the player it pays. */
    dActor_c *mFollowTarget;            /* 0x39c */
    s32 mCoinType;            /* 0x3a0 */
    s32 mBehaviorType;            /* 0x3a4 */
    u16 mDisappearTimer;            /* 0x3a8 -- every read in the ROM is ldrh */
    u8  mNoClsnTimer;            /* 0x3aa */
    /* param1 bits 4..6 for a red (0x121) or blue (0x122) coin, 0xff otherwise
       -- the same three-bit value LoadObjects compares against gCurrentStarId to
       decide whether an object belongs to the entrance the level was started
       from. A red coin claims a
       star-marker slot only when it matches (or the level is 0x13); a blue
       coin with a filter under 8 clears bit 0 of mCoinFlags. */
    u8  mSpawnFilter;            /* 0x3ab */
    s8  mTrackStarID;            /* 0x3ac */
    u8  pad_3ad[0x1];
    /* Packed flags at 0x3ae. raw is the byte Behavior shifts; bits is the
       same byte split into the launch, bounce and floor fields. */
    struct CoinFlagBits {
        u8 active : 1;
        u8 launchSoundPending : 1;
        u8 bounces : 3;
        u8 floorState : 3;
    };
    union {
        u8 raw;
        CoinFlagBits bits;
    } mCoinFlags;              /* 0x3ae */
    /* Set to 1 by LinkToBlock once it has looked for a BLOCK_L
       actor near a red coin, so the search runs one time. */
    u8  mBlockScanned;            /* 0x3af */
    u8  mInBrickBlock;            /* 0x3b0 */

    virtual ~daCoin_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */

    /* Per-frame helpers and the nine state handlers the behavior table
     * (data_ov002_0210dc70) dispatches through. The ROM names no symbol for
     * them: Step and StateSlideAwayFromMario follow the N64 coin_step and
     * blue_coin_sliding_away_from_mario, the rest are coined or in-tree. */
    void LinkToBlock();
    int KillIfCollected();
    void ProbeFloor();
    int CheckInYoshiMouth();
    daStarBase_c *FindStarMarker();
    void BounceOffWall();
    void Step();
    void UpdateModelMatrix();
    void CollectBlueCoin(daPly_c *player);
    void CollectRedCoin(daPly_c *player);
    void CollectYellowCoin(daPly_c *player);
    int CheckPlayerHit();
    void StateFollowTarget();
    void StateLaunchNearPlayer();
    void StateSlideAwayFromMario();
    void StateBounce();
    void StateRest();
    void StateIdle();
    void StateDrop();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daCoin_c_size_must_be_0x3b4[sizeof(daCoin_c) == 0x3b4 ? 1 : -1];
#endif

#endif
