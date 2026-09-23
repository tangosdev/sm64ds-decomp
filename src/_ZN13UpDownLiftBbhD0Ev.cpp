//cpp
// @symbol _ZN13UpDownLiftBbhD0Ev
/* recovered: deleting ABI variant emitted from the real C++ destructor
 *
 * The source body is the same empty destructor as D1. mwccarm supplies the
 * vptr transitions, inherited member/base teardown and dActor_c's actor-heap
 * operator delete wrapper.
 */
#include "UpDownLiftBbh.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~UpDownLiftBbh() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" UpDownLiftBbh *_ZN13UpDownLiftBbhD0Ev(UpDownLiftBbh *thiz)
{
    thiz->UpDownLiftBbh::~UpDownLiftBbh();  /* the D1 body, through the one host symbol */
    UpDownLiftBbh::operator delete(thiz);   /* the class-specific delete D0 ends with */
    return thiz;
}
#else
UpDownLiftBbh::~UpDownLiftBbh()
{
}
#endif
