#ifndef COIN_H
#define COIN_H
#include "types.h"
#include "dActor_c.h"
#include "CommonModel.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "math/Matrix.h"

/* THREE WITNESSES:
 *
 *   Coin_Spawn / BlueCoin_Spawn / RedCoin_Spawn
 *       fBase_c::operator new(948 = 0x3b4), dActor_c::dActor_c(), stores
 *       _ZTV4Coin, then the five members below in this order.
 *   _ZN4CoinD0Ev  the same five members destroyed in reverse, then ~dActor_c.
 *
 * All three Spawn entry points build the SAME class (same vtable, same
 * size) -- Coin is the coin actor shared by yellow/blue/red spawn paths.
 *
 * SIZE 0x3b4 is the factory's own literal; the last member (unk_3b0, 1 byte)
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
    s32 unk_0d4;            /* 0x0d4 */
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
    Matrix4x3 unk_368;        /* 0x368 */
    s32 unk_398;            /* 0x398 */
    u8  pad_39c[0x4];
    s32 mCoinType;            /* 0x3a0 */
    s32 mBehaviorType;            /* 0x3a4 */
    s16 unk_3a8;            /* 0x3a8 */
    u8  unk_3aa;            /* 0x3aa */
    u8  unk_3ab;            /* 0x3ab */
    s8  unk_3ac;            /* 0x3ac */
    u8  pad_3ad[0x1];
    u8  unk_3ae;            /* 0x3ae */
    u8  pad_3af[0x1];
    u8  unk_3b0;            /* 0x3b0 */

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
