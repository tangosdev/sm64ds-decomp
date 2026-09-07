//cpp
// @symbol _ZN7ClipperC1Ev
/* recovered: real C++ constructor. The ROM stores _ZTV7Clipper and then calls
 * the shared init helper -- vptr-then-body per the measured emission order
 * (notes/ctor-migration.md section 6); there are no bases or members, so only
 * the body statement is written here.
 *
 * __sinit_02074e84 constructs the lone instance explicitly and declares this
 * symbol itself; it does not include Clipper.h.
 */
#include "Clipper.h"

Clipper::Clipper()
{
    Func_020156DC(0x1555, 0xe38, 0x1000, 0x1388000);
}
