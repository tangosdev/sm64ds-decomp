//cpp
// @symbol _ZN8PlatformD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~Platform is defined in the class body -- the seventy-odd classes derived
 * from it inline its vptr store rather than calling it, which the compiler can
 * only do from a visible body. So this file cannot define it, and a TU that
 * merely includes the header emits nothing. The delete-expression below forces
 * the deleting destructor's own out-of-line copy into existence; objisolate
 * keeps the one this file is bound to and drops the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~Platform()`, AND THE DIFFERENCE IS NEW. While
 * Platform declared no virtual but its inline destructor it had no key
 * function, so any TU that touched the destructor group emitted all of it and
 * a plain destructor call was enough. Platform now declares Kill (slot 31, see
 * the header), which IS a key function -- so D0 is emitted only where the
 * vtable is, and the destructor call here stopped producing it: this file
 * compiled to D1 plus the forcing function, two .text sections, and dropped
 * out of the build with every byte gate still green. `delete p` asks for the
 * deleting half by name and brings it back. Same shape, same fix, same
 * sentence in src/_ZN19dScMgSingle3DBase_cD0Ev.cpp, which has had a key
 * function since #1544.
 *
 * D0 is the deleting half: destroy through Platform and Actor, then hand the
 * object back through Actor's inline operator delete, which is why nothing
 * here mentions a heap.
 */
#include "Platform.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void Platform_EmitDeletingDestructor(Platform *p)
{
    delete p;
}
