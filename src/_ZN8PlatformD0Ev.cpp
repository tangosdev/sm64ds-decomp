//cpp
// @symbol _ZN8PlatformD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~Platform is defined in the class body -- the seventy-odd classes derived
 * from it inline its vptr store rather than calling it, which the compiler can
 * only do from a visible body. So this file cannot define it, and a TU that
 * merely includes the header emits nothing. The explicit destructor call below
 * forces the whole group into existence; objisolate keeps the one this file is
 * bound to and drops the rest.
 *
 * D0 is the deleting half: destroy through Platform and Actor, then hand the
 * object back through Actor's inline operator delete, which is why nothing
 * here mentions a heap.
 */
#include "Platform.h"

/* Not called. Forces the out-of-line copy of the inline destructor group. */
void Platform_EmitDeletingDestructor(Platform *p)
{
    p->~Platform();
}
