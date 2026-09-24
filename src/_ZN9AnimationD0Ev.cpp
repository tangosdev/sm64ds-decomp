//cpp
// @symbol _ZN9AnimationD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same
 * one `Animation::~Animation()` definition. The class `operator delete` in the header
 * is what makes the tail call land on the right deallocator. */
#include "Animation.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~Animation() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" Animation *_ZN9AnimationD0Ev(Animation *thiz)
{
    thiz->Animation::~Animation();     /* the D1 body, through the one host symbol */
    Animation::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
Animation::~Animation()
{
}
#endif
