//cpp
// @symbol _ZN15daObjGuragura_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjGuragura_c is defined in the class body, not here, and that is
 * load-bearing: this class's two descendants (TiltingPlatformBfs,
 * TiltingPlatformLll) inline this destructor's vptr store rather than
 * calling it, which the compiler can only do from a visible body.
 * include/daObjGuragura_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * The body it forces is dBgActor_c's own layout: this class's own fields
 * (mTilt, mTiltTarget, mBumped, mSettleDelay) have no destructor, so the
 * compiler-generated body destroys only the inherited dBgW_KcMbg at
 * 0x124 and Model at 0xd4, the same as _ZN10dBgActor_cD1Ev.
 *
 * include/daObjGuragura_c.h's own banner explains why this file does not
 * carry _ZTV15daObjGuragura_c / _ZTI15daObjGuragura_c: the class's key
 * function is Render, declared first and still unmigrated, so no TU
 * currently defines the vtable and none of this class's out-of-line member
 * files -- Behavior's or this one -- pull it in.
 */
#include "daObjGuragura_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjGuragura_c_EmitDestructor(daObjGuragura_c *p)
{
    p->~daObjGuragura_c();
}
