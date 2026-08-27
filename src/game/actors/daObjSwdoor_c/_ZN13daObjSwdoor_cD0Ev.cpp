//cpp
// @symbol _ZN13daObjSwdoor_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~daObjSwdoor_c is defined in the class body -- this class's two
 * descendants inline its vptr store rather than calling it, which the
 * compiler can only do from a visible body. So this file cannot define it,
 * and a TU that merely includes the header emits nothing. The delete-
 * expression below forces the deleting destructor's own out-of-line copy
 * into existence; objisolate keeps the one this file is bound to and drops
 * the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~daObjSwdoor_c()`, for the same reason
 * src/_ZN10dBgActor_cD0Ev.cpp gives: dBgActor_c declares Kill, a key
 * function reachable from this class, so a plain destructor call would not
 * reproduce D0 out of line. `delete p` asks for the deleting half by name
 * and brings it back.
 *
 * UNLIKE ITS SEVEN SIBLINGS IN THIS SERIES, daObjSwdoor_c has no other
 * out-of-line virtual and so no key function elsewhere either -- see
 * src/_ZN13daObjSwdoor_cD1Ev.cpp. This TU is expected to define
 * _ZTV13daObjSwdoor_c and _ZTI13daObjSwdoor_c too, alongside D1, as
 * vague-linkage data; objisolate reduces the object to the declared D0
 * function before eligible.py and rombuild.py judge it.
 *
 * D0 is the deleting half: destroy through daObjSwdoor_c and dBgActor_c,
 * then hand the object back through dActor_c's inline operator delete, which
 * is why nothing here mentions a heap.
 */
#include "daObjSwdoor_c.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjSwdoor_c_EmitDeletingDestructor(daObjSwdoor_c *p)
{
    delete p;
}
