//cpp
// @symbol _ZN10dBgActor_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~dBgActor_c is defined in the class body, not here, and that is load-bearing:
 * the seventy-odd classes derived from dBgActor_c inline this destructor's vptr
 * store rather than calling it, which the compiler can only do from a visible
 * body. include/dBgActor_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable points
 * at into existence. It is never called; objisolate keeps the function this
 * file declares and drops the rest.
 *
 * The body it forces is the class's own layout evidence: the ROM destroys a
 * dBgW_KcMbg at 0x124 and a Model at 0xd4, and both offsets are where
 * those classes' asserted sizes put them.
 */
#include "dBgActor_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void Platform_EmitDestructor(dBgActor_c *p)
{
    p->~dBgActor_c();
}
