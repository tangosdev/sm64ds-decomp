//cpp
// @symbol _ZN10BowserFireD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out what a destructor does anyway: store
 * the vtable, destroy the members in reverse declaration order, chain to the
 * base. Declaring the layout in BowserFire.h instead lets the compiler emit
 * all of it, and the ROM's order is the evidence that the declaration order
 * is right: it destroys 0x304, 0x2d0, 0x110 -- exactly reverse declaration
 * order -- then calls Enemy's base-object destructor.
 *
 * No stand-in structs. The base is the real Enemy and the three members are
 * their real classes, each of whose size is pinned by its own static
 * assertion. That is what makes this file this short.
 */
#include "BowserFire.h"

BowserFire::~BowserFire()
{
}
