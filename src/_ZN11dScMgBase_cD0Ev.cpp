//cpp
// @symbol _ZN11dScMgBase_cD0Ev
/* Real out-of-line definition, identical body to _ZN11dScMgBase_cD1Ev.cpp
   -- see that file's note and dScMgBase_c.h's own note. */
#include "dScMgBase_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgBase_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgBase_c *_ZN11dScMgBase_cD0Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();   /* the D1 body, through the one host symbol */
    dScMgBase_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
}
#endif
