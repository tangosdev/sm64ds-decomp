//cpp
// @symbol _ZN8PlatformD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~Platform is defined in the class body, not here, and that is load-bearing:
 * the seventy-odd classes derived from Platform inline this destructor's vptr
 * store rather than calling it, which the compiler can only do from a visible
 * body. include/Platform.h records the measurement.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing at all. The explicit
 * destructor call below is what forces the out-of-line copy the vtable points
 * at into existence. It is never called; objisolate keeps the function this
 * file declares and drops the rest.
 *
 * The body it forces is the class's own layout evidence: the ROM destroys a
 * MovingMeshCollider at 0x124 and a Model at 0xd4, and both offsets are where
 * those classes' asserted sizes put them.
 */
#include "Platform.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void Platform_EmitDestructor(Platform *p)
{
    p->~Platform();
}
