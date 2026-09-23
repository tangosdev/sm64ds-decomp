//cpp
#include "View.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~View()
 * is defined in the class body, so there is no out-of-line host body for
 * a qualified call to reach: the call would inline a second copy of the
 * destructor. The D1 is reachable only by its flat ROM name, and that is
 * the call port/faces_sync.txt's face for this D0 made, so this arm makes
 * the same two calls that face made: the D1 body through its flat name,
 * then the class-specific operator delete. Nothing here reaches mwccarm:
 * it builds the `#else` arm and emits the ROM bytes it always emitted, and
 * the object is byte-identical either way. */
extern "C" void _ZN4ViewD1Ev(void *self);
extern "C" View *_ZN4ViewD0Ev(View *thiz)
{
    _ZN4ViewD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    View::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the deleting D0 copy of the inline destructor. */
void View_EmitDeletingDestructor(View *view)
{
    delete view;
}
#endif
