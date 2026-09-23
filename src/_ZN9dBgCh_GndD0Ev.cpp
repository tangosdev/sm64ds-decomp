//cpp
// @symbol _ZN9dBgCh_GndD0Ev
/* Compiler-owned deleting destructor. The empty body is the original source
 * shape: mwcc restores both MI vptrs, destroys the bases in reverse order,
 * and then calls dBgCh_Gnd::operator delete. */
#include "dBgCh_Gnd.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dBgCh_Gnd() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dBgCh_Gnd *_ZN9dBgCh_GndD0Ev(dBgCh_Gnd *thiz)
{
    thiz->dBgCh_Gnd::~dBgCh_Gnd();     /* the D1 body, through the one host symbol */
    dBgCh_Gnd::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dBgCh_Gnd::~dBgCh_Gnd()
{
}
#endif
