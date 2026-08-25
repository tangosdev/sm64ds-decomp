//cpp
// @symbol _ZN11daDsnBase_cD1Ev
/* recovered: real C++ destructor -- inline in the class, forced out of line here
 *
 * daDsnBase_c.h defines `~daDsnBase_c() {}` in the class body on purpose, so
 * every descendant's destructor inlines it; a TU that merely includes the
 * header emits no _ZN11daDsnBase_cD1Ev, but the ROM carries one at ov091
 * 0x02132d6c. The explicit call below cannot be satisfied by the inline copy
 * alone, so mwcc emits the out-of-line variant and objisolate drops the
 * forcing function's own .text (notes/dtor-migration.md section 3). The body
 * it emits is the class's: the ShadowModel and TextureSequence members torn
 * down in reverse declaration order, the dBgActor_c base step last.
 */
#include "daDsnBase_c.h"

void _force_daDsnBase_cD1(daDsnBase_c *p)
{
    p->~daDsnBase_c();
}
