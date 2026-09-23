//cpp
// @symbol _ZN12dScMgAmida_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN12dScMgAmida_cD1Ev.cpp -- see that file's note and
   include/dScMgAmida_c.h's own class banner. dScMgBase_c's own operator
   delete (its immediate base) covers the Memory::Deallocate call the
   pre-migration source made explicitly; no per-class copy needed, same
   precedent as every other dScMgBase_c leaf's D0. The tree-wide
   OnYoshiTryEat mislabel on this class landed HERE too (its source comment
   said "recovered name: dScMgAmida_c_OnYoshiTryEat") rather than on the
   real OnYoshiTryEat-shaped slot 18 helper -- this body is the textbook D0
   shape (vtable write, four array destroys, base D2, Deallocate), not
   gameplay logic, same mislabel dScMgHanachan_c's own D0 carried. */
#include "dScMgAmida_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgAmida_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgAmida_c *_ZN12dScMgAmida_cD0Ev(dScMgAmida_c *thiz)
{
    thiz->dScMgAmida_c::~dScMgAmida_c();  /* the D1 body, through the one host symbol */
    dScMgAmida_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgAmida_c::~dScMgAmida_c()
{
    __cxa_vec_cleanup((char *)this + 0x4768, 0x80, 0x18, (void *)func_ov006_020d116c);
    __cxa_vec_cleanup((char *)this + 0x4744, 4, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup((char *)this + 0x4724, 4, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup((char *)this + 0x4660, 4, 8, (void *)NullDestructor_0203d47c);
}
#endif
