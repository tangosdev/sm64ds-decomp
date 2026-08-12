#ifndef HUGECOVER_H
#define HUGECOVER_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   HugeCover_Spawn  ActorBase::operator new(800 = 0x320), Platform::Platform(), stores _ZTV9HugeCover,
 *                 then the members below in this order.
 *   ~HugeCover   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct HugeCover : Platform {

    virtual ~HugeCover();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char HugeCover_size_must_be_0x320[sizeof(HugeCover) == 0x320 ? 1 : -1];

#endif /* HUGECOVER_H */
