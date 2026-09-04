//cpp
// @symbol _ZN13daObjKuruma_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjKuruma_c is defined in the class body, not here, and that is
 * load-bearing: this class's two descendants (daObjKm1_Kuruma_c and the
 * unnamed ov047 sibling daObjKm3_Kuruma_c) inline this destructor's vptr
 * store rather than calling it, which the compiler can only do from a
 * visible body. include/daObjKuruma_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * The body it forces is dBgActor_c's own layout: this class has no fields of
 * its own, so the compiler-generated body destroys only the inherited
 * dBgW_KcMbg at 0x124 and Model at 0xd4, the same as
 * _ZN10dBgActor_cD1Ev.
 *
 * include/daObjKuruma_c.h's own banner explains why this file does not
 * carry _ZTV13daObjKuruma_c / _ZTI13daObjKuruma_c: the class's key function
 * is Render, declared first and still unmigrated, so no TU currently
 * defines the vtable and none of this class's out-of-line member files --
 * Behavior's or this one -- pull it in.
 */
#include "daObjKuruma_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKuruma_c_EmitDestructor(daObjKuruma_c *p)
{
    p->~daObjKuruma_c();
}
