//cpp
// @symbol _ZN16BowserShockwavesD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. All
 * eight member destructors, the base chain and the deallocation come from the same
 * `~BowserShockwaves()` the D1 file declares; the deallocation is the inline
 * dActor_c::operator delete, which is why nothing here mentions the heap.
 */
#include "BowserShockwaves.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~BowserShockwaves() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" BowserShockwaves *_ZN16BowserShockwavesD0Ev(BowserShockwaves *thiz)
{
    thiz->BowserShockwaves::~BowserShockwaves();  /* the D1 body, through the one host symbol */
    BowserShockwaves::operator delete(thiz);      /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BowserShockwaves::~BowserShockwaves()
{
}
#endif
