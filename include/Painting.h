#ifndef PAINTING_H
#define PAINTING_H

#include "types.h"
#include "Actor.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Painting_Spawn  ActorBase::operator new(444 = 0x1bc), Actor::Actor(), stores _ZTV8Painting,
 *                 then the members below in this order.
 *   ~Painting   the same members destroyed in reverse, then ~Actor.
 *
 * SIZE 0x1bc is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Painting : Actor {
    u8  pad_0d0[0xec];

    virtual ~Painting();            /* slots 16 (D1), 17 (D0) */
};

typedef char Painting_size_must_be_0x1bc[sizeof(Painting) == 0x1bc ? 1 : -1];

#endif /* PAINTING_H */
