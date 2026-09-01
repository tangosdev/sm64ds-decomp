//cpp
// @symbol _ZN17daObjFloatBoard_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjFloatBoard_c is defined in the class body, not here, and that is
 * load-bearing: this class's three descendants (FloatOnWaterPlatformJrb,
 * daObjWcObj01_c, daObjWcObj06_c) inline this
 * destructor's vptr store rather than calling it, which the compiler can only
 * do from a visible body. include/daObjFloatBoard_c.h records the
 * measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * The body it forces is dBgActor_c's own layout: this class adds fields of
 * its own (mWaterY etc.) but none of them has a destructor, so the compiler-
 * generated body destroys only the inherited dBgW_KcMbg at 0x124 and
 * Model at 0xd4, the same as _ZN10dBgActor_cD1Ev.
 *
 * This class's key function is Behavior (src/_ZN17daObjFloatBoard_c8BehaviorEv.cpp),
 * already out of line, so this file does not need to (and does not) carry
 * _ZTV17daObjFloatBoard_c / _ZTI17daObjFloatBoard_c.
 */
#include "daObjFloatBoard_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjFloatBoard_c_EmitDestructor(daObjFloatBoard_c *p)
{
    p->~daObjFloatBoard_c();
}
