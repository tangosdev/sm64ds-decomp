//cpp
// @symbol _ZN7dBase_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~dBase_c is defined in the class body rather than here, and that is not
 * a style choice -- dScene_c::~dScene_c and every actor destructor below it INLINE
 * this destructor's vptr store, which the compiler can only do from a visible
 * body. include/dBase_c.h records the measurement.
 *
 * So this file cannot define it: that would be a redefinition. But the vtable
 * points at an out-of-line copy, and a TU that merely includes the header
 * emits nothing at all -- `_ZN7dBase_cD1Ev is not in the object`. The
 * explicit destructor call below is what forces the copy into existence.
 *
 * It is never called. mwcc emits the destructor group when it sees the call,
 * objisolate keeps the one this file declares and drops the rest, and the
 * bytes at 0x02013e80 come out exactly as the ROM has them.
 */
#include "dBase_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor above. */
void ActorDerived_EmitDestructor(dBase_c *p)
{
    p->~dBase_c();
}
