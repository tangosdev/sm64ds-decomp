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

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void Scene_EmitDeletingDestructor(dScene_c *p)
{
    delete p;
}
