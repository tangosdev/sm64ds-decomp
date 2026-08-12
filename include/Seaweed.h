#ifndef SEAWEED_H
#define SEAWEED_H

#include "types.h"
#include "Actor.h"
#include "ModelAnim.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Seaweed_Spawn  ActorBase::operator new(312 = 0x138), Actor::Actor(), stores _ZTV7Seaweed,
 *                 then the member below in this order.
 *   ~Seaweed   the same member destroyed in reverse, then ~Actor.
 *
 * SIZE 0x138 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Seaweed : Actor {
    u8  pad_0d0[0x4];
    ModelAnim              mModelAnim;   /* 0x0d4 */

    virtual ~Seaweed();            /* slots 16 (D1), 17 (D0) */
};

typedef char Seaweed_size_must_be_0x138[sizeof(Seaweed) == 0x138 ? 1 : -1];

#endif /* SEAWEED_H */
