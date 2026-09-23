//cpp
// @symbol _ZN12WorkElevatorD0Ev
/* recovered: real C++ deleting destructor
 *
 * mwccarm generates D0 from the same source destructor as D1, then resolves
 * the inherited actor delete path. objisolate retains only this file's D0
 * contribution and rebinds its vtable references to the ROM-owned table.
 */
#include "WorkElevator.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~WorkElevator() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" WorkElevator *_ZN12WorkElevatorD0Ev(WorkElevator *thiz)
{
    thiz->WorkElevator::~WorkElevator();  /* the D1 body, through the one host symbol */
    WorkElevator::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
WorkElevator::~WorkElevator()
{
}
#endif
