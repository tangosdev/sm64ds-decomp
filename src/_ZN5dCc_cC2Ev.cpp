//cpp
// @symbol _ZN5dCc_cC2Ev
/* recovered: real C++ base-object constructor
 *
 * The ROM's three steps are the vptr store and then zeroing both intrusive-list
 * links: an unlinked node. That is exactly what `dCc_c()` with no base class
 * and two member initializers means, so the body handles the member initialization.
 */
#include "dCc_c.h"

dCc_c::dCc_c()
{
    *(void**)this = data_0208e6ec;
    prev = 0;
    next = 0;
}
