//cpp
// @symbol _ZN8MadPianoD0Ev
/* The deleting variant is generated from the same native destructor and the
 * actor hierarchy's inherited operator delete. */
#include "MadPiano.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~MadPiano() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" MadPiano *_ZN8MadPianoD0Ev(MadPiano *thiz)
{
    thiz->MadPiano::~MadPiano();      /* the D1 body, through the one host symbol */
    MadPiano::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
MadPiano::~MadPiano()
{
}
#endif
