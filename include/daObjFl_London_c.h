#ifndef DAOBJFL_LONDON_C_H
#define DAOBJFL_LONDON_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_London_c_classInit  fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV16daObjFl_London_c,
 *                 then the members below in this order.
 *   ~daObjFl_London_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x320 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * mCooldown/mFlag sit at 0x31e/0x31f, in dBgActor_c's TAIL PADDING (same placement
 * rationale as daObjRc_Guruguru_c's mAngVelY). InitResources is this task's only
 * evidence for them (seeds mCooldown to 0xf, mFlag to 0); Behavior (out of this
 * task's scope) is presumably where they are read.
 */
struct daObjFl_London_c : dBgActor_c {
    /* The destructor is declared FIRST and defined INLINE: mwcc then emits the
     * D1/D0 pair in retail order and no homeless D2, which is what lets the
     * whole class isolate into one translation unit. */
    virtual ~daObjFl_London_c() {}         /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    u8  mCooldown;                         /* 0x31e */
    u8  mFlag;                             /* 0x31f */
};

typedef char daObjFl_London_c_size_must_be_0x320[sizeof(daObjFl_London_c) == 0x320 ? 1 : -1];

#endif /* DAOBJFL_LONDON_C_H */
