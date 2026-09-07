#ifndef BLUEFLAME_H
#define BLUEFLAME_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjFire_c -- daObjFire_c_classInit_OBJ_BLUE_FIRE (was BlueFlame_Spawn), g_profile_OBJ_BLUE_FIRE (was BlueFlame_SpawnInfo)
 */

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFire_c_classInit_OBJ_BLUE_FIRE  fBase_c::operator new(280 = 0x118), dActor_c::dActor_c(), stores _ZTV9BlueFlame,
 *                 then the member below in this order.
 *   ~BlueFlame   the same member destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x118 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BlueFlame : dActor_c {
    u8  pad_0d0[0x4];
    Vector3                mFlamePos;            /* 0x0d4 */
    u8                     mDisappearTimer;      /* 0x0e0 */
    u8  pad_0e1[0x3];
    dCcAc_c     mdCcAc_c; /* 0x0e4 */

    virtual ~BlueFlame();            /* slots 16 (D1), 17 (D0) */

    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */

    int Behavior();
    int InitResources();
};

typedef char BlueFlame_size_must_be_0x118[sizeof(BlueFlame) == 0x118 ? 1 : -1];

#endif /* BLUEFLAME_H */
