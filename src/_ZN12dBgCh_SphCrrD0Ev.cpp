//cpp
// @symbol _ZN12dBgCh_SphCrrD0Ev
/* Compiler-owned deleting destructor. The empty body makes mwcc synthesize
 * the three MI vptr restores, member/base teardown, and class deallocation. */
#include "dBgCh_SphCrr.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dBgCh_SphCrr() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dBgCh_SphCrr *_ZN12dBgCh_SphCrrD0Ev(dBgCh_SphCrr *thiz)
{
    thiz->dBgCh_SphCrr::~dBgCh_SphCrr();  /* the D1 body, through the one host symbol */
    dBgCh_SphCrr::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dBgCh_SphCrr::~dBgCh_SphCrr()
{
}
#endif
