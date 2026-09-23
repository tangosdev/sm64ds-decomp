//cpp
// @symbol _ZN9SpikeBombD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. Both
 * halves come from the same `~SpikeBomb()` the D1 file declares; the
 * deallocation is the inline dActor_c::operator delete, which is why nothing here
 * mentions the heap.
 */
#include "SpikeBomb.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~SpikeBomb() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" SpikeBomb *_ZN9SpikeBombD0Ev(SpikeBomb *thiz)
{
    thiz->SpikeBomb::~SpikeBomb();     /* the D1 body, through the one host symbol */
    SpikeBomb::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
SpikeBomb::~SpikeBomb()
{
}
#endif
