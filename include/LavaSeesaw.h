#ifndef LAVASEESAW_H
#define LAVASEESAW_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_Seesaw_c_classInit  fBase_c::operator new(804 = 0x324), dBgActor_c::dBgActor_c(), stores _ZTV10LavaSeesaw,
 *                 then the members below in this order.
 *   ~LavaSeesaw   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x324 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * mSwingStep sits at 0x31e, in dBgActor_c's TAIL PADDING (same placement rationale
 * as daObjRc_Guruguru_c's mAngVelY): Behavior adds it to mAngleX every frame while
 * mSwingCooldown is zero, and flips its sign (a see-saw tilt reversal) once mAngleX
 * passes +-0x400, also reloading mSwingCooldown to 0x1e. InitResources seeds
 * mSwingStep to -0x10.
 */
struct LavaSeesaw : dBgActor_c {
    s16 mSwingStep;          /* 0x31e */
    u8  mSwingCooldown;      /* 0x320 */
    u8  pad_321[0x3];

    virtual ~LavaSeesaw();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char LavaSeesaw_size_must_be_0x324[sizeof(LavaSeesaw) == 0x324 ? 1 : -1];

#endif /* LAVASEESAW_H */
