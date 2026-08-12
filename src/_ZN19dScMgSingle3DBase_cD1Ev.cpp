//cpp
// @symbol _ZN19dScMgSingle3DBase_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * ~dScMgSingle3DBase_c is defined in the class body rather than here, and
 * that is not a style choice -- every one of its 13 direct RTTI children
 * inlines this destructor's vptr store + mSysTracker destruction + chain
 * to ~dScMgBase_c(), the same way Stage inlines Scene's own D2 (see
 * include/Scene.h and src/_ZN5SceneD1Ev.cpp for the identical precedent).
 *
 * So this file cannot define it: that would be a redefinition. But the
 * vtable points at an out-of-line copy, and a TU that merely includes the
 * header emits nothing at all -- `_ZN19dScMgSingle3DBase_cD1Ev is not in
 * the object`. The explicit destructor call below is what forces the copy
 * into existence.
 *
 * It is never called. mwcc emits the destructor group when it sees the
 * call, objisolate keeps the one this file declares and drops the rest,
 * and the bytes at 0x0210a4b0 come out exactly as the ROM has them.
 */
#include "dScMgSingle3DBase_c.h"

/* Not called. Forces the out-of-line copy of the inline destructor above. */
void dScMgSingle3DBase_c_EmitDestructor(dScMgSingle3DBase_c *p)
{
    p->~dScMgSingle3DBase_c();
}
