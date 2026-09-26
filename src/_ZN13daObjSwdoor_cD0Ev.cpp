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
 * THIS TU NO LONGER DEFINES _ZTV13daObjSwdoor_c OR _ZTI13daObjSwdoor_c. It
 * did while the class had no out-of-line virtual anywhere -- see
 * src/_ZN13daObjSwdoor_cD1Ev.cpp. Render (slot 9) is now defined in
 * src/actors/daObjSwdoor_c.cpp, which is therefore the key-function TU and
 * carries that data; objisolate reduces this object to the declared D0
 * function before eligible.py and rombuild.py judge it.
 *
 * D0 is the deleting half: destroy through daObjSwdoor_c and dBgActor_c,
 * then hand the object back through dActor_c's inline operator delete, which
 * is why nothing here mentions a heap.
 */
#include "daObjSwdoor_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~daObjSwdoor_c()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN13daObjSwdoor_cD1Ev(void *self);
extern "C" daObjSwdoor_c *_ZN13daObjSwdoor_cD0Ev(daObjSwdoor_c *thiz)
{
    _ZN13daObjSwdoor_cD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    daObjSwdoor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjSwdoor_c_EmitDeletingDestructor(daObjSwdoor_c *p)
{
    delete p;
}
#endif
