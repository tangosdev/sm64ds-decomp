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

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC NEVER EMITS IT.
 * ~daObjSwdoor_c() is defined in the class body, and MSVC emits an inline
 * destructor only for a caller that odr-uses it. The port's slot 16 for this
 * class and the class's own D0 arm (src/_ZN13daObjSwdoor_cD0Ev.cpp) call the
 * flat name, so this arm defines it as the qualified, direct call that makes
 * MSVC emit the body: the text port/tools/dtorfwd.py generated for this name
 * into hal/dtor_forwarders_gen.cpp until run linkfull wave 27, and the same
 * object code. QUALIFIED on purpose: `self->~daObjSwdoor_c()` would be a
 * virtual call through the port's ROM-shaped table at slot 16, straight back
 * into this symbol. The inline chain MSVC emits stores an MSVC vftable
 * (??_7dBgActor_c@@6B@, measured) where the cartridge's D1 (0x020bab64, ov002)
 * stores _ZTV13daObjSwdoor_c; port/tools/dtor_store_guard.py reads this arm
 * and re-proves from the ROM body on every build that nothing dispatches
 * before the base destructor restores a ROM-shaped table. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted. */
extern "C" void _ZN13daObjSwdoor_cD1Ev(void *self)
{ ((daObjSwdoor_c *)self)->daObjSwdoor_c::~daObjSwdoor_c(); }
#else
/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjSwdoor_c_EmitDestructor(daObjSwdoor_c *p)
{
    p->~daObjSwdoor_c();
}
#endif
