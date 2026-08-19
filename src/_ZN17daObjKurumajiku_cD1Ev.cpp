//cpp
// @symbol _ZN17daObjKurumajiku_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjKurumajiku_c is defined in the class body, not here, and that is
 * load-bearing: this class's two descendants (RickshawBdw and the ov047
 * sibling daObjKm3_Kurumajiku_c) inline this destructor's vptr store rather
 * than calling it, which the compiler can only do from a visible body.
 * include/daObjKurumajiku_c.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * The body it forces is dBgActor_c's own layout: this class's own field
 * (mMountedActorIds, a plain u32[4]) has no destructor, so the compiler-
 * generated body destroys only the inherited dBgW_KcMbg at 0x124
 * and Model at 0xd4, the same as _ZN10dBgActor_cD1Ev.
 *
 * This class's key function is Behavior (already out of line, in
 * src/_ZN17daObjKurumajiku_c8BehaviorEv.cpp -- see
 * include/daObjKurumajiku_c.h), so this file does not carry
 * _ZTV17daObjKurumajiku_c / _ZTI17daObjKurumajiku_c.
 */
#include "daObjKurumajiku_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKurumajiku_c_EmitDestructor(daObjKurumajiku_c *p)
{
    p->~daObjKurumajiku_c();
}
