//cpp
// @symbol _ZN16dPathLiftActor_cD0Ev
/* The deleting (D0) half of the dPathLiftActor_c lifecycle pair. A
 * `delete` expression is what materialises D0 -- destroy through the class and
 * its bases, which is why more than one vptr store appears, then hand the
 * object to the inherited operator delete.
 */
#include "PathLift.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~dPathLiftActor_c()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN16dPathLiftActor_cD1Ev(void *self);
extern "C" dPathLiftActor_c *_ZN16dPathLiftActor_cD0Ev(dPathLiftActor_c *thiz)
{
    _ZN16dPathLiftActor_cD1Ev(thiz);          /* the D1 body, through its flat ROM name */
    dPathLiftActor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void dPathLiftActor_c_EmitDeletingDestructor(dPathLiftActor_c *p)
{
    delete p;
}
#endif
