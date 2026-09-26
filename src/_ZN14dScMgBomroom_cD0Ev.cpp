//cpp
// @symbol _ZN14dScMgBomroom_cD0Ev
#include "dScMgBomroom_c.h"
/* dScMgBomroom_c::~dScMgBomroom_c (D0, deleting destructor) -- dScMgBase_c's
   own operator delete (its immediate base) covers this; no per-class copy
   needed. */
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgBomroom_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgBomroom_c *_ZN14dScMgBomroom_cD0Ev(dScMgBomroom_c *thiz)
{
    thiz->dScMgBomroom_c::~dScMgBomroom_c();  /* the D1 body, through the one host symbol */
    dScMgBomroom_c::operator delete(thiz);    /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgBomroom_c::~dScMgBomroom_c()
{
}
#endif
