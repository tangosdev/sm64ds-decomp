#ifndef TINYCOVER_H
#define TINYCOVER_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjTtFuta_c_classInit (historical alias TinyCover_Spawn) calls
 *                 fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV9TinyCover,
 *                 then the members below in this order.
 *   ~TinyCover   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct TinyCover : dBgActor_c {

    virtual ~TinyCover();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnGroundPounded(dActor_c &other); /* slot 21 */
};

typedef char TinyCover_size_must_be_0x320[sizeof(TinyCover) == 0x320 ? 1 : -1];

#endif /* TINYCOVER_H */
