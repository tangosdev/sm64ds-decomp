#ifndef POWERSTARCREATE_H
#define POWERSTARCREATE_H

#include "types.h"
#include "Actor.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   PowerStarCreate_Spawn  ActorBase::operator new(212 = 0xd4), Actor::Actor(), stores _ZTV15PowerStarCreate,
 *                 then the members below in this order.
 *   ~PowerStarCreate   the same members destroyed in reverse, then ~Actor.
 *
 * SIZE 0xd4 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct PowerStarCreate : Actor {
    u8  pad_0d0[0x4];

    virtual ~PowerStarCreate();            /* slots 16 (D1), 17 (D0) */
};

typedef char PowerStarCreate_size_must_be_0xd4[sizeof(PowerStarCreate) == 0xd4 ? 1 : -1];

#endif /* POWERSTARCREATE_H */
