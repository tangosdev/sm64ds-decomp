#ifndef BOWSERPUZZLEMANAGER_H
#define BOWSERPUZZLEMANAGER_H

#include "types.h"
#include "Actor.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BowserPuzzleManager_Spawn  ActorBase::operator new(216 = 0xd8), Actor::Actor(), stores _ZTV19BowserPuzzleManager,
 *                 then the members below in this order.
 *   ~BowserPuzzleManager   the same members destroyed in reverse, then ~Actor.
 *
 * SIZE 0xd8 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BowserPuzzleManager : Actor {
    u8  pad_0d0[0x8];

    virtual ~BowserPuzzleManager();            /* slots 16 (D1), 17 (D0) */
};

typedef char BowserPuzzleManager_size_must_be_0xd8[sizeof(BowserPuzzleManager) == 0xd8 ? 1 : -1];

#endif /* BOWSERPUZZLEMANAGER_H */
