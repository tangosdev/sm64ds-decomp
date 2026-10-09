//cpp
// @symbol _ZN11daPicGate_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 * D0 is the deleting destructor (vtable slot); mwcc emits D0/D1/D2 from one
 * ~daPicGate_c() and objisolate keeps the variant this TU is bound to. Body is
 * identical to the D1 file; delinks.txt binds each file to one symbol. */
#include "daPicGate_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daPicGate_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" daPicGate_c *_ZN11daPicGate_cD0Ev(daPicGate_c *thiz)
{
    thiz->daPicGate_c::~daPicGate_c();      /* the D1 body, through the one host symbol */
    daPicGate_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daPicGate_c::~daPicGate_c()
{
}
#endif
