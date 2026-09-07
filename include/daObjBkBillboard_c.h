#ifndef DAOBJBKBILLBOARD_C_H
#define DAOBJBKBILLBOARD_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjBkBillboard_c_classInit  fBase_c::operator new(292 = 0x124), dActor_c::dActor_c(), stores _ZTV18daObjBkBillboard_c,
 *                 then the member below in this order.
 *   ~daObjBkBillboard_c   the same member destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x124 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjBkBillboard_c : dActor_c {
    u8  pad_0d0[0x4];
    Model                  mModel;       /* 0x0d4 */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjBkBillboard_c() {}     /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Render();                /* slot  9 */
};

typedef char daObjBkBillboard_c_size_must_be_0x124[sizeof(daObjBkBillboard_c) == 0x124 ? 1 : -1];

#endif /* DAOBJBKBILLBOARD_C_H */
