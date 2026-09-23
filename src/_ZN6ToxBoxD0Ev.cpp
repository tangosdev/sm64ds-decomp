//cpp
// @symbol _ZN6ToxBoxD0Ev
/* recovered: native deleting destructor -- the same typed ownership generates
 * the full teardown followed by inherited actor-heap deallocation. */
#include "ToxBox.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~ToxBox() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" ToxBox *_ZN6ToxBoxD0Ev(ToxBox *thiz)
{
    thiz->ToxBox::~ToxBox();        /* the D1 body, through the one host symbol */
    ToxBox::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
ToxBox::~ToxBox()
{
}
#endif
