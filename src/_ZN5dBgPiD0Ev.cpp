//cpp
// @symbol _ZN5dBgPiD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: store this class's vptr, destroy the dBgPc
 * base sub-object, then return the object to its heap. None of that is
 * written here; declaring `~dBgPi()` is enough, because mwcc emits D2, D0 and
 * D1 together and objisolate keeps the one this file is bound to.
 *
 * The heap call is dBgPi's inline `operator delete` in include/dBgPi.h, which
 * routes to Memory::operator_delete2 (0x0203cbcc) -- the destination the ROM
 * body actually branches to. Without that member mwcc would call the global
 * `_ZdlPv` (0x0203cbf0) instead: same bytes, wrong relocation.
 */
#include "dBgPi.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dBgPi() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dBgPi *_ZN5dBgPiD0Ev(dBgPi *thiz)
{
    thiz->dBgPi::~dBgPi();         /* the D1 body, through the one host symbol */
    dBgPi::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dBgPi::~dBgPi()
{
}
#endif
