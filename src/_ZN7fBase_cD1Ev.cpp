//cpp
// @symbol _ZN7fBase_cD1Ev
/* D1, the complete-object destructor. One `fBase_c::~fBase_c()` definition makes mwcc
 * emit D0, D1 and D2 together; objisolate keeps the one this file is bound to
 * by its delinks entry, so the siblings carry the same definition. */
#include "fBase_c.h"

fBase_c::~fBase_c()
{
}
