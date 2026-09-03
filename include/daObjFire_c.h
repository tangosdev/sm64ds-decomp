#ifndef DAOBJFIRE_C_H
#define DAOBJFIRE_C_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   BlueFlame_Spawn  fBase_c::operator new(280 = 0x118), dActor_c::dActor_c(), stores _ZTV11daObjFire_c,
 *                 then the member below in this order.
 *   ~daObjFire_c   the same member destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x118 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjFire_c : dActor_c {
    u8  pad_0d0[0x4];
    Vector3                mFlamePos;            /* 0x0d4 */
    u8                     mDisappearTimer;      /* 0x0e0 */
    u8  pad_0e1[0x3];
    dCcAc_c     mdCcAc_c; /* 0x0e4 */

    /* Inline, and declared FIRST. This TU defines every virtual the class has,
     * so it emits the vtable and RTTI whatever is declared first -- moving the
     * declaration last buys nothing. Out of line, mwccarm emits the D2/D1/D0
     * triple in D0-before-D1 order, but retail puts D1 (0x020b5734) ABOVE D0
     * (0x020b5764), and objisolate then refuses the whole TU for emitting out
     * of ROM address order. The inline body emits only the retail D1/D0 pair,
     * in retail order, and emits no D2. */
    virtual ~daObjFire_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual int   InitResources();         /* slot  0 */
    virtual int   Behavior();              /* slot  6 */
    virtual s32   OnYoshiTryEat();         /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char daObjFire_c_size_must_be_0x118[sizeof(daObjFire_c) == 0x118 ? 1 : -1];

#endif /* DAOBJFIRE_C_H */
