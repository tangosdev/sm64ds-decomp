#ifndef COIN_H
#define COIN_H

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
#include "CommonModel.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "math/Matrix.h"

/* THREE WITNESSES:
 *
 *   daCoin_c_classInit_COIN / daCoin_c_classInit_BLUE_COIN / daCoin_c_classInit_RED_COIN
 *       fBase_c::operator new(948 = 0x3b4), dActor_c::dActor_c(), stores
 *       _ZTV4Coin, then the five members below in this order.
 *   _ZN4CoinD0Ev  the same five members destroyed in reverse, then ~dActor_c.
 *
 * All three Spawn entry points build the SAME class (same vtable, same
 * size) -- Coin is the coin actor shared by yellow/blue/red spawn paths.
 *
 * SIZE 0x3b4 is the factory's own literal; the last member (mInBrickBlock, 1 byte)
 * closes at 0x3b1 and rounds up to 0x3b4 under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0 (mAreaId + pad_0cd + unk_0ce), so
 * mEatingPlayer at 0x0d0 is Coin's first own field. Consumers that used the
 * old duplicated names (mParam, mActorID, mAreaId, unk_074, unk_0b0, ...)
 * were repointed to the inherited dActor_c/fBase_c names.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Coin overrides
 * slot 0 (InitResources), slot 3 (CleanupResources), slot 6 (Behavior) and
 * slot 9 (Render) -- all still fBase_c's own slots in dActor_c -- plus 18
 * (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot holds the base's
 * own word and is inherited, so it is deliberately not redeclared here.
 * InitResources and CleanupResources are defined as extern "C" free
 * functions under their mangled names (src/_ZN4Coin13InitResourcesEv.c,
 * src/_ZN4Coin16CleanupResourcesEv.c), the same idiom fBase_c.h itself uses
 * for slot 0 -- declaring them here only fills the vtable slot, it does not
 * make this TU or theirs the key function.
 */
struct Coin : dActor_c {
    s32 mEatingPlayer;            /* 0x0d0 */
    s32 mPuzzleManagerID;            /* 0x0d4 */
    /* CommonModel member, named by the class's own destructor calling
       CommonModel's D1 at +0x0d8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4CoinD0Ev.c] */
    CommonModel mCommonModel1;            /* 0x0d8 */
    /* CommonModel member, named by the class's own destructor calling
       CommonModel's D1 at +0x114 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4CoinD0Ev.c] */
    CommonModel mCommonModel2;            /* 0x114 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4CoinD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x150 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x178 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4CoinD0Ev.c] */
    dCcAc_c mdCc_c;            /* 0x178 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1ac -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN4CoinD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1ac */
    Matrix4x3 mShadowMat;        /* 0x368 */
    s32 mFloorPosY;            /* 0x398 */
    u8  pad_39c[0x4];
    s32 mCoinType;            /* 0x3a0 */
    s32 mBehaviorType;            /* 0x3a4 */
    s16 mDisappearTimer;            /* 0x3a8 */
    u8  mNoClsnTimer;            /* 0x3aa */
    /* param1 bits 4..6 for a red (0x121) or blue (0x122) coin, 0xff otherwise
       -- the same three-bit value LoadObjects compares against data_0209f220 to
       decide whether an object belongs to the entrance the level was started
       from (src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.cpp). A red coin
       claims a star-marker slot only when it matches (or the level is 0x13); a
       blue coin with a filter under 8 clears bit 0 of mCoinFlags.
       [_ZN4Coin13InitResourcesEv.cpp] */
    u8  mSpawnFilter;            /* 0x3ab */
    s8  mTrackStarID;            /* 0x3ac */
    u8  pad_3ad[0x1];
    /* The flag byte the header comment above already describes. Bit 0 gates
       Render entirely; Behavior tests bits 0 and 1. Every read-modify-write of
       it keeps its raw `*(u8*)((int)c + 0x3ae)` spelling on purpose -- see the
       "FOUR SITES KEEP RAW OFFSETS" note in
       src/_ZN4Coin13InitResourcesEv.cpp. [_ZN4Coin6RenderEv.cpp,
       _ZN4Coin8BehaviorEv.cpp] */
    u8  mCoinFlags;            /* 0x3ae */
    u8  pad_3af[0x1];
    u8  mInBrickBlock;            /* 0x3b0 */

    virtual ~Coin();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char Coin_size_must_be_0x3b4[sizeof(Coin) == 0x3b4 ? 1 : -1];

#endif
