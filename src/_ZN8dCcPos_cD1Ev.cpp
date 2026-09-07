//cpp
// @symbol _ZN8dCcPos_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the dCc_c subobject destructor. Both
 * come out of `struct dCcPos_c : dCc_c` and `virtual ~dCcPos_c()` in the
 * header, so there is nothing left to write.
 *
 * The one member is a Vector3, which has no destructor, so the ROM's 0x24
 * bytes are the vtable store and the base call.
 */
#include "dCcPos_c.h"

dCcPos_c::~dCcPos_c()
{
}
