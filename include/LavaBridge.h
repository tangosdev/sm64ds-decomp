#ifndef LAVABRIDGE_H
#define LAVABRIDGE_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_London_c_classInit  fBase_c::operator new(800 = 0x320), dBgActor_c::dBgActor_c(), stores _ZTV10LavaBridge,
 *                 then the members below in this order.
 *   ~LavaBridge   the same members destroyed in reverse, then ~dBgActor_c.
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
struct LavaBridge : dBgActor_c {
    u8  mCooldown;           /* 0x31e */
    u8  mFlag;                /* 0x31f */

    virtual ~LavaBridge();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char LavaBridge_size_must_be_0x320[sizeof(LavaBridge) == 0x320 ? 1 : -1];

#endif /* LAVABRIDGE_H */
