//cpp
// @symbol _ZN13daObjDorifu_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~daObjDorifu_c is defined in the class body -- this class's three
 * descendants inline its vptr store rather than calling it, which the
 * compiler can only do from a visible body. So this file cannot define it,
 * and a TU that merely includes the header emits nothing. The delete-
 * expression below forces the deleting destructor's own out-of-line copy
 * into existence; objisolate keeps the one this file is bound to and drops
 * the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~daObjDorifu_c()`, for the same reason
 * src/_ZN10dBgActor_cD0Ev.cpp gives: dBgActor_c declares Kill, a key
 * function reachable from this class, so a plain destructor call would not
 * reproduce D0 out of line. `delete p` asks for the deleting half by name
 * and brings it back.
 *
 * The array cleanup is this class's own, same as its D1 sibling: a
 * dBgW_KcMbg[5] at 0x4b0 and a Model[5] at 0x320 are destroyed
 * before the vptr is restored to dBgActor_c's, then the inherited
 * dBgW_KcMbg (0x124) and Model (0xd4), then dActor_c's base
 * destructor, then the object is handed back through dActor_c's inline
 * operator delete.
 */
#include "daObjDorifu_c.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjDorifu_c_EmitDeletingDestructor(daObjDorifu_c *p)
{
    delete p;
}
