//cpp
// @symbol _ZN17daObjFloatBoard_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~daObjFloatBoard_c is defined in the class body -- this class's three
 * descendants inline its vptr store rather than calling it, which the
 * compiler can only do from a visible body. So this file cannot define it,
 * and a TU that merely includes the header emits nothing. The delete-
 * expression below forces the deleting destructor's own out-of-line copy
 * into existence; objisolate keeps the one this file is bound to and drops
 * the rest.
 *
 * A DELETE-EXPRESSION, NOT `p->~daObjFloatBoard_c()`. This class's key
 * function is Behavior, defined out of line elsewhere
 * (src/_ZN17daObjFloatBoard_c8BehaviorEv.cpp), so a plain destructor call
 * here would not produce D0 out of line -- the same shape
 * src/_ZN10dBgActor_cD0Ev.cpp documents once Kill became dBgActor_c's key
 * function. `delete p` asks for the deleting half by name and brings it
 * back.
 *
 * D0 is the deleting half: destroy through daObjFloatBoard_c and dBgActor_c,
 * then hand the object back through dActor_c's inline operator delete, which
 * is why nothing here mentions a heap.
 */
#include "daObjFloatBoard_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~daObjFloatBoard_c()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN17daObjFloatBoard_cD1Ev(void *self);
extern "C" daObjFloatBoard_c *_ZN17daObjFloatBoard_cD0Ev(daObjFloatBoard_c *thiz)
{
    _ZN17daObjFloatBoard_cD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    daObjFloatBoard_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjFloatBoard_c_EmitDeletingDestructor(daObjFloatBoard_c *p)
{
    delete p;
}
#endif
