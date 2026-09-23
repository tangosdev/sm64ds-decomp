//cpp
// @symbol _ZN7TornadoD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. */
#include "Tornado.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~Tornado()
 * is defined in the class body, so there is no out-of-line host body for
 * a qualified call to reach: the call would inline a second copy of the
 * destructor. The D1 is reachable only by its flat ROM name, and that is
 * the call port/faces_sync.txt's face for this D0 made, so this arm makes
 * the same two calls that face made: the D1 body through its flat name,
 * then the class-specific operator delete. Nothing here reaches mwccarm:
 * it builds the `#else` arm and emits the ROM bytes it always emitted, and
 * the object is byte-identical either way. */
extern "C" void _ZN7TornadoD1Ev(void *self);
extern "C" Tornado *_ZN7TornadoD0Ev(Tornado *thiz)
{
    _ZN7TornadoD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    Tornado::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
void Tornado_EmitDeletingDestructor(Tornado *tornado)
{
    delete tornado;
}
#endif
