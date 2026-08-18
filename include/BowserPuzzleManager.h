#ifndef BOWSERPUZZLEMANAGER_H
#define BOWSERPUZZLEMANAGER_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BowserPuzzleManager_Spawn  fBase_c::operator new(216 = 0xd8), dActor_c::dActor_c(), stores _ZTV19BowserPuzzleManager,
 *                 then the members below in this order.
 *   ~BowserPuzzleManager   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xd8 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BowserPuzzleManager : dActor_c {
    u8  pad_0d0[0x8];

    virtual ~BowserPuzzleManager();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   Behavior();              /* slot  6 */
};

typedef char BowserPuzzleManager_size_must_be_0xd8[sizeof(BowserPuzzleManager) == 0xd8 ? 1 : -1];

#endif /* BOWSERPUZZLEMANAGER_H */
