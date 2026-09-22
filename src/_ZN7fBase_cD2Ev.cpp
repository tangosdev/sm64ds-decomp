//cpp
// @symbol _ZN7fBase_cD2Ev
/* D2, the base-object destructor. Same definition as the D1 file, and
 * deliberately so: fBase_c has no virtual bases, so mwcc emits D1 and D2 as
 * byte-identical code. Only how the ROM REACHES an address separates them --
 * a vtable slot holds D1, a derived destructor's base-chain `bl` reaches D2. */
#include "fBase_c.h"

fBase_c::~fBase_c()
{
}
