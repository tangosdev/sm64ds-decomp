//cpp
// @symbol _ZN10dScTitle_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~dScTitle_c()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "dScTitle_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1dScTitle_c@@QAE@XZ that
 * src/_ZN10dScTitle_cD1Ev.cpp defines out of line (include/dScTitle_c.h
 * declares ~dScTitle_c() with no slot under _MSC_VER), so this file cannot
 * carry the same definition: the host link refuses the pair (LNK2005). It
 * spells out, in terms of that one host symbol, what the variant this file is
 * enrolled for does: the D1 body, then the class-specific operator delete
 * (Memory::Deallocate with the game heap, the word at 0x020a0eac, from
 * dScene_c's inline operator delete). The qualified call is direct. The port's
 * slot 17 for this class (port_title_d0 in hal/title_vtable_seat14d.cpp) calls
 * this name. Nothing here reaches mwccarm: it builds the `#else` arm and emits
 * the ROM bytes it always emitted, and the object is byte-identical either
 * way. */
extern "C" dScTitle_c *_ZN10dScTitle_cD0Ev(dScTitle_c *thiz)
{
    thiz->dScTitle_c::~dScTitle_c();          /* the D1 body, through the one host symbol */
    dScTitle_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScTitle_c::~dScTitle_c()
{
}
#endif
