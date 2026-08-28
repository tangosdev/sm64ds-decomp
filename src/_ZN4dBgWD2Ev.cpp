//cpp
// @symbol _ZN4dBgWD2Ev
/* D2, the base-object destructor. dBgW has no virtual bases, so mwcc emits
 * D1 and D2 as byte-identical code -- only how the ROM REACHES an address
 * separates them (a vtable slot holds D1; a derived destructor's base-chain
 * `bl` reaches D2). Comparing the two bodies proves nothing; the binding in
 * the delinks entry is what decides. */
#include "dBgW.h"

dBgW::~dBgW()
{
}
