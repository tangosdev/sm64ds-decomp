#ifndef ROCKPILLAR_H
#define ROCKPILLAR_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   RockPillar_Spawn  fBase_c::operator new(808 = 0x328), dBgActor_c::dBgActor_c(), stores _ZTV10RockPillar,
 *                 then the members below in this order.
 *   ~RockPillar   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct RockPillar : dBgActor_c {
    u8  pad_320[0x8];

    virtual ~RockPillar();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char RockPillar_size_must_be_0x328[sizeof(RockPillar) == 0x328 ? 1 : -1];

#endif /* ROCKPILLAR_H */
