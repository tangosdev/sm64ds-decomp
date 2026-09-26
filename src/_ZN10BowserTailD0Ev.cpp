//cpp
// @symbol _ZN10BowserTailD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. The
 * hand-written version spelled that out -- store the vtable, call the member's
 * destructor, chain to dActor_c, call Memory::Deallocate. All four come from the same
 * `~BowserTail()` the D1 file declares; the deallocation is the inline
 * dActor_c::operator delete, which is why nothing here mentions the heap.
 *
 * The identical body in both files is not duplication: D1 and D0 are two of the
 * three functions the compiler emits from one destructor, and each file is bound
 * to one of them by config/arm9/overlays/ov060/delinks.txt.
 */
#include "BowserTail.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~BowserTail() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" BowserTail *_ZN10BowserTailD0Ev(BowserTail *thiz)
{
    thiz->BowserTail::~BowserTail();    /* the D1 body, through the one host symbol */
    BowserTail::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BowserTail::~BowserTail()
{
}
#endif
