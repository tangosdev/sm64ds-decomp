#ifndef POWERSTARCREATE_H
#define POWERSTARCREATE_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   PowerStarCreate_Spawn  fBase_c::operator new(212 = 0xd4), dActor_c::dActor_c(), stores _ZTV15PowerStarCreate,
 *                 then the members below in this order.
 *   ~PowerStarCreate   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xd4 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct PowerStarCreate : dActor_c {
    u8  pad_0d0[0x4];

    virtual ~PowerStarCreate() {}            /* slots 16 (D1), 17 (D0) */

    virtual s32   Behavior();               /* slot  6 */
};

typedef char PowerStarCreate_size_must_be_0xd4[sizeof(PowerStarCreate) == 0xd4 ? 1 : -1];

#endif /* POWERSTARCREATE_H */
