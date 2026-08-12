#ifndef SKILIFT_H
#define SKILIFT_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   SkiLift_Spawn  ActorBase::operator new(852 = 0x354), Platform::Platform(), stores _ZTV7SkiLift,
 *                 then the members below in this order.
 *   ~SkiLift   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x354 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct SkiLift : Platform {
    u8  pad_320[0x34];

    virtual ~SkiLift();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char SkiLift_size_must_be_0x354[sizeof(SkiLift) == 0x354 ? 1 : -1];

#endif /* SKILIFT_H */
