//cpp
// @symbol _ZN13daObjSwdoor_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~daObjSwdoor_c is defined in the class body, not here, and that is
 * load-bearing: this class's two descendants (daObjBSwdoor_c, daObjCvShutter_c)
 * inline this destructor's vptr store rather than calling it, which the
 * compiler can only do from a visible body. include/daObjSwdoor_c.h records
 * the measurement.
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
 * THIS FILE NO LONGER CARRIES THE CLASS'S VAGUE-LINKAGE DATA. It used to:
 * with only the inline destructor declared, daObjSwdoor_c had no key
 * function anywhere and referencing the destructor group here made this TU
 * the one that defined _ZTV13daObjSwdoor_c and _ZTI13daObjSwdoor_c, the same
 * way src/_ZN8Vector3sD1Ev.cpp's own banner describes for a class with no
 * key function. Render (slot 9) is now declared out of line in
 * include/daObjSwdoor_c.h and defined in src/actors/daObjSwdoor_c.cpp, so
 * that TU is the key-function TU and anchors the vtable and typeinfo
 * instead. InitResources, CleanupResources and Behavior are pure here and
 * can never be a key function; each leaf supplies its own.
 * tools/objisolate.py reduces the object to the declared function before
 * eligible.py and rombuild.py judge it -- checked on this file, not
 * assumed.
 */
#include "daObjSwdoor_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjSwdoor_c_EmitDestructor(daObjSwdoor_c *p)
{
    p->~daObjSwdoor_c();
}
