//cpp
// @symbol _ZN16daObjFallBlock_cD1Ev
/* recovered: real C++ destructor -- inline in the class, forced out of line here
 *
 * daObjFallBlock_c.h defines `~daObjFallBlock_c() {}` in the class body on
 * purpose, so every descendant's destructor inlines it; a TU that merely
 * includes the header emits no _ZN16daObjFallBlock_cD1Ev, but the ROM carries
 * one at ov098 0x02139fc8. The explicit call below forces mwcc to emit the
 * out-of-line variant, and objisolate drops the forcing function's own .text
 * (notes/dtor-migration.md section 3).
 */
#include "daObjFallBlock_c.h"

void _force_daObjFallBlock_cD1(daObjFallBlock_c *p)
{
    p->~daObjFallBlock_c();
}
