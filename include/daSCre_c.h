#ifndef DASCRE_C_H
#define DASCRE_C_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daSCre_c_Spawn  fBase_c::operator new(212 = 0xd4), dActor_c::dActor_c(),
 *                   stores _ZTV8daSCre_c, then the members below in this order.
 *   ~daSCre_c       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0xd4 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daSCre_c : dActor_c {
    u8  pad_0d0[0x4];

    /* Slots 16 (D1) and 17 (D0). DEFINED INLINE ON PURPOSE. Written
     * out-of-line in the TU, mwccarm emits D0 before D1 -- the reverse of the
     * cartridge's order -- which objisolate refuses for the whole translation
     * unit, and it emits a third D2 body with no ROM home. Defined here the
     * compiler produces the retail D1/D0 pair in ROM order and no D2, while
     * Behavior -- the first virtual declared out-of-line -- keeps
     * src/actors/d_a_s_cre.cpp as this class's key-function TU. The body is
     * genuinely empty: the class adds no owned resource, only pad_0d0. */
    virtual ~daSCre_c() {}

    virtual s32   Behavior();               /* slot  6 */
};

typedef char daSCre_c_size_must_be_0xd4[sizeof(daSCre_c) == 0xd4 ? 1 : -1];

#endif /* DASCRE_C_H */
