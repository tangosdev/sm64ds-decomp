#ifndef FIRESEAELEVATOR_H
#define FIRESEAELEVATOR_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   FireSeaElevator_Spawn  ActorBase::operator new(808 = 0x328), Platform::Platform(), stores _ZTV15FireSeaElevator,
 *                 then the members below in this order.
 *   ~FireSeaElevator   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct FireSeaElevator : Platform {
    u8  pad_320[0x8];

    virtual ~FireSeaElevator();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char FireSeaElevator_size_must_be_0x328[sizeof(FireSeaElevator) == 0x328 ? 1 : -1];

#endif /* FIRESEAELEVATOR_H */
