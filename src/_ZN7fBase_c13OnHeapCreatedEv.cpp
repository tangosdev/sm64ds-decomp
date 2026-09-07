//cpp
// @symbol _ZN7fBase_c13OnHeapCreatedEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::OnHeapCreated() at 0x02043494 -- vtable slot 15.
 * Hook fired after the actor's dedicated heap is set up.
 * Base returns the VirtualFuncSuccess code VS_FAIL (1); leaf classes override.
 */

struct fBase_c;

bool fBase_c::OnHeapCreated()
{
    return 1; /* VS_FAIL */
}
