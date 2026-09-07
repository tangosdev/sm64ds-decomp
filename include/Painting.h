#ifndef PAINTING_H
#define PAINTING_H

#include "types.h"
#include "dActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daPicGate_c_classInit  fBase_c::operator new(444 = 0x1bc), dActor_c::dActor_c(), stores _ZTV8Painting,
 *                 then the members below in this order.
 *   ~Painting   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x1bc is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 *
 * SM64DS RTTI names the implementation daPicGate_c. The reconstructed
 * factory daPicGate_c_classInit (historical alias
 * Painting_Spawn) constructs it for the PICTURE_GATE
 * registry profile.
 */
struct Painting : dActor_c {
    u8  pad_0d0[0xec];

    virtual ~Painting();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnPendingDestroy();      /* slot 12 */
};

typedef char Painting_size_must_be_0x1bc[sizeof(Painting) == 0x1bc ? 1 : -1];

#endif /* PAINTING_H */
