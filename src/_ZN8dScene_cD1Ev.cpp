//cpp
// @symbol _ZN8dScene_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~dScene_c is defined in the class body rather than here, and that is not a
 * style choice -- Stage::~Stage and every one of dScene_c's other nine direct
 * children inline this destructor's vptr store, which the compiler can only
 * do from a visible body. include/dScene_c.h records the measurement.
 *
 * So this file cannot define it: that would be a redefinition. But the
 * vtable points at an out-of-line copy, and a TU that merely includes the
 * header emits nothing at all -- `_ZN8dScene_cD1Ev is not in the object`. The
 * explicit destructor call below is what forces the copy into existence.
 *
 * It is never called. mwcc emits the destructor group when it sees the call,
 * objisolate keeps the one this file declares and drops the rest, and the
 * bytes at 0x0202e140 come out exactly as the ROM has them.
 */
#include "dScene_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor above. */
void Scene_EmitDestructor(dScene_c *p)
{
    p->~dScene_c();
}
