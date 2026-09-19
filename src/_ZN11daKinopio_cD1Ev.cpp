//cpp
// @symbol _ZN11daKinopio_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and three destructor calls, every one a consequence of
 * `struct daKinopio_c : dActor_c` and the members that declaration types:
 * its own vptr, then ShadowModel (0x16c), ModelAnim (0x108) and dCcAc_c (0x0d4)
 * in reverse declaration order, then dActor_c::~dActor_c.
 *
 * This body is the evidence for the header. The factory at 0x02129cd0
 * constructs the same three types at the same three offsets.
 */
#include "daKinopio_c.h"

daKinopio_c::~daKinopio_c()
{
}
