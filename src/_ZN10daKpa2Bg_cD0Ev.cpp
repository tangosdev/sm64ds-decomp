//cpp
// @symbol _ZN10daKpa2Bg_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy -- through both this class and its dBgActor_c base,
 * which is why two vptr stores appear -- then return the object to the actor heap.
 * The deallocation is the inline dActor_c::operator delete, which is why nothing here
 * mentions the heap.
 */
#include "daKpa2Bg_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daKpa2Bg_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" daKpa2Bg_c *_ZN10daKpa2Bg_cD0Ev(daKpa2Bg_c *thiz)
{
    thiz->daKpa2Bg_c::~daKpa2Bg_c();    /* the D1 body, through the one host symbol */
    daKpa2Bg_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daKpa2Bg_c::~daKpa2Bg_c()
{
}
#endif
