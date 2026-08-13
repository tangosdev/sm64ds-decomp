#ifndef BLUEFLAME_H
#define BLUEFLAME_H

#include "types.h"
#include "Actor.h"
#include "MovingCylinderClsn.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BlueFlame_Spawn  ActorBase::operator new(280 = 0x118), Actor::Actor(), stores _ZTV9BlueFlame,
 *                 then the member below in this order.
 *   ~BlueFlame   the same member destroyed in reverse, then ~Actor.
 *
 * SIZE 0x118 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BlueFlame : Actor {
    u8  pad_0d0[0x14];
    MovingCylinderClsn     mMovingCylinderClsn; /* 0x0e4 */

    virtual ~BlueFlame();            /* slots 16 (D1), 17 (D0) */

    virtual s32   OnYoshiTryEat();         /* slot 18 */
};

typedef char BlueFlame_size_must_be_0x118[sizeof(BlueFlame) == 0x118 ? 1 : -1];

#endif /* BLUEFLAME_H */
