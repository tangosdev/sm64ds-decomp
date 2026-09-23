//cpp
// @symbol _ZN17ExtendingPlatformD0Ev
/* Real compiler-spelled deleting destructor. dActor_c's inline operator delete
 * supplies the actor-heap release after complete destruction. */
#include "ExtendingPlatform.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~ExtendingPlatform() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" ExtendingPlatform *_ZN17ExtendingPlatformD0Ev(ExtendingPlatform *thiz)
{
    thiz->ExtendingPlatform::~ExtendingPlatform();  /* the D1 body, through the one host symbol */
    ExtendingPlatform::operator delete(thiz);       /* the class-specific delete D0 ends with */
    return thiz;
}
#else
ExtendingPlatform::~ExtendingPlatform()
{
}
#endif
