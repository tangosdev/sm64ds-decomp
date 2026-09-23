//cpp
// @symbol _ZN10HootTheOwlD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~HootTheOwl()`
 * is enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reached because
 * dEnemyBase_c is this class's IMMEDIATE base -- which is why nothing below mentions a
 * heap.
 *
 * The C original stored `_ZTV7daOwl_c` here where D1 stored `_ZTV10HootTheOwl`.
 * Those are two names for one address (ov094 0x02136a58, both in symbols.txt),
 * EAD's internal name and the tree's English one, so the difference was only
 * ever in the source.
 */
#include "HootTheOwl.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~HootTheOwl() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" HootTheOwl *_ZN10HootTheOwlD0Ev(HootTheOwl *thiz)
{
    thiz->HootTheOwl::~HootTheOwl();    /* the D1 body, through the one host symbol */
    HootTheOwl::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
HootTheOwl::~HootTheOwl()
{
}
#endif
