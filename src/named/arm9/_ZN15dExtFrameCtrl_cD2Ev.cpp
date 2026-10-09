//cpp
// @symbol _ZN15dExtFrameCtrl_cD2Ev
/* D2, the base-object destructor. dExtFrameCtrl_c has no virtual bases, so mwcc emits
 * D1 and D2 as byte-identical code -- only how the ROM REACHES an address
 * separates them (a vtable slot holds D1; a derived destructor's base-chain
 * `bl` reaches D2). Comparing the two bodies proves nothing; the binding in
 * the delinks entry is what decides. */
#include "dExtFrameCtrl_c.h"

dExtFrameCtrl_c::~dExtFrameCtrl_c()
{
}
