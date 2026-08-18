//cpp
// @symbol _ZN19dScMgSingle3DBase_cD0Ev
/* recovered: real C++ deleting destructor, forced from the inline definition
 *
 * ~dScMgSingle3DBase_c is defined in the class body
 * (include/dScMgSingle3DBase_c.h), not here -- see the long note there and
 * in _ZN19dScMgSingle3DBase_cD1Ev.cpp: this class's 13 direct RTTI children
 * inline its own D2, which the compiler can only do from a visible body.
 * This file cannot define ~dScMgSingle3DBase_c() again (redefinition), so
 * a delete-expression forces the deleting destructor's own out-of-line copy
 * into existence instead -- the same role _ZN19dScMgSingle3DBase_cD1Ev.cpp's
 * plain destructor call plays for D1/D2. Same precedent as
 * src/_ZN8dScene_cD0Ev.cpp.
 *
 * It is never called. objisolate keeps the symbol this file declares and
 * drops the rest, and the bytes at 0x0210a4e8 come out exactly as the ROM
 * has them.
 */
#include "dScMgSingle3DBase_c.h"

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void dScMgSingle3DBase_c_EmitDeletingDestructor(dScMgSingle3DBase_c *p)
{
    delete p;
}
