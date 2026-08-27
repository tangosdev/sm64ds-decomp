//cpp
// @symbol _ZN5dCc_cD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `dCc_c::~dCc_c()` definition. */
#include "dCc_c.h"

extern "C" void func_02014fa4(void *self);

dCc_c::~dCc_c()
{
    func_02014fa4(this);
}
