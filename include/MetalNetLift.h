#ifndef METALNETLIFT_H
#define METALNETLIFT_H

#include "types.h"
#include "Platform.h"
#include "PathPtr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   MetalNetLift_Spawn  ActorBase::operator new(872 = 0x368), Platform::Platform(), stores _ZTV12MetalNetLift,
 *                 then the member below in this order.
 *   ~MetalNetLift   the same member destroyed in reverse, then ~Platform.
 *
 * SIZE 0x368 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct MetalNetLift : Platform {
    u8  pad_320[0x40];
    PathPtr                mPathPtr;     /* 0x360 */

    virtual ~MetalNetLift();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char MetalNetLift_size_must_be_0x368[sizeof(MetalNetLift) == 0x368 ? 1 : -1];

#endif /* METALNETLIFT_H */
