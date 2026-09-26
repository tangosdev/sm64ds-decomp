//cpp
// @symbol _ZN12daDossyCap_cD0Ev
/* A delete-expression forces mwccarm to materialize daDossyCap_c's deleting
 * destructor and use dActor_c's actor-heap deallocator. */
#include "daDossyCap_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~daDossyCap_c()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN12daDossyCap_cD1Ev(void *self);
extern "C" daDossyCap_c *_ZN12daDossyCap_cD0Ev(daDossyCap_c *thiz)
{
    _ZN12daDossyCap_cD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    daDossyCap_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void daDossyCap_c_EmitDeletingDestructor(daDossyCap_c *cap)
{
    delete cap;
}
#endif
