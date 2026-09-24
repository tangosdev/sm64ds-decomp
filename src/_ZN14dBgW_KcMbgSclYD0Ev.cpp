//cpp
// @symbol _ZN14dBgW_KcMbgSclYD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~dBgW_KcMbgSclY()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "dBgW_KcMbgSclY.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dBgW_KcMbgSclY() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dBgW_KcMbgSclY *_ZN14dBgW_KcMbgSclYD0Ev(dBgW_KcMbgSclY *thiz)
{
    thiz->dBgW_KcMbgSclY::~dBgW_KcMbgSclY();  /* the D1 body, through the one host symbol */
    dBgW_KcMbgSclY::operator delete(thiz);    /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dBgW_KcMbgSclY::~dBgW_KcMbgSclY()
{
}
#endif
