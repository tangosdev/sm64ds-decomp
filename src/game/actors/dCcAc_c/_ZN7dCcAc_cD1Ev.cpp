//cpp
// @symbol _ZN7dCcAc_cD1Ev
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
