//cpp
// @symbol _ZN13daObjDorifu_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjDorifu_c is defined in the class body, not here, and that is
 * load-bearing: this class's three descendants (daObjKm1_Dorifu_c,
 * daObjRc_Dorifu_c, daObjKm3_Dorifu_c/daObjKm3_Dorifu_c) inline this
 * destructor's vptr store rather than calling it, which the compiler can
 * only do from a visible body. include/daObjDorifu_c.h records the
 * measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable
 * points at into existence. It is never called; objisolate keeps the
 * function this file declares and drops the rest.
 *
 * UNLIKE ITS SEVEN SIBLINGS IN THIS SERIES, this class's own destructor body
 * is not a bare copy of dBgActor_c's: it also owns a dBgW_KcMbg[5] at
 * 0x4b0 and a Model[5] at 0x320 (include/daObjDorifu_c.h), so the implicit
 * member-array cleanup the compiler generates for the empty `{}` body runs
 * BEFORE the vptr is restored to dBgActor_c's and the inherited dBgW_KcMbg
 * (0x124) and Model (0xd4) are destroyed -- the same order the pre-existing
 * src/func_ov002_020b4af8.c placeholder already showed.
 *
 * This class's key function is Behavior (already out of line, in
 * src/_ZN13daObjDorifu_c8BehaviorEv.cpp -- see include/daObjDorifu_c.h),
 * so this file does not carry _ZTV13daObjDorifu_c / _ZTI13daObjDorifu_c.
 */
#include "daObjDorifu_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjDorifu_c_EmitDestructor(daObjDorifu_c *p)
{
    p->~daObjDorifu_c();
}
