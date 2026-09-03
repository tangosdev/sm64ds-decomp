#ifndef DAOBJLAVA_C_H
#define DAOBJLAVA_C_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjLava_c_Spawn  fBase_c::operator new(216 = 0xd8), dActor_c::dActor_c(), stores _ZTV11daObjLava_c,
 *                 then the members below in this order.
 *   ~daObjLava_c   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xd8 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjLava_c : dActor_c {
    u8  pad_0d0[0x4];

    /* 0x0d4. Behavior() feeds this word back into the particle system as the
     * handle argument and stores the result, which is what a live emitter
     * handle looks like -- a fresh call would pass a constant instead. */
    u32 mEffectHandle;

    virtual ~daObjLava_c();                     /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();     /* slot  0 */

    /* Slot 6. The retail vtable's slot 6 holds 0x020b6d84, this class's own
     * function, not dActor_c's word -- so it IS an override and belongs here.
     * It was carried as the unnamed func_ov002_020b6d84 until now. */
    virtual s32   Behavior();          /* slot  6 */
};

typedef char daObjLava_c_size_must_be_0xd8[sizeof(daObjLava_c) == 0xd8 ? 1 : -1];

#endif /* DAOBJLAVA_C_H */
