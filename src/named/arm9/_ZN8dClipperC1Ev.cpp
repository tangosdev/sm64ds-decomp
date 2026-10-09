//cpp
// @symbol _ZN8dClipperC1Ev
/* recovered: real C++ constructor. The ROM stores _ZTV8dClipper and then calls
 * the shared init helper -- vptr-then-body per the measured emission order
 * (notes/ctor-migration.md section 6); there are no bases or members, so only
 * the body statement is written here.
 *
 * __sinit_02074e84 constructs the lone instance explicitly and declares this
 * symbol itself; it does not include dClipper.h.
 */
#include "dClipper.h"

dClipper::dClipper()
{
    Func_020156DC(0x1555, 0xe38, 0x1000, 0x1388000);
}
