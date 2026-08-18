#ifndef BILLBLASTER_H
#define BILLBLASTER_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BillBlaster_Spawn  fBase_c::operator new(804 = 0x324), dBgActor_c::dBgActor_c(), stores _ZTV11BillBlaster,
 *                 then the members below in this order.
 *   ~BillBlaster   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x324 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct BillBlaster : dBgActor_c {
    u8  pad_320[0x4];

    virtual ~BillBlaster();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
    /* dBgActor_c's own slot, overridden here: _ZTV11BillBlaster+0x7c relocates to
       0x02126e58 while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void  Kill();                  /* slot 31 */
};

typedef char BillBlaster_size_must_be_0x324[sizeof(BillBlaster) == 0x324 ? 1 : -1];

#endif /* BILLBLASTER_H */
