//cpp
// @symbol _ZN13daObjMaruta_cD1Ev
/* recovered: real C++ destructor -- inline in the class, forced out of line here
 *
 * daObjMaruta_c.h defines `~daObjMaruta_c() {}` in the class body on purpose,
 * so every descendant's destructor inlines it; a TU that merely includes the
 * header emits no _ZN13daObjMaruta_cD1Ev, but the ROM carries one at ov080
 * 0x02127014. The explicit call below forces mwcc to emit the out-of-line
 * variant, and objisolate drops the forcing function's own .text
 * (notes/dtor-migration.md section 3).
 */
#include "daObjMaruta_c.h"

void _force_daObjMaruta_cD1(daObjMaruta_c *p)
{
    p->~daObjMaruta_c();
}
