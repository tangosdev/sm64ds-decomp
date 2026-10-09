//cpp
// @symbol _ZN5dBgChD0Ev
/* recovered: real C++ deleting destructor
 *
 * D0 is the DELETING destructor: run the destructor body, then return the
 * object to its heap. Nobody writes that; the same empty `~dBgCh()` the D1
 * file defines emits D2, D0 and D1 together, and objisolate keeps the one
 * this file is bound to. The ROM carries this one at 0x020354e0 (currently
 * `func_020354e0`). The heap hand-off is the family's inline operator delete,
 * which the header declares.
 */
#include "dBgCh.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dBgCh() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dBgCh *_ZN5dBgChD0Ev(dBgCh *thiz)
{
    thiz->dBgCh::~dBgCh();         /* the D1 body, through the one host symbol */
    dBgCh::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dBgCh::~dBgCh()
{
}
#endif
