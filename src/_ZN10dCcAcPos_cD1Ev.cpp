//cpp
// @symbol _ZN10dCcAcPos_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the dCcAc_c subobject destructor. Both
 * come out of `struct dCcAcPos_c : dCcAc_c` and `virtual ~dCcAcPos_c()` in the
 * header, so there is nothing left to write.
 *
 * This class adds no members of its own; it combines the owner pointer it
 * inherits with a position. The whole destructor is the vtable store and the
 * base call.
 */
#include "dCcAcPos_c.h"

dCcAcPos_c::~dCcAcPos_c()
{
}
