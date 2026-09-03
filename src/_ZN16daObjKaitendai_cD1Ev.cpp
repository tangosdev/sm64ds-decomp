//cpp
// @symbol _ZN16daObjKaitendai_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjKaitendai_c is defined in the class body, not here, and that is
 * load-bearing: this class's five descendants (daObjBk_Ukisima_c,
 * RotatingPlatformLll, daObjWc_Obj07_c, daObjRc_Kaitendai_c,
 * daObjKm3_Kaitendai_c) inline this destructor's vptr store rather than
 * calling it, which the compiler can only do from a visible body.
 * include/daObjKaitendai_c.h records the measurement.
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
 * This class's key function is Behavior (already out of line, in
 * src/_ZN16daObjKaitendai_c8BehaviorEv.cpp -- see
 * include/daObjKaitendai_c.h), so this file does not carry
 * _ZTV16daObjKaitendai_c / _ZTI16daObjKaitendai_c.
 */
#include "daObjKaitendai_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKaitendai_c_EmitDestructor(daObjKaitendai_c *p)
{
    p->~daObjKaitendai_c();
}
