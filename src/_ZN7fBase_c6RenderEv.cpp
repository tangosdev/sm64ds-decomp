//cpp
// @symbol _ZN7fBase_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::Render() at 0x02043af0 -- vtable slot 9, per-frame draw.
 * Base fBase_c draws nothing and returns VS_FAIL (1); leaf classes override
 * slot 9 to do the actual rendering.
 */

typedef int s32;

struct fBase_c;

s32 fBase_c::Render()
{
    return 1; /* VS_FAIL */
}
