//cpp
// @symbol _ZN5dCc_cD1Ev
/* D1, the complete-object destructor. One `dCc_c::~dCc_c()` definition makes mwcc
 * emit D0, D1 and D2 together; objisolate keeps the one this file is bound to
 * by its delinks entry, so the siblings carry the same definition. */
#include "dCc_c.h"

extern "C" void func_02014fa4(void *self);

dCc_c::~dCc_c()
{
    func_02014fa4(this);
}
