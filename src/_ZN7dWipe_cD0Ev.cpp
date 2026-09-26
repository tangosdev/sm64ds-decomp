//cpp
// @symbol _ZN7dWipe_cD0Ev
/* recovered: real C++ destructor -- the compiler emits everything but the body
 *
 * D0 is the DELETING destructor, vtable slot 1. Only the `if` below is
 * written source; the vptr store that precedes it and the FaderColor base
 * sub-object destructor (D2) that follows it are both consequences of
 * `struct dWipe_c : FaderColor` and land in exactly the ROM's order.
 *
 * The body itself is this class's one real obligation: if the per-scanline
 * hardware capture is still armed, cancel it before the object goes away.
 *
 * Defining the destructor out of line makes this the key-function TU, so mwcc
 * emits _ZTV7dWipe_c and the D1 variant beside D0; objisolate keeps the variant
 * this file is bound to and rebinds the vtable reference to the ROM's table at
 * 0x020926f0.
 */
#include "dWipe_c.h"
#include "decl_common.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dWipe_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dWipe_c *_ZN7dWipe_cD0Ev(dWipe_c *thiz)
{
    thiz->dWipe_c::~dWipe_c();       /* the D1 body, through the one host symbol */
    dWipe_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dWipe_c::~dWipe_c()
{
    if (needsCleanup == 1)
        func_0202fb30(this);
}
#endif
