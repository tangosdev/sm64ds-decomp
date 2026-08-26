//cpp
// @symbol _ZN7dWipe_cD0Ev
/* recovered: real C++ destructor -- the compiler emits everything but the body
 *
 * D0 is the DELETING destructor, vtable slot 1. Only the `if` below is
 * written source; the vptr store that precedes it and the FaderColor base
 * sub-object destructor (D2) that follows it are both consequences of
 * `struct dWipe_c : FaderColor` and land in exactly the ROM's order.
 *
 * The body itself is this class's one real obligation: if the per-scanline
 * hardware capture is still armed, cancel it before the object goes away.
 *
 * Defining the destructor out of line makes this the key-function TU, so mwcc
 * emits _ZTV7dWipe_c and the D1 variant beside D0; objisolate keeps the variant
 * this file is bound to and rebinds the vtable reference to the ROM's table at
 * 0x020926f0.
 */
#include "dWipe_c.h"
#include "decl_common.h"

dWipe_c::~dWipe_c()
{
    if (needsCleanup == 1)
        func_0202fb30(this);
}
