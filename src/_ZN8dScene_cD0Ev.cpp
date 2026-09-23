//cpp
// @symbol _ZN8dScene_cD0Ev
/* recovered: real C++ deleting destructor, forced from the inline definition
 *
 * ~dScene_c is defined in the class body (include/dScene_c.h), not here -- see the
 * long note there and in _ZN8dScene_cD1Ev.cpp: Stage and dScene_c's nine other
 * direct children inline dScene_c's own D2, which the compiler can only do from
 * a visible body. This file cannot define ~dScene_c() again (redefinition), so
 * a delete-expression forces the deleting destructor's own out-of-line copy
 * into existence instead -- the same role _ZN8dScene_cD1Ev.cpp's plain
 * destructor call plays for D1/D2.
 *
 * It is never called. objisolate keeps the symbol this file declares and
 * drops the rest, and the bytes at 0x0202e170 come out exactly as the ROM
 * has them: dScene_c's own vptr, then dBase_c's (inlined), then
 * fBase_c::~fBase_c(), then dScene_c's own operator delete.
 */
#include "dScene_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. ~dScene_c()
 * is defined in the class body, so there is no out-of-line host body for
 * a qualified call to reach: the call would inline a second copy of the
 * destructor. The D1 is reachable only by its flat ROM name, and that is
 * the call port/faces_sync.txt's face for this D0 made, so this arm makes
 * the same two calls that face made: the D1 body through its flat name,
 * then the class-specific operator delete. Nothing here reaches mwccarm:
 * it builds the `#else` arm and emits the ROM bytes it always emitted, and
 * the object is byte-identical either way. */
extern "C" void _ZN8dScene_cD1Ev(void *self);
extern "C" dScene_c *_ZN8dScene_cD0Ev(dScene_c *thiz)
{
    _ZN8dScene_cD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    dScene_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void Scene_EmitDeletingDestructor(dScene_c *p)
{
    delete p;
}
#endif
