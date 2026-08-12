#ifndef POLEBILLBOARD_H
#define POLEBILLBOARD_H

#include "types.h"
#include "Actor.h"
#include "Model.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   PoleBillboard_Spawn  ActorBase::operator new(292 = 0x124), Actor::Actor(), stores _ZTV13PoleBillboard,
 *                 then the member below in this order.
 *   ~PoleBillboard   the same member destroyed in reverse, then ~Actor.
 *
 * SIZE 0x124 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV5Actor. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct PoleBillboard : Actor {
    u8  pad_0d0[0x4];
    Model                  mModel;       /* 0x0d4 */

    virtual ~PoleBillboard();            /* slots 16 (D1), 17 (D0) */

    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char PoleBillboard_size_must_be_0x124[sizeof(PoleBillboard) == 0x124 ? 1 : -1];

#endif /* POLEBILLBOARD_H */
