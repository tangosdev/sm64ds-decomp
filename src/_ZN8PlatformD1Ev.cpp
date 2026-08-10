//cpp
// @symbol _ZN8PlatformD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~Platform is defined in the class body, not here, and that is load-bearing:
 * the 72 classes derived from Platform inline this destructor's vptr store
 * rather than calling it, which the compiler can only do from a visible body.
 * include/ActorDerived.h records the measurement behind that.
 *
 * So this file cannot define it -- that would be a redefinition -- and a TU
 * that merely includes the header emits nothing. The explicit destructor call
 * below forces the out-of-line copy the vtable points at into existence. It is
 * never called, and objisolate drops it.
 *
 * The body it forces is the layout evidence for the whole class: the ROM
 * destroys a MovingMeshCollider at 0x124 and a Model at 0xd4, and both of
 * those offsets are where those classes' own asserted sizes put them.
 */
#include "Platform.h"

/* Not called. Forces the out-of-line copy of the inline destructor. */
void Platform_EmitDestructor(Platform *p)
{
    p->~Platform();
}
