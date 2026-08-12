#ifndef WARP_H
#define WARP_H

#include "types.h"
#include "Actor.h"
#include "MovingCylinderClsn.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Warp_Spawn  ActorBase::operator new(264 = 0x108), Actor::Actor(), stores _ZTV4Warp,
 *                 then the member below in this order.
 *   ~Warp   the same member destroyed in reverse, then ~Actor.
 *
 * SIZE 0x108 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Warp : Actor {
    u8  pad_0d0[0x4];
    MovingCylinderClsn     mMovingCylinderClsn; /* 0x0d4 */

    virtual ~Warp();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnPendingDestroy();      /* slot 12 */
};

typedef char Warp_size_must_be_0x108[sizeof(Warp) == 0x108 ? 1 : -1];

#endif /* WARP_H */
