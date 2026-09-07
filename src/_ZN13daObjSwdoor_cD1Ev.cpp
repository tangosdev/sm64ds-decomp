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
 * UNLIKE ITS SEVEN SIBLINGS IN THIS SERIES, this class declares no
 * out-of-line virtual at all yet: Render, InitResources and
 * CleanupResources are all still unmigrated and none of them is spelled as
 * a member function in include/daObjSwdoor_c.h (see its banner: "ABSTRACT
 * IN THREE SLOTS"). With only the inline destructor declared, this class
 * currently has NO key function, so referencing the destructor group here
 * makes THIS TU the one that defines _ZTV13daObjSwdoor_c and
 * _ZTI13daObjSwdoor_c as vague-linkage data, the same way
 * src/_ZN8Vector3sD1Ev.cpp's own banner describes for a class with no key
 * function. tools/objisolate.py reduces the object to the declared function
 * before eligible.py and rombuild.py judge it -- checked on this file, not
 * assumed.
 */
#include "daObjSwdoor_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjSwdoor_c_EmitDestructor(daObjSwdoor_c *p)
{
    p->~daObjSwdoor_c();
}
