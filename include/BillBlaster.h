#ifndef BILLBLASTER_H
#define BILLBLASTER_H

#include "types.h"
#include "Platform.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BillBlaster_Spawn  ActorBase::operator new(804 = 0x324), Platform::Platform(), stores _ZTV11BillBlaster,
 *                 then the members below in this order.
 *   ~BillBlaster   the same members destroyed in reverse, then ~Platform.
 *
 * SIZE 0x324 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8Platform. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BillBlaster : Platform {
    u8  pad_320[0x4];

    virtual ~BillBlaster();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char BillBlaster_size_must_be_0x324[sizeof(BillBlaster) == 0x324 ? 1 : -1];

#endif /* BILLBLASTER_H */
