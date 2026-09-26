//cpp
// @symbol _ZN15dScMgHanachan_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN15dScMgHanachan_cD1Ev.cpp -- see that file's note and
   dScMgHanachan_c.h's own note. dScMgBase_c's own operator delete (its
   immediate base) covers the Memory::Deallocate call the pre-migration
   source made explicitly; no per-class copy needed, same precedent as
   every other dScMgBase_c leaf's D0. The tree-wide OnYoshiTryEat mislabel
   on this class landed HERE (its source comment said "recovered name:
   dScMgHanachan_c_OnYoshiTryEat") rather than on the real OnYoshiTryEat-
   shaped slot 18 helper -- this body is the textbook D0 shape (vtable
   write, array destroy, base D2, Deallocate), not gameplay logic. */
#include "dScMgHanachan_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgHanachan_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgHanachan_c *_ZN15dScMgHanachan_cD0Ev(dScMgHanachan_c *thiz)
{
    thiz->dScMgHanachan_c::~dScMgHanachan_c();  /* the D1 body, through the one host symbol */
    dScMgHanachan_c::operator delete(thiz);     /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgHanachan_c::~dScMgHanachan_c()
{
    __cxa_vec_cleanup((char *)this + 0x4678, 0xf, 0x98, (void *)func_ov006_020ea324);
}
#endif
