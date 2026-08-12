#ifndef POPPINGLAVABUBBLES_H
#define POPPINGLAVABUBBLES_H

#include "types.h"
#include "Actor.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   PoppingLavaBubbles_Spawn  ActorBase::operator new(216 = 0xd8), Actor::Actor(), stores _ZTV18PoppingLavaBubbles,
 *                 then the members below in this order.
 *   ~PoppingLavaBubbles   the same members destroyed in reverse, then ~Actor.
 *
 * SIZE 0xd8 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct PoppingLavaBubbles : Actor {
    u8  pad_0d0[0x8];

    virtual ~PoppingLavaBubbles();            /* slots 16 (D1), 17 (D0) */
};

typedef char PoppingLavaBubbles_size_must_be_0xd8[sizeof(PoppingLavaBubbles) == 0xd8 ? 1 : -1];

#endif /* POPPINGLAVABUBBLES_H */
