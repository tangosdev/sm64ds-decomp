//cpp
// @symbol _ZN7daOts_cD1Ev
/* recovered: real C++ destructor -- inline in the class, forced out of line here
 *
 * daOts_c.h defines `~daOts_c() {}` in the class body on purpose, so every
 * descendant's destructor inlines it; a TU that merely includes the header
 * emits no _ZN7daOts_cD1Ev, but the ROM carries one at ov064 0x02115ee0. The
 * explicit call below forces mwcc to emit the out-of-line variant, and
 * objisolate drops the forcing function's own .text
 * (notes/dtor-migration.md section 3).
 */
#include "daOts_c.h"

void _force_daOts_cD1(daOts_c *p)
{
    p->~daOts_c();
}
