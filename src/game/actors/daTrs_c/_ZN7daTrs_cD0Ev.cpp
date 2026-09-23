//cpp
// @symbol _ZN7daTrs_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The six sub-object destructors and the dCapEnemy_c base chain all follow from
 * include/daTrs_c.h's typed members; see the note there on why the body model is
 * `mBodyModel` and not `mModel`.
 */
#include "daTrs_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daTrs_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" daTrs_c *_ZN7daTrs_cD0Ev(daTrs_c *thiz)
{
    thiz->daTrs_c::~daTrs_c();       /* the D1 body, through the one host symbol */
    daTrs_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daTrs_c::~daTrs_c()
{
}
#endif
