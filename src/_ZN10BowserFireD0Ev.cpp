//cpp
// @symbol _ZN10BowserFireD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 1 here (BowserFire's chain starts at dEnemyBase_c): destroy, then
 * return the object to the actor heap. The hand-written version spelled that out --
 * store the vtable, call the three member destructors, chain to dEnemyBase_c, call
 * Memory::Deallocate. All of it comes from the same `~BowserFire()` the D1 file
 * declares; the deallocation is the inline dEnemyBase_c::operator delete, which is why
 * nothing here mentions the heap.
 *
 * The identical body in both files is not duplication: D1 and D0 are two of the
 * three functions the compiler emits from one destructor, and each file is bound
 * to one of them by config/arm9/overlays/ov060/delinks.txt.
 */
#include "BowserFire.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~BowserFire() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" BowserFire *_ZN10BowserFireD0Ev(BowserFire *thiz)
{
    thiz->BowserFire::~BowserFire();    /* the D1 body, through the one host symbol */
    BowserFire::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
BowserFire::~BowserFire()
{
}
#endif
