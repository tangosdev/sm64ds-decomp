//cpp
// @symbol _ZN7fBase_cD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `fBase_c::~fBase_c()` definition. */
#include "fBase_c.h"

extern "C" void func_020440e8(void *node);

fBase_c::~fBase_c()
{
    func_020440e8(&renderNode);
    func_020440e8(&behavNode);
}
