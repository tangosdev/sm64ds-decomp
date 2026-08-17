#ifndef SWINGINGPLATFORM_H
#define SWINGINGPLATFORM_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SwingingPlatform_Spawn  ActorBase::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV16SwingingPlatform,
 *                 then the members below in this order.
 *   ~SwingingPlatform   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct SwingingPlatform : dBgActor_c {

    virtual ~SwingingPlatform();            /* slots 16 (D1), 17 (D0) */

    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char SwingingPlatform_size_must_be_0x320[sizeof(SwingingPlatform) == 0x320 ? 1 : -1];

#endif /* SWINGINGPLATFORM_H */
