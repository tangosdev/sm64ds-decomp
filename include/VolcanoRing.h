#ifndef VOLCANORING_H
#define VOLCANORING_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   VolcanoRing_Spawn  ActorBase::operator new(808 = 0x328), dBgActor_c::dBgActor_c(), stores _ZTV11VolcanoRing,
 *                 then the members below in this order.
 *   ~VolcanoRing   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct VolcanoRing : dBgActor_c {
    u8  pad_320[0x8];

    virtual ~VolcanoRing();            /* slots 16 (D1), 17 (D0) */
};

typedef char VolcanoRing_size_must_be_0x328[sizeof(VolcanoRing) == 0x328 ? 1 : -1];

#endif /* VOLCANORING_H */
