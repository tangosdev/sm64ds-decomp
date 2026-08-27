//cpp
// @symbol _ZN14daObjUkiyuka_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjUkiyuka_c is defined in the class body, not here, and that is
 * load-bearing: this class's descendants (FloatingFloorLllSmall and its
 * sibling) inline this destructor's vptr store rather than calling it,
 * which the compiler can only do from a visible body.
 * include/daObjUkiyuka_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * The body it forces is dBgActor_c's own layout: this class's own fields
 * (mRestY, mBobAmplitude, mBobPhase, mRestTimer) have no destructor, so the
 * compiler-generated body destroys only the inherited dBgW_KcMbg at
 * 0x124 and Model at 0xd4, the same as _ZN10dBgActor_cD1Ev.
 *
 * This class's key function is Behavior (already out of line, in
 * src/_ZN14daObjUkiyuka_c8BehaviorEv.cpp -- see include/daObjUkiyuka_c.h),
 * so this file does not carry _ZTV14daObjUkiyuka_c / _ZTI14daObjUkiyuka_c.
 */
#include "daObjUkiyuka_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjUkiyuka_c_EmitDestructor(daObjUkiyuka_c *p)
{
    p->~daObjUkiyuka_c();
}
