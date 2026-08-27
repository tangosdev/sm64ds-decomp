//cpp
// @symbol _ZN7dCcAc_cC1Ev
/* recovered: real C++ complete-object constructor
 *
 * The ROM's three steps are the dCc_c base step (_ZN5dCc_cC2Ev), the
 * _ZTV7dCcAc_c vptr store, and owner = 0 -- exactly what `dCcAc_c : dCc_c`
 * with a declared base constructor and an `owner(0)` initialiser means, so
 * the compiler emits all of it. The identical body in the C2 file is not
 * duplication: C1 and C2 are two of the functions one constructor emits, and
 * each file is bound to one of them by config/arm9/delinks.txt.
 */
#include "dCcAc_c.h"

dCcAc_c::dCcAc_c() : owner(0)
{
}
