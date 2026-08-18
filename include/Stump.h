#ifndef STUMP_H
#define STUMP_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Stump_Spawn  fBase_c::operator new(816 = 0x330), dBgActor_c::dBgActor_c(), stores _ZTV5Stump,
 *                 then the members below in this order.
 *   ~Stump   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x330 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Stump : dBgActor_c {
    u8  pad_320[0x10];

    virtual ~Stump();            /* slots 16 (D1), 17 (D0) */

    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char Stump_size_must_be_0x330[sizeof(Stump) == 0x330 ? 1 : -1];

#endif /* STUMP_H */
