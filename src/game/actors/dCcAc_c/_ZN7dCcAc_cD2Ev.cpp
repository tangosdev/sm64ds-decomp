//cpp
// @symbol _ZN7dCcAc_cD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN7dCcAc_cD1Ev.cpp, and deliberately so: dCcAc_c has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the dCc_c subobject destructor. Both
 * come out of `struct dCcAc_c : dCc_c` and `virtual ~dCcAc_c()` in the
 * header, so there is nothing left to write.
 *
 * The one member is a back-pointer to the owning dActor_c -- not owned, and with
 * no destructor -- so the ROM's 0x24 bytes are the vtable store and the base
 * call.
 */
#include "dCcAc_c.h"

dCcAc_c::~dCcAc_c()
{
}
