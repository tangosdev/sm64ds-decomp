//cpp
// @symbol _ZN13RacingPenguinD0Ev
/* D0 and D1 are compiler-emitted variants of this one real destructor. Each
 * remains isolated in its own build entry until the original TU is rebuilt. */
#include "RacingPenguin.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~RacingPenguin() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" RacingPenguin *_ZN13RacingPenguinD0Ev(RacingPenguin *thiz)
{
    thiz->RacingPenguin::~RacingPenguin();  /* the D1 body, through the one host symbol */
    RacingPenguin::operator delete(thiz);   /* the class-specific delete D0 ends with */
    return thiz;
}
#else
RacingPenguin::~RacingPenguin()
{
}
#endif
