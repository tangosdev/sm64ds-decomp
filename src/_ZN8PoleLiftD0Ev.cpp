//cpp
// @symbol _ZN8PoleLiftD0Ev
/* A delete-expression forces mwccarm to emit the deleting destructor. The
 * inherited inline dActor_c::operator delete supplies the actor-heap release
 * used by the ROM, which is why nothing below mentions a heap.
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap.
 */
#include "PoleLift.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~PoleLift()
 * is defined in the class body, so a qualified call from here would inline
 * a second copy of the whole destructor chain into this arm. The port's D1
 * for this class is its flat ROM name, the name the class's vtable slot 16
 * holds, so this arm makes the D0's two calls: the D1 body through that
 * flat name, then the class-specific operator delete. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" void _ZN8PoleLiftD1Ev(void *self);
extern "C" PoleLift *_ZN8PoleLiftD0Ev(PoleLift *thiz)
{
    _ZN8PoleLiftD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    PoleLift::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void PoleLift_EmitDeletingDestructor(PoleLift *p)
{
    delete p;
}
#endif
