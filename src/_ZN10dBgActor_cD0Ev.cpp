//cpp
// @symbol _ZN10dBgActor_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~dBgActor_c is defined in the class body -- the seventy-odd classes derived
 * from it inline its vptr store rather than calling it, which the compiler can
 * only do from a visible body. So this file cannot define it, and a TU that
 * merely includes the header emits nothing. The delete-expression below forces
 * the deleting destructor's own out-of-line copy into existence; objisolate
 * keeps the one this file is bound to and drops the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~dBgActor_c()`, AND THE DIFFERENCE IS NEW. While
 * dBgActor_c declared no virtual but its inline destructor it had no key
 * function, so any TU that touched the destructor group emitted all of it and
 * a plain destructor call was enough. dBgActor_c now declares Kill (slot 31, see
 * the header), which IS a key function -- so D0 is emitted only where the
 * vtable is, and the destructor call here stopped producing it: this file
 * compiled to D1 plus the forcing function, two .text sections, and dropped
 * out of the build with every byte gate still green. `delete p` asks for the
 * deleting half by name and brings it back. Same shape, same fix, same
 * sentence in src/actors/dScMgSingle3DBase_c.cpp, which has had a key
 * function since #1544.
 *
 * D0 is the deleting half: destroy through dBgActor_c and dActor_c, then hand the
 * object back through dActor_c's inline operator delete, which is why nothing
 * here mentions a heap.
 */
#include "dBgActor_c.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void Platform_EmitDeletingDestructor(dBgActor_c *p)
{
    delete p;
}
