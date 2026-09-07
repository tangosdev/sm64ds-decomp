//cpp
// @symbol _ZN4dBgWC2Ev
/* recovered: real C++ constructor -- the compiler emits the vptr store
 *
 * The ROM stores _ZTV4dBgW and THEN calls func_02039624, which is the
 * order the probe in notes/ctor-migration.md section 6 produces for a
 * body call: mwcc writes a most-derived vptr before the first statement.
 * func_02039624's field pokes (+0x04..+0x1c) are therefore body work, not
 * a base step -- dBgW is a root class. The helper keeps its generic name:
 * it is shared with SetFile, which re-runs it (include/dBgW.h LAYOUT note).
 * One out-of-line definition also emits a C1 sibling; objisolate strips it,
 * as for every ctor here (section 1).
 */
#include "dBgW.h"

extern "C" void func_02039624(void *self);

dBgW::dBgW()
{
    func_02039624(this);
}
