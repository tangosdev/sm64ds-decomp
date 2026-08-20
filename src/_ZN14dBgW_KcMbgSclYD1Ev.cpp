//cpp
// @symbol _ZN14dBgW_KcMbgSclYD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the dBgW_KcMbg subobject destructor. Both
 * come out of `struct dBgW_KcMbgSclY : dBgW_KcMbg` and `virtual ~dBgW_KcMbgSclY()` in the
 * header, so there is nothing left to write.
 *
 * Its two members are Fix12i, which have no destructors, so the ROM's 0x24
 * bytes are the vtable store and the base call.
 */
#include "dBgW_KcMbgSclY.h"

dBgW_KcMbgSclY::~dBgW_KcMbgSclY()
{
}
