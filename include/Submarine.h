#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Submarine_Spawn  ActorBase::operator new(800 = 0x320), Platform::Platform(), stores _ZTV9Submarine,
 *                 then the members below in this order.
 *   ~Submarine   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Submarine : Platform {

    virtual ~Submarine();            /* slots 16 (D1), 17 (D0) */
};

typedef char Submarine_size_must_be_0x320[sizeof(Submarine) == 0x320 ? 1 : -1];

#endif /* SUBMARINE_H */
