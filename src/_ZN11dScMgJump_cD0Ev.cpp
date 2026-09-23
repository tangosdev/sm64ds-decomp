//cpp
// @symbol _ZN11dScMgJump_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN11dScMgJump_cD1Ev.cpp -- see that file's note. */
#include "dScMgJump_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgJump_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgJump_c *_ZN11dScMgJump_cD0Ev(dScMgJump_c *thiz)
{
    thiz->dScMgJump_c::~dScMgJump_c();   /* the D1 body, through the one host symbol */
    dScMgJump_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgJump_c::~dScMgJump_c()
{
    __cxa_vec_cleanup(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
}
#endif
